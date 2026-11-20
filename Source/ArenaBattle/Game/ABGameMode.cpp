// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/ABGameMode.h"
#include "ABGameMode.h"
#include "Player/ABPlayerController.h"

#include <ArenaBattle.h>
#include "ABGameState.h"

AABGameMode::AABGameMode()
{
	static ConstructorHelpers::FClassFinder<APawn> DefaultPawnClassRef(TEXT("/Script/Engine.Blueprint'/Game/ArenaBattle/Blueprint/BP_ABCharacterPlayer.BP_ABCharacterPlayer_C'"));
	if (DefaultPawnClassRef.Class)
	{
		DefaultPawnClass = DefaultPawnClassRef.Class;
	}

	static ConstructorHelpers::FClassFinder<APlayerController> PlayerControllerClassRef(TEXT("/Script/ArenaBattle.ABPlayerController"));
	if (PlayerControllerClassRef.Class)
	{
		PlayerControllerClass = PlayerControllerClassRef.Class;
	}

	GameStateClass = AABGameState::StaticClass();
}

void AABGameMode::OnPlayerDead()
{

}

void AABGameMode::PreLogin(const FString& Options, const FString& Address, const FUniqueNetIdRepl& UniqueId, FString& ErrorMessage)
{
	AB_LOG(LogABNetwork, Log, TEXT("%s"), TEXT("======================================================="));
	AB_LOG(LogABNetwork, Log, TEXT("%s"), TEXT("Begin"));

	Super::PreLogin(Options, Address, UniqueId, ErrorMessage);

	// NetDriver 가져오기
	UNetDriver* NetDriver = GetNetDriver();
	if (NetDriver)
	{
		// 서버 입장에서 클라이언트의 접속이 있는지 확인
		if (NetDriver->ClientConnections.Num() == 0)
		{
			AB_LOG(LogABNetwork, Log, TEXT("%s"), TEXT("No Client Connections"));
		}
		else
		{
			for (const auto& Connection : NetDriver->ClientConnections)
			{
				AB_LOG(LogABNetwork, Log, TEXT("Client Connection: %s"), *Connection->GetName());
			}
		}
	}
	else
	{
		AB_LOG(LogABNetwork, Log, TEXT("%s"), TEXT("NoNetDriver"));
	}

	// 클라이언트 접속 거부 처리
	// ErrorMessage 값에 빈 문자열이 아닌 값을 설정하면 접속 거부 처리 가능
	// GameModeBase.h의 PreLogin 함수 선언의 설명 참고
	//ErrorMessage = TEXT("Server is full");

	AB_LOG(LogABNetwork, Log, TEXT("%s"), TEXT("End"));
}

APlayerController* AABGameMode::Login(UPlayer* NewPlayer, ENetRole InRemoteRole, const FString& Portal, const FString& Options, const FUniqueNetIdRepl& UniqueId, FString& ErrorMessage)
{
	AB_LOG(LogABNetwork, Log, TEXT("%s"), TEXT("Begin"));

	APlayerController* NewPlayerController = Super::Login(NewPlayer, InRemoteRole, Portal, Options, UniqueId, ErrorMessage);

	AB_LOG(LogABNetwork, Log, TEXT("%s"), TEXT("End"));

	return NewPlayerController;
}

void AABGameMode::PostLogin(APlayerController* NewPlayer)
{
	AB_LOG(LogABNetwork, Log, TEXT("%s"), TEXT("Begin"));

	Super::PostLogin(NewPlayer);

	AB_LOG(LogABNetwork, Log, TEXT("%s"), TEXT("End"));
}

void AABGameMode::StartPlay()
{
	AB_LOG(LogABNetwork, Log, TEXT("%s"), TEXT("Begin"));

	Super::StartPlay();

	AB_LOG(LogABNetwork, Log, TEXT("%s"), TEXT("End"));
}
