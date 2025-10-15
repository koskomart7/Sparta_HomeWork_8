// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TimerManager.h"
#include "RotatingPlatform.generated.h"

UCLASS()
class PR_SPARTAPROJECT_API ARotatingPlatform : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ARotatingPlatform();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = " RotatingPlatform|Components")
	USceneComponent* SceneRoot;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = " RotatingPlatform|Components")
	UStaticMeshComponent* StaticMeshComp;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	// 기본 회전 타이머 
	FTimerHandle RotationTimerHandle;

	// 회전 설정
	UPROPERTY(editAnywhere, BlueprintReadWrite, Category = "RotatingPlatform|Settings")
	float RotationSpeed = 90.0f;

	UPROPERTY(editAnywhere, BlueprintReadWrite, Category = "RotatingPlatform|Settings")
	float RotationInterval = 0.02f;

	UPROPERTY(editAnywhere, BlueprintReadWrite, Category = "RotatingPlatform|Settings")
	FVector RotationAxis = FVector(0.0f, 0.0f, 1.0f);

	// 도전 과제 1: 타이머 시스템 속성들
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "RotatingPlatform|Timer Challenge")
	bool bEnablePeriodicSpeedChange = false;  // 주기적 속도 변경

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "RotatingPlatform|Timer Challenge")
	float SpeedChangeInterval = 3.0f;  // 속도 변경 간격

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "RotatingPlatform|Timer Challenge")
	float MinRotationSpeed = 30.0f;  // 최소 회전 속도

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "RotatingPlatform|Timer Challenge")
	float MaxRotationSpeed = 180.0f;  // 최대 회전 속도

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "RotatingPlatform|Timer Challenge")
	bool bEnableDisappearing = false;  // 사라지기 기능

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "RotatingPlatform|Timer Challenge")
	float DisappearDelay = 5.0f;  // 사라지기 전 대기 시간

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "RotatingPlatform|Timer Challenge")
	float DisappearDuration = 3.0f;  // 사라진 상태 유지 시간

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "RotatingPlatform|Timer Challenge")
	bool bEnableAxisChange = false;  // 회전축 변경 기능

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "RotatingPlatform|Timer Challenge")
	float AxisChangeInterval = 4.0f;  // 회전축 변경 간격

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "RotatingPlatform|Timer Challenge")
	TArray<FVector> PresetRotationAxes;  // 미리 정의된 회전축들

	// 추가 타이머 핸들들
	FTimerHandle SpeedChangeTimerHandle;
	FTimerHandle DisappearTimerHandle;
	FTimerHandle ReappearTimerHandle;
	FTimerHandle AxisChangeTimerHandle;

	// 상태 변수들
	bool bIsDisappeared = false;
	int32 CurrentAxisIndex = 0;

	// 타이머 기반 함수들
	UFUNCTION()
	void RotatePlatform();

	UFUNCTION()
	void ChangeRotationSpeed();

	UFUNCTION()
	void DisappearPlatform();

	UFUNCTION()
	void ReappearPlatform();

	UFUNCTION()
	void ChangeRotationAxis();

	// 유틸리티 함수들
	void SetupAdvancedTimers();
	FVector GetRandomRotationAxis();
	float GetRandomRotationSpeed();
	
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// 공개 제어 함수들
	UFUNCTION(BlueprintCallable, Category = "RotatingPlatform|Control")
	void StartPeriodicSpeedChange();

	UFUNCTION(BlueprintCallable, Category = "RotatingPlatform|Control")
	void StopPeriodicSpeedChange();

	UFUNCTION(BlueprintCallable, Category = "RotatingPlatform|Control")
	void TriggerDisappear();

	UFUNCTION(BlueprintCallable, Category = "RotatingPlatform|Control")
	void StartAxisChanges();

	UFUNCTION(BlueprintCallable, Category = "RotatingPlatform|Control")
	void StopAxisChanges();

	UFUNCTION(BlueprintCallable, Category = "RotatingPlatform|Control")
	void SetRandomProperties(float NewRotationSpeed = -1.0f, FVector NewRotationAxis = FVector::ZeroVector);
};
