// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameState.h"
#include "SpartaGameState.generated.h"

USTRUCT(BlueprintType)
struct FWaveData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wave")
	int32 WaveNumber =1;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wave")
	float WaveTime = 30.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wave")
	int32 ItemsToSpawn = 10;

	FWaveData()
	{
		WaveNumber = 1;
		WaveTime = 30.0f;
		ItemsToSpawn = 10;
	}
};


UCLASS()
class PR_SPARTAPROJECT_API ASpartaGameState : public AGameState
{
	GENERATED_BODY()

public:
	ASpartaGameState();
	
	virtual void BeginPlay() override;

	// 기존 변수 
	UPROPERTY(visibleAnywhere, BlueprintReadWrite, Category = "Score")
	int32 Score;
	UPROPERTY(visibleAnywhere, BlueprintReadOnly, Category = "Coin")
	int32 CollectedCoinCount;
	UPROPERTY(visibleAnywhere, BlueprintReadOnly, Category = "Coin")
	int32 SpawnedCoinCount;
	// 레벨 관련 
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Level")
	float LevelDuration;
	UPROPERTY(visibleAnywhere, BlueprintReadOnly, Category = "Level")
	int32 CurrentLevelIndex;
	UPROPERTY(visibleAnywhere, BlueprintReadOnly, Category = "Level")
	int32 MaxLevels;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Level")
	TArray<FName> LevelMapNames;

	// 웨이브 데이터 목록 ( 에디터에서 설정 )
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wave")
	TArray<FWaveData> Waves;
	// 현재 진행 중인 웨이브 인덱스 
	UPROPERTY(visibleAnywhere, BlueprintReadOnly, Category = "Wave")
	int32 CurrentWaveIndex;
	//남은 시간
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Wave")
	float RemainingWaveTime;
	// 웨이브 작동여부 
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Wave")
	bool bIsWaveActive;
	// 웨이브 완료여부
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Wave")
	bool bAllWavesCompleted;

	// 타이머 핸들 
	FTimerHandle LevelTimerHandle;
	FTimerHandle HUDUpdateTimerHandle;
	FTimerHandle WaveTimerHandle;
	
	UFUNCTION(BlueprintPure, Category = "Score")
	int32 GetScore() const;
	UFUNCTION(BlueprintCallable, Category = "Score")
	void AddScore(int32 Amount);

	UFUNCTION(BlueprintCallable, Category = "Level")
	void OnGameOver();
	
	void StartLevel();
	void OnLevelTimeUp();
	void EndLevel();
	void UpdateHUD();

	// 웨이브 관련 새 함수
	void StartWave();
	void AdvanceWave();
	void OnWaveTimeUp();
	const FWaveData* GetCurrentWaveData() const;

	void OnCoinCollected();
	void CompleteAllWaves();
};
