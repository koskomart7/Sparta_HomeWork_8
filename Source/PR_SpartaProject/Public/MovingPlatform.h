// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TimerManager.h"
#include "MovingPlatform.generated.h"

UCLASS()
class PR_SPARTAPROJECT_API AMovingPlatform : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AMovingPlatform();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite,	Category="MoveActor|Components")
	USceneComponent* SceneRoot;
	UPROPERTY(EditAnywhere, BlueprintReadWrite,	Category="MoveActor|Components")
	UStaticMeshComponent* StaticMeshComp;

	UPROPERTY(EditAnywhere, BlueprintReadWrite,	Category="MoveActor|Properties")
	float RotationSpeed = 100.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite,	Category="MoveActor|Properties")
	FVector MoveSpeed = FVector(500.0f, 0.0f, 0.0f);

	UPROPERTY(EditAnywhere, BlueprintReadWrite,	Category="MoveActor|Properties")
	float MaxRange = 200.0f;
	
	FVector StartLocation;

	// 타이머 시스템 관련 속성들
	UPROPERTY(editAnywhere, BlueprintReadWrite,	Category="MoveActor|Timer Settings")
	bool bUseTimerBasedMovement = false;

	UPROPERTY(editAnywhere, BlueprintReadWrite,	Category="MoveActor|Timer Settings")
	float MovementInterval = 2.0f;	// 이동 간격

	UPROPERTY(editAnywhere, BlueprintReadWrite,	Category="MoveActor|Timer Settings")
	bool bEnableDisappearing = false;	// 사라지기 기능 활성화

	UPROPERTY(editAnywhere, BlueprintReadWrite,	Category="MoveActor|Timer Settings")
	float DisappearDelay = 3.0f;	// 사라지기 전 대기시간

	UPROPERTY(editAnywhere, BlueprintReadWrite,	Category="MoveActor|Timer Settings")
	float DisappearDuration = 2.0f;		// 사라진 상태 유지 시간

	UPROPERTY(editAnywhere, BlueprintReadWrite,	Category="MoveActor|Timer Settings")
	bool bPeriodicLocationChange = false;	// 주기적 위치 변경

	UPROPERTY(editAnywhere, BlueprintReadWrite,	Category="MoveActor|Timer Settings")
	float LocationChangeInterval = 5.0f;	// 위치 변경 간격

	UPROPERTY(editAnywhere, BlueprintReadWrite,	Category="MoveActor|Timer Settings")
	TArray<FVector> PresetLocations;	// 미리 정의된 위치들

	// 타이머 핸들
	FTimerHandle MovementTimerHandle;
	FTimerHandle DisappearTimerHandle;
	FTimerHandle ReappearTimerHandle;
	FTimerHandle LocationChangeTimerHandle;

	// 상태 변수들
	bool bIsDisappeared = false;
	bool bIsMovingToTarget = false;
	FVector TargetLocation;
	int32 CurrentLocationIndex = 0;

	// 타이머 기반 함수들
	UFUNCTION()
	void TimerBasedMove();

	UFUNCTION()
	void DisappearPlatform();

	UFUNCTION()
	void ReappearPlatform();

	UFUNCTION()
	void ChangeToRandomLocation();

	UFUNCTION()
	void StartPeriodicLocationChange();

	// 유틸리티 함수들
	void SetupTimers();
	FVector GetRandomLocationInRange(float Range = 1000.0f);
	void MoveToTarget(float DeltaTime);


public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Setter 함수들 (PuzzleSpawnManager에서 호출 가능)
	UFUNCTION(BlueprintCallable, Category = "MoveActor|Properties")
	void SetRotationSpeed(float NewRotationSpeed) { RotationSpeed = NewRotationSpeed; }

	UFUNCTION(BlueprintCallable, Category = "MoveActor|Properties")
	void SetMoveSpeed(const FVector& NewMoveSpeed) { MoveSpeed = NewMoveSpeed; }

	UFUNCTION(BlueprintCallable, Category = "MoveActor|Properties")
	void SetMaxRange(float NewMaxRange) { MaxRange = NewMaxRange; }

	UFUNCTION(BlueprintCallable, Category = "MoveActor|Timer Settings")
	void SetTimerBasedMovement(bool bEnabled) { bUseTimerBasedMovement = bEnabled; }

	UFUNCTION(BlueprintCallable, Category = "MoveActor|Timer Settings")
	void SetMovementInterval(float NewInterval) { MovementInterval = NewInterval; }

	UFUNCTION(BlueprintCallable, Category = "MoveActor|Timer Settings")
	void SetDisappearSettings(bool bEnabled, float Delay, float Duration)
	{
		bEnableDisappearing = bEnabled;
		DisappearDelay = Delay;
		DisappearDuration = Duration;
	}

	// Getter 함수들
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "MoveActor|Properties")
	float GetRotationSpeed() const { return RotationSpeed; }

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "MoveActor|Properties")
	FVector GetMoveSpeed() const { return MoveSpeed; }

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "MoveActor|Properties")
	float GetMaxRange() const { return MaxRange; }
	
	// 공개 함수들
	UFUNCTION(BlueprintCallable, Category = "MoveActor|Control")
	void StartTimerBasedMovement();

	UFUNCTION(BlueprintCallable, Category = "MoveActor|Control")
	void StopTimerBasedMovement();

	UFUNCTION(BlueprintCallable, Category = "MoveActor|Control")
	void TriggerDisappear();

	UFUNCTION(BlueprintCallable, Category = "MoveActor|Control")
	void StartLocationChanges();

	UFUNCTION(BlueprintCallable, Category = "MoveActor|Control")
	void StopLocationChanges();
	
	
	
};
