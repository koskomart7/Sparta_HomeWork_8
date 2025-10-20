// Fill out your copyright notice in the Description page of Project Settings.


#include "SpartaGameMode.h"
#include "SpartaCharacter.h"
#include "SpartaGameState.h"
#include "SpartaPlayerController.h"

ASpartaGameMode::ASpartaGameMode()
{
	DefaultPawnClass = ASpartaCharacter::StaticClass();	// Uclass 형태의 타입 반환. 리플렉션시스템 지원
	PlayerControllerClass = ASpartaPlayerController::StaticClass();
	GameStateClass = ASpartaGameState::StaticClass();

	SpartaGameState = nullptr;
}

void ASpartaGameMode::BeginPlay()
{
	Super::BeginPlay();
	SpartaGameState = GetGameState<ASpartaGameState>();
	InitializeWaves();
}

void ASpartaGameMode::InitializeWaves()
{
	if (SpartaGameState)
	{
		//웨이브 데이터가 비어있다면 기본값 설정
		if (SpartaGameState->Waves.Num() == 0)
		{
			FWaveData Wave1;
			Wave1.WaveNumber = 1;
			Wave1.WaveTime = 30.0f;
			Wave1.ItemsToSpawn = 15;

			FWaveData Wave2;
			Wave2.WaveNumber = 2;
			Wave2.WaveTime = 40.0f;
			Wave2.ItemsToSpawn = 35;

			FWaveData Wave3;
			Wave3.WaveNumber = 3;
			Wave3.WaveTime = 50.0f;
			Wave3.ItemsToSpawn = 55;

			SpartaGameState->Waves.Add(Wave1);
			SpartaGameState->Waves.Add(Wave2);
			SpartaGameState->Waves.Add(Wave3);
		}
	}
}
