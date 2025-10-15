// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TimerManager.h"
#include "PuzzleSpawnManager.generated.h"

UCLASS()
class PR_SPARTAPROJECT_API APuzzleSpawnManager : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	APuzzleSpawnManager();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	// 스폰할 클래스들
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SpawnActor|Spawn Settings")
	TArray<TSubclassOf<AActor>> PlatformClasses;

	// 스폰 범위 설정
	UPROPERTY(editAnywhere, BlueprintReadWrite, Category = "SpawnActor|Spawn Settings")
	FVector SpawnAreaMin = FVector(-1000, -1000, 0);
	UPROPERTY(editAnywhere, BlueprintReadWrite, Category = "SpawnActor|Spawn Settings")
	FVector SpawnAreaMax = FVector(1000, 1000, 500);

	UPROPERTY(editAnywhere, BlueprintReadWrite, Category = "SpawnActor|Spawn Settings")
	int32 MinPlatforms = 5;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SpawnActor|Spawn Settings")
	int32 MaxPlatforms = 20;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SpawnActor|Spawn Settings")
	float MinSpawnDistance = 200.0f;	// 플랫폼 간 최소 거리

	// 도전 과제 2: 랜덤 퍼즐 생성 강화 설정들
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SpawnActor|Random Challenge")
	bool bEnableDynamicRespawn = false;  // 동적 재스폰 활성화

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SpawnActor|Random Challenge")
	float DynamicRespawnInterval = 10.0f;  // 동적 재스폰 간격

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SpawnActor|Random Challenge")
	bool bEnableWaveSystem = false;  // 웨이브 시스템 활성화

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SpawnActor|Random Challenge")
	int32 WaveCount = 3;  // 총 웨이브 수

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SpawnActor|Random Challenge")
	float WaveInterval = 15.0f;  // 웨이브 간격

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SpawnActor|Random Challenge")
	bool bIncreaseComplexityPerWave = true;  // 웨이브마다 복잡도 증가

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SpawnActor|Random Challenge")
	bool bEnableAreaBasedSpawning = false;  // 구역 기반 스폰

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SpawnActor|Random Challenge")
	TArray<FVector> SpawnZoneCenters;  // 스폰 구역 중심점들

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SpawnActor|Random Challenge")
	float SpawnZoneRadius = 300.0f;  // 각 구역의 반지름

	// 랜덤 속성 범위 설정
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SpawnActor|Random Properties")
	FVector2D RotationSpeedRange = FVector2D(30.0f, 180.0f);  // 회전 속도 범위

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SpawnActor|Random Properties")
	FVector2D MovementSpeedRange = FVector2D(100.0f, 800.0f);  // 이동 속도 범위

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SpawnActor|Random Properties")
	FVector2D MovementRangeMinMax = FVector2D(100.0f, 500.0f);  // 이동 범위

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SpawnActor|Random Properties")
	FVector2D ScaleRange = FVector2D(0.5f, 3.0f);  // 스케일 범위

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SpawnActor|Random Properties")
	bool bRandomizeTimerIntervals = true;  // 타이머 간격 랜덤화

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SpawnActor|Random Properties")
	FVector2D TimerIntervalRange = FVector2D(1.0f, 5.0f);  // 타이머 간격 범위

	// 생성된 액터들 추적
	UPROPERTY()
	TArray<AActor*> SpawnedPlatforms;

	// 타이머 핸들들
	FTimerHandle DynamicRespawnTimerHandle;
	FTimerHandle WaveSpawnTimerHandle;

	// 웨이브 시스템 변수들
	int32 CurrentWave = 0;
	bool bWaveSystemActive = false;
	
	// 스폰 함수들
	
	
	

	UFUNCTION(BlueprintCallable, Category = "SpawnActor|Wave System")
	void StartWaveSystem();

	UFUNCTION(BlueprintCallable, Category = "SpawnActor|Wave System")
	void StopWaveSystem();

	UFUNCTION()
	void SpawnNextWave();

	UFUNCTION()
	void DynamicRespawn();
	
	// 랜덤 속성 설정 함수들
	void SetAdvancedRandomProperties(AActor* Platform, int32 ComplexityLevel = 1);

	bool IsValidSpawnLocation(const FVector& Location);
	FVector GetRandomSpawnLocation();
	FVector GetZoneBasedSpawnLocation();
	int32 GetComplexityForCurrentWave();

	// 로그라이크 시스템 함수들
	void GenerateRandomConfiguration();
	void CreateDifficultySpike(int32 WaveNumber);
	
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// 공개 제어 함수들
	UFUNCTION(BlueprintCallable, Category = "SpawnActor|Spawn")
	void SpawnRandomPuzzle();

	UFUNCTION(BlueprintCallable, Category = "SpawnActor|Spawn")
	void ClearSpawnedPlatforms();
	
	UFUNCTION(BlueprintCallable, Category = "SpawnActor|Control")
	void StartDynamicRespawning();

	UFUNCTION(BlueprintCallable, Category = "SpawnActor|Control")
	void StopDynamicRespawning();

	UFUNCTION(BlueprintCallable, Category = "SpawnActor|Control")
	void SpawnSpecificWave(int32 WaveNumber);

	UFUNCTION(BlueprintCallable, Category = "SpawnActor|Control")
	void AddSpawnZone(FVector ZoneCenter);

	UFUNCTION(BlueprintCallable, Category = "SpawnActor|Control")
	void ClearSpawnZones();

	// 정보 조회 함수들
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "SpawnActor|Info")
	int32 GetSpawnedPlatformCount() const { return SpawnedPlatforms.Num(); }

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "SpawnActor|Info")
	int32 GetCurrentWave() const { return CurrentWave; }

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "SpawnActor|Info")
	bool IsWaveSystemActive() const { return bWaveSystemActive; }
};
