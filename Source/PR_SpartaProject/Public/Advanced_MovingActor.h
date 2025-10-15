// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Advanced_MovingActor.generated.h"

UCLASS()
class PR_SPARTAPROJECT_API AAdvanced_MovingActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AAdvanced_MovingActor();

	// === 도전 과제 2: 랜덤 속성 설정을 위한 Public Setter 함수들 ===
    UFUNCTION(BlueprintCallable, Category = "Advanced|Properties")
    void SetRotationSpeed(float NewRotationSpeed) 
    { 
        RotationSpeed = NewRotationSpeed; 
        UE_LOG(LogTemp, Warning, TEXT("Advanced_MovingActor: RotationSpeed set to %f"), NewRotationSpeed);
    }

    UFUNCTION(BlueprintCallable, Category = "Advanced|Properties")
    void SetMovementSpeed(const FVector& NewMovementSpeed) 
    { 
        MovementSpeed = NewMovementSpeed; 
        UE_LOG(LogTemp, Warning, TEXT("Advanced_MovingActor: MovementSpeed set to %s"), *NewMovementSpeed.ToString());
    }

    UFUNCTION(BlueprintCallable, Category = "Advanced|Properties")
    void SetMaxRange(float NewMaxRange) 
    { 
        MaxRange = NewMaxRange; 
        UE_LOG(LogTemp, Warning, TEXT("Advanced_MovingActor: MaxRange set to %f"), NewMaxRange);
    }

    UFUNCTION(BlueprintCallable, Category = "Advanced|Properties")
    void SetDisappearSettings(float NewDelay, float NewDuration, float NewInterval)
    {
        DisappearDelay = NewDelay;
        DisappearDuration = NewDuration;
        MovementInterval = NewInterval;
        UE_LOG(LogTemp, Warning, TEXT("Advanced_MovingActor: Disappear settings updated"));
    }

    UFUNCTION(BlueprintCallable, Category = "Advanced|Properties")
    void SetMovementPoints(const TArray<FVector>& NewMovementPoints)
    {
        MovementPoints = NewMovementPoints;
        CurrentPointIndex = 0;
        UE_LOG(LogTemp, Warning, TEXT("Advanced_MovingActor: MovementPoints updated with %d points"), MovementPoints.Num());
    }

    // 완전 자동 랜덤 설정 함수 (도전 과제 2 핵심)
    UFUNCTION(BlueprintCallable, Category = "Advanced|Random")
    void ApplyRandomConfiguration(int32 ComplexityLevel = 1)
    {
        float ComplexityMultiplier = 1.0f + (ComplexityLevel * 0.3f);
        
        // FMath::RandRange를 활용한 모든 속성 랜덤화
        RotationSpeed = FMath::RandRange(30.0f, 180.0f) * ComplexityMultiplier;
        MovementSpeed = FVector(
            FMath::RandRange(100.0f, 500.0f) * ComplexityMultiplier,
            FMath::RandRange(-200.0f, 200.0f),
            FMath::RandRange(-100.0f, 100.0f)
        );
        MaxRange = FMath::RandRange(100.0f, 400.0f) * ComplexityMultiplier;
        MovementInterval = FMath::RandRange(0.5f, 3.0f) / ComplexityMultiplier;
        DisappearDelay = FMath::RandRange(0.5f, 3.0f) / ComplexityMultiplier;
        DisappearDuration = FMath::RandRange(1.0f, 4.0f);
        
        // MovementPoints 랜덤 생성
        MovementPoints.Empty();
        int32 NumPoints = FMath::RandRange(3, 8);
        FVector CurrentLocation = GetActorLocation();
        
        for (int32 i = 0; i < NumPoints; i++)
        {
            FVector RandomPoint = FVector(
                CurrentLocation.X + FMath::RandRange(-MaxRange, MaxRange),
                CurrentLocation.Y + FMath::RandRange(-MaxRange, MaxRange),
                CurrentLocation.Z + FMath::RandRange(-MaxRange * 0.3f, MaxRange * 0.3f)
            );
            MovementPoints.Add(RandomPoint);
        }
        
        CurrentPointIndex = 0;
        UE_LOG(LogTemp, Warning, TEXT("Advanced_MovingActor: Full random configuration applied (Complexity: %d, Points: %d)"), 
            ComplexityLevel, NumPoints);
    }

	// Getter 함수들
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Advanced|Properties")
	float GetRotationSpeed() const { return RotationSpeed; }

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Advanced|Properties")
	FVector GetMovementSpeed() const { return MovementSpeed; }

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Advanced|Properties")
	float GetMaxRange() const { return MaxRange; }

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Advanced|Properties")
	int32 GetMovementPointCount() const { return MovementPoints.Num(); }

	// 도전 과제 1: 타이머 시스템 제어 함수들
	UFUNCTION(BlueprintCallable, Category = "Advanced|Timer Control")
	void StartAdvancedTimerSystem()
    {
    	// 고급 타이머 시스템 시작
    	if (MovementPoints.Num() > 0)
    	{
    		GetWorld()->GetTimerManager().SetTimer(
				MovingTimerHandle,
				this,
				&AAdvanced_MovingActor::MoveToNextPoint,
				MovementInterval,
				true
			);
    	}
        
    	if (bEnableDisappearing)
    	{
    		GetWorld()->GetTimerManager().SetTimer(
				DisappearTimerHandle,
				this,
				&AAdvanced_MovingActor::DisappearPlatform,
				DisappearDelay,
				false
			);
    	}
        
    	UE_LOG(LogTemp, Warning, TEXT("Advanced timer system started"));
    }

	UFUNCTION(BlueprintCallable, Category = "Advanced|Timer Control")
	void StopAdvancedTimerSystem()
    {
    	GetWorld()->GetTimerManager().ClearTimer(MovingTimerHandle);
    	GetWorld()->GetTimerManager().ClearTimer(DisappearTimerHandle);
    	GetWorld()->GetTimerManager().ClearTimer(ReappearTimerHandle);
        
    	UE_LOG(LogTemp, Warning, TEXT("Advanced timer system stopped"));
    }

	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = " MovingPlatform|Components")
	USceneComponent* SceneRoot;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = " MovingPlatform|Components")
	UStaticMeshComponent* StaticMeshComp;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = " MovingPlatform|Properties")
	float RotationSpeed;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = " MovingPlatform|Properties")
	FVector MovementSpeed;

	UPROPERTY(editAnywhere, BlueprintReadWrite, Category = " MovingPlatform|Properties")
	float MaxRange = 200;
	
	// 타이버 핸들
	UPROPERTY()
	FTimerHandle DisappearTimerHandle;
	UPROPERTY()
	FTimerHandle MovingTimerHandle;
	UPROPERTY()
	FTimerHandle ReappearTimerHandle;

	// 설정 가능한 속성들
	UPROPERTY(editAnywhere, BlueprintReadWrite, Category="MovingPlatform|Properties")
	float DisappearDelay = 1.0f;
	UPROPERTY(editAnywhere, BlueprintReadWrite, Category="MovingPlatform|Properties")
	float DisappearDuration = 2.0f;
	UPROPERTY(editAnywhere, BlueprintReadWrite, Category="MovingPlatform|Properties")
	float MovementInterval = 2.0f;
	UPROPERTY(editAnywhere, BlueprintReadWrite, Category="MovingPlatform|Properties")
	TArray<FVector> MovementPoints;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MovingPlatform|Properties")
	bool bEnableDisappearing = false;
	
	// 현재 상태
	FVector StartLocation;
	int32 CurrentPointIndex = 0;
	bool bIsDisappeared = false;

	// 타이머 함수들
	UFUNCTION()
	void DisappearPlatform();

	UFUNCTION()
	void ReappearPlatform();

	UFUNCTION()
	void MoveToNextPoint();

	UFUNCTION()
	void OnMovementComplete();  // 이동 완료 후 호출되는 함수

	UFUNCTION()
	void StartNextCycle();
	
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	
};
