// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ABFountain.generated.h"

UCLASS()
class ARENABATTLE_API AABFountain : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AABFountain();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	/*
	virtual void OnActorChannelOpen(class FInBunch& InBunch, class UNetConnection* Connection) override;

	virtual bool IsNetRelevantFor(const AActor* RealViewer, const AActor* ViewTarget, const FVector& SrcLocation) const override;

	virtual void PreReplication(IRepChangedPropertyTracker& ChangedPropertyTracker) override;
	//*/
	
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Mesh)
	TObjectPtr<class UStaticMeshComponent> Body;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Mesh)
	TObjectPtr<class UStaticMeshComponent> Water;

	// 속성 변경됐을 때 호툴될 콜백 함수 -> 클라이언트에서 값이 변경될 떄 호출
	UFUNCTION()
	void OnRep_ServerRotationYaw();

	UFUNCTION()
	void OnRep_ServerLightColor();

	// Multicast RPC 함수
	UFUNCTION(NetMulticast, Unreliable)
	void MulticastRPCChangeLightColor(const FLinearColor& NewLightColor);

	UFUNCTION(Server, Unreliable, WithValidation)
	void ServerRPCChangeLightColor();

	UFUNCTION(Client, Unreliable)
	void ClientRPCChangeLightColor(const FLinearColor& NewLightColor);

	// 회전에 사용할 속성 (리플리케이션 옵션 지정)
	UPROPERTY(ReplicatedUsing = OnRep_ServerRotationYaw)
	float ServerRotationYaw;

	UPROPERTY(ReplicatedUsing = OnRep_ServerLightColor)
	FLinearColor ServerLightColor;

	// 회전 속도
	float RotationRate = 30.0f;

	// 서버로부터 데이터를 받은 후에 시간이 얼마나 경과했는지를 기록
	float ClientTimeSinceUpdate = 0.0f;

	// 서버로부터 데이터를 받고, 그 다음 데이터를 받았을 때까지 걸린 시간
	float ClientTimeBetweenLastUpdate = 0.0f;

	// 의도적으로 네트워크를 포화 상태로 만들기 위한 변수
	UPROPERTY(Replicated)
	TArray<float> BigData;

	float BigDataElement = 0.0f;
};
