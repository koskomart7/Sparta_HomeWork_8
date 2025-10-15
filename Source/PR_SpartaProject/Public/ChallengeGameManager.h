// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TimerManager.h"
#include "Engine/Engine.h"
#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"
#include "ChallengeGameManager.generated.h"

// 전방 선언 추가
class APuzzleSpawnManager;
class AMovingPlatform;
class ARotatingPlatform;
class AAdvanced_MovingActor;

UCLASS()
class PR_SPARTAPROJECT_API AChallengeGameManager : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AChallengeGameManager();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// 도전 과제 1: 타이머 시스템 관리
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Challenge 1|Timer System")
	bool bEnableTimerChallenge = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Challenge 1|Timer System")
	float GlobalTimerInterval = 5.0f;  // 전역 타이머 간격

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Challenge 1|Timer System")
	bool bEnableGlobalDisappearing = true;  // 전역 사라지기 기능

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Challenge 1|Timer System")
	float DisappearWaveInterval = 8.0f;  // 사라지기 웨이브 간격

	// 도전 과제 2: 랜덤 퍼즐 시스템 관리
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Challenge 2|Random System")
	bool bEnableRandomChallenge = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Challenge 2|Random System")
	class APuzzleSpawnManager* PuzzleSpawnManager;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Challenge 2|Random System")
	float AutoReshuffleInterval = 30.0f;  // 자동 재배치 간격

	// 타이머 핸들들
	FTimerHandle GlobalTimerHandle;
	FTimerHandle DisappearWaveTimerHandle;
	FTimerHandle ReshuffleTimerHandle;
	
	// 도전 과제 함수들
	UFUNCTION()
	void ExecuteTimerChallenge();

	UFUNCTION()
	void TriggerDisappearWave();

	UFUNCTION()
	void AutoReshuffle();

	// 현재 활성 플랫폼들 추적
	UPROPERTY()
	TArray<AActor*> ActivePlatforms;

	// 유틸리티 함수들 
	void UpdateActivePlatformsList();
	void ApplyTimerEffectsToAllPlatforms();

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// 게임 제어 함수들
	UFUNCTION(BlueprintCallable, Category = "Game Control")
	void StartChallengeGame();

	UFUNCTION(BlueprintCallable,Category = "Game Control")
	void StopChallengeGame();

	UFUNCTION(BlueprintCallable, Category = "Challenge Control")
	void TriggerManualDisappearWave();

	UFUNCTION(BlueprintCallable, Category = "Challenge Control")
	void ForceReshuffle();
};
