// Fill out your copyright notice in the Description page of Project Settings.


#include "Prop/ABFountain.h"
#include "Components/StaticMeshComponent.h"
#include <ArenaBattle.h>

#include <Net/UnrealNetwork.h>

// Sets default values
AABFountain::AABFountain()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Body = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Body"));
	Water = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Water"));

	RootComponent = Body;
	Water->SetupAttachment(Body);
	Water->SetRelativeLocation(FVector(0.0f, 0.0f, 132.0f));

	static ConstructorHelpers::FObjectFinder<UStaticMesh> BodyMeshRef(TEXT("/Script/Engine.StaticMesh'/Game/ArenaBattle/Environment/Props/SM_Plains_Castle_Fountain_01.SM_Plains_Castle_Fountain_01'"));
	if (BodyMeshRef.Object)
	{
		Body->SetStaticMesh(BodyMeshRef.Object);
	}

	static ConstructorHelpers::FObjectFinder<UStaticMesh> WaterMeshRef(TEXT("/Script/Engine.StaticMesh'/Game/ArenaBattle/Environment/Props/SM_Plains_Fountain_02.SM_Plains_Fountain_02'"));
	if (WaterMeshRef.Object)
	{
		Water->SetStaticMesh(WaterMeshRef.Object);
	}

	// 리플리케이션 옵션 활성화
	//bReplicates = true;
	SetReplicates(true);

	// 네트워크 전송 빈도 설정 (1초에 한번씩)
	//NetUpdateFrequency = 1.0f;
	SetNetUpdateFrequency(1.0f);

	// 연관성 판정 거리 값
	// 20미터
	SetNetCullDistanceSquared(4000000.0f);
}

// Called when the game starts or when spawned
void AABFountain::BeginPlay()
{
	Super::BeginPlay();
	
	// 타이머를 횔용해 호직 반복 실행
	/* 서버에서만 실행
	if (HasAuthority())
	{
		FTimerHandle Handle;
		GetWorld()->GetTimerManager().SetTimer(
			Handle,
			FTimerDelegate::CreateLambda([&]()
				{
					ServerRotationYaw += 1.0f;
				}
			), 1.0f, true
		);
	}									//*/
}

void AABFountain::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	// 리플리케이션 대상 속성 추가
	DOREPLIFETIME(AABFountain, ServerRotationYaw);
}

void AABFountain::OnActorChannelOpen(FInBunch& InBunch, UNetConnection* Connection)
{
	AB_LOG(LogABNetwork, Log, TEXT("%s"), TEXT("Begin"));

	Super::OnActorChannelOpen(InBunch, Connection);

	AB_LOG(LogABNetwork, Log, TEXT("%s"), TEXT("End"));
}

bool AABFountain::IsNetRelevantFor(const AActor* RealViewer, const AActor* ViewTarget, const FVector& SrcLocation) const
{
	bool Result = Super::IsNetRelevantFor(RealViewer, ViewTarget, SrcLocation);

	if (!Result)
	{
		AB_LOG(LogABNetwork, Log, TEXT("Not Relavant! [%s] %s"), *RealViewer->GetName(), *SrcLocation.ToCompactString());
	}

	return Result;
}

// Called every frame
void AABFountain::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// 서버 / 클라이언트 로직 구분해서 구현 -> 이 액터가 존재하는 위치 구분
	// 서버인지 확인 -> HasAuthority
	if (HasAuthority())
	{
		// 회전 적용
		AddActorLocalRotation(FRotator(0.0f, RotationRate * DeltaTime, 0.0f));

		//변경된 회전 값을 복제(리플리케이션)되는 변수에 설정
		ServerRotationYaw = RootComponent->GetComponentRotation().Yaw;
	}
	else	// 클라이언트
	{
		/* 서버로부터 전달 받은 "변경된 회전 값"을 적용
		FRotator NewRotator = RootComponent->GetComponentRotation();

		// Yaw 회전 값은 서버로부터 전달 받은 값 설정
		NewRotator.Yaw = ServerRotationYaw;

		RootComponent->SetWorldRotation(NewRotator);	//*/

		// 서버로부터 데이터를 받은 이후에 경과한 시간 기록
		ClientTimeSinceUpdate += DeltaTime;

		// 서버로부터 데이터를 받은 이후에 경과한 시간이 얼마 안됐으면 종료
		if (ClientTimeBetweenLastUpdate < KINDA_SMALL_NUMBER)
		{
			return;
		}

		// 회전 예측 값 - 다음에 전달 받을 회전의 예측 값
		const float EstimateRotationYaw = ServerRotationYaw + RotationRate * ClientTimeBetweenLastUpdate;

		// 보간 시간 계산 (t 또는 alpha)
		const float LerpRatio = ClientTimeSinceUpdate / ClientTimeBetweenLastUpdate;

		// 클라이언트 보간
		const float ClientNewYaw = FMath::Lerp(ServerRotationYaw, EstimateRotationYaw, LerpRatio);

		FRotator ClientRotator = RootComponent->GetComponentRotation();
		ClientRotator.Yaw = ClientNewYaw;

		// 화전 값 설정
		RootComponent->SetWorldRotation(ClientRotator);
	}
}

void AABFountain::OnRep_ServerRotationYaw()
{
	// 로그 출력
	AB_LOG(LogABNetwork, Log, TEXT("Yaw: %f"), ServerRotationYaw);

	//* 서버로부터 전달 받은 "변경된 회전 값"을 적용
	FRotator NewRotator = RootComponent->GetComponentRotation();

	// Yaw 회전 값은 서버로부터 전달 받은 값 설정
	NewRotator.Yaw = ServerRotationYaw;

	// 루트 컴포넌트에 회전 설정
	RootComponent->SetWorldRotation(NewRotator);		//*/

	// 시간 갱신
	ClientTimeBetweenLastUpdate = ClientTimeSinceUpdate;

	// 경과 시간 초기화
	ClientTimeSinceUpdate = 0.0f;
}

