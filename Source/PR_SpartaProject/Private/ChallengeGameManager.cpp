// Fill out your copyright notice in the Description page of Project Settings.


#include "ChallengeGameManager.h"
#include "PuzzleSpawnManager.h"
#include "MovingPlatform.h"
#include "RotatingPlatform.h"
#include "Advanced_MovingActor.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
AChallengeGameManager::AChallengeGameManager()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

}

// Called when the game starts or when spawned
void AChallengeGameManager::BeginPlay()
{
	Super::BeginPlay();

	// PuzzleSpawnManager 자동 찾기
	if (!PuzzleSpawnManager)
	{
		PuzzleSpawnManager = Cast<APuzzleSpawnManager>(
			UGameplayStatics::GetActorOfClass(GetWorld(), APuzzleSpawnManager::StaticClass())
		);
	}

	// 자동 게임 시작
	if (bEnableTimerChallenge || bEnableRandomChallenge)
	{
		StartChallengeGame();
	}
	
}

void AChallengeGameManager::ExecuteTimerChallenge()
{
	UpdateActivePlatformsList();
	ApplyTimerEffectsToAllPlatforms();

	UE_LOG(LogTemp, Warning, TEXT("타이머 챌린지 실행됨 - 활성 액터: %d"), 
		ActivePlatforms.Num());
}

void AChallengeGameManager::TriggerDisappearWave()
{
	UpdateActivePlatformsList();

	// 랜덤하게 플랫폼들을 사라지게 함
	int32 PlatformsToDisappear = FMath::Max(1, ActivePlatforms.Num() / 3);
	
	for (int32 i = 0; i < PlatformsToDisappear && ActivePlatforms.Num() > 0; i++)
	{
		int32 RandomIndex = FMath::RandRange(0, ActivePlatforms.Num() - 1);
		AActor* SelectedPlatform = ActivePlatforms[RandomIndex];

		if (AMovingPlatform* MovePlatform = Cast<AMovingPlatform>(SelectedPlatform))
		{
			MovePlatform->TriggerDisappear();
		}
		else if (ARotatingPlatform* RotatePlatform = Cast<ARotatingPlatform>(SelectedPlatform))
		{
			RotatePlatform->TriggerDisappear();
		}
	}

	UE_LOG(LogTemp, Warning, TEXT("사라짐 웨이브가 트리거됨 - 영향을 받은 플랫폼 %d 개 "), PlatformsToDisappear);
}

void AChallengeGameManager::AutoReshuffle()
{
	if (PuzzleSpawnManager)
	{
		PuzzleSpawnManager->SpawnRandomPuzzle();
		UE_LOG(LogTemp, Warning, TEXT("Auto reshuffle completed - New puzzle generated"));
	}
}

void AChallengeGameManager::UpdateActivePlatformsList()
{
	ActivePlatforms.Empty();

	// MovingPlatform 찾기
	TArray<AActor*> MovingPlatforms;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AMovingPlatform::StaticClass(), MovingPlatforms);
	ActivePlatforms.Append(MovingPlatforms);

	// RotatingPlatform 찾기
	TArray<AActor*> RotatingPlatforms;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ARotatingPlatform::StaticClass(), RotatingPlatforms);
	ActivePlatforms.Append(RotatingPlatforms);

	// Advanced_MovingActor 찾기
	TArray<AActor*> AdvancedActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AAdvanced_MovingActor::StaticClass(), AdvancedActors);
	ActivePlatforms.Append(AdvancedActors);
}

void AChallengeGameManager::ApplyTimerEffectsToAllPlatforms()
{
	for (AActor* Platform : ActivePlatforms)
	{
		if (AMovingPlatform* MovePlatform = Cast<AMovingPlatform>(Platform))
		{
			// MovingPlatform 타이머 효과 적용
			if (FMath::RandBool())
			{
				MovePlatform->StartTimerBasedMovement();
			}
		}
		else if (ARotatingPlatform* RotatePlatform = Cast<ARotatingPlatform>(Platform))
		{
			// RotatingPlatform 타이머 효과 적용
			if (FMath::RandBool())
			{
				RotatePlatform->StartPeriodicSpeedChange();
			}
			if (FMath::RandBool())
			{
				RotatePlatform->StartAxisChanges();
			}
		}
	}
}

void AChallengeGameManager::StartChallengeGame()
{
	UE_LOG(LogTemp, Warning, TEXT("챌린지 게임 시작!"));

	// 도전 과제 1: 타이머 시스템 활성화
	if (bEnableTimerChallenge)
	{
		GetWorld()->GetTimerManager().SetTimer(
			GlobalTimerHandle,
			this,
			&AChallengeGameManager::ExecuteTimerChallenge,
			GlobalTimerInterval,
			true
		);

		if (bEnableGlobalDisappearing)
		{
			GetWorld()->GetTimerManager().SetTimer(
				DisappearWaveTimerHandle,
				this,
				&AChallengeGameManager::TriggerDisappearWave,
				DisappearWaveInterval,
				true
			);
		}
	}
	// 도전 과제 2: 랜덤 시스템 활성화
	if (bEnableRandomChallenge && PuzzleSpawnManager)
	{
		GetWorld()->GetTimerManager().SetTimer(
			ReshuffleTimerHandle,
			this,
			&AChallengeGameManager::AutoReshuffle,
			AutoReshuffleInterval,
			true
		);
	}
}

void AChallengeGameManager::StopChallengeGame()
{
	// 모든 타이머 정지
	GetWorld()->GetTimerManager().ClearTimer(GlobalTimerHandle);
	GetWorld()->GetTimerManager().ClearTimer(DisappearWaveTimerHandle);
	GetWorld()->GetTimerManager().ClearTimer(ReshuffleTimerHandle);
	
	UE_LOG(LogTemp, Warning, TEXT("Challenge Game Stopped!"));
}

void AChallengeGameManager::TriggerManualDisappearWave()
{
	TriggerDisappearWave();
}

void AChallengeGameManager::ForceReshuffle()
{
	AutoReshuffle();
}

// Called every frame
void AChallengeGameManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}
