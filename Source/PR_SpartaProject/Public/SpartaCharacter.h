// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MovieSceneSequencePlayer.h"
#include "Components/WidgetComponent.h"
#include "GameFramework/Character.h"
#include "SpartaCharacter.generated.h"

// 전방선언
class USpringArmComponent;
class UCameraComponent;
struct FInputActionValue;

// 디버프 타입 열거형
UENUM(blueprintType)
enum class EDebuffType : uint8
{
	None,
	Slowing,
	ReverseControl,
	Blind
};

// 디버프 정보 구조체
USTRUCT(BlueprintType)
struct FDebuffInfo
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly)
	EDebuffType DebuffType;
	UPROPERTY(BlueprintReadOnly)
	float Duration;	// 총 지속시간
	UPROPERTY(blueprintReadOnly)
	float StartTime;	// 시작 시간 (GetWorld()->GetTimeSeconds())
	UPROPERTY(BlueprintReadOnly)
	FTimerHandle TimerHandle;

	FDebuffInfo()
	{
		DebuffType = EDebuffType::None;
		Duration = 0.0f;
		StartTime = 0.0f;
	}

	FDebuffInfo(EDebuffType Type, float InDuration, float InStartTime)
	{
		DebuffType = Type;
		Duration = InDuration;
		StartTime = InStartTime;
	}

	// 남은 시간 계산 (UI용)
	float GetRemainingTime(float CurrentTime) const
	{
		return FMath::Max(0.0f, Duration - (CurrentTime - StartTime));
	}
};

UCLASS()
class PR_SPARTAPROJECT_API ASpartaCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ASpartaCharacter();

	// Called every frame
	//virtual void Tick(float DeltaTime) override;

	
	UPROPERTY(visibleAnywhere, BlueprintReadOnly, Category = "Camera")
	USpringArmComponent* SpringArmComp;
	UPROPERTY(visibleAnywhere, BlueprintReadOnly, Category = "Camera")
	UCameraComponent* CameraComp;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "UI")
	UWidgetComponent* OverheadWidget;

	UFUNCTION(BlueprintPure, Category = "Health")
	float GetHealth() const;
	UFUNCTION(BlueprintCallable, Category = "Health")
	void AddHealth(float Amount);

	// 디버프 시스템 함수
	UFUNCTION(BlueprintCallable, Category = "Debuff")
	void ApplySlowingDebuff(float Duration, float SlowMultiplier);
	UFUNCTION(BlueprintCallable, Category = "Debuff")
	void ApplyReverseControlDebuff(float Duration);
	UFUNCTION(BlueprintCallable, Category = "Debuff")
	void ApplyBlindedDebuff(float Duration);
	UFUNCTION(BlueprintPure, Category = "Debuff")
	bool IsDebuffActive(EDebuffType DebuffType) const;
	UFUNCTION(BlueprintPure, Category = "Debuff")
	TArray<FDebuffInfo> GetActiveDebuffs() const;
	UFUNCTION(BlueprintPure, Category = "Debuff")
	int32 GetSlowingStackCount() const { return SlowingStackCount; }
	//UI 업데이트용 함수  (외부에서 호출 가능)
	UFUNCTION(BlueprintCallable, Category = "Debuff")
	FString GetDebuffStatusText() const;
	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Health")
	float MaxHealth;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Health")
	float Health;
	
	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	virtual float TakeDamage(
		float DamageAmount,
		struct FDamageEvent const& DamageEvent,
		class AController* EventInstigator,	// 데미지를 발생시킨 액터의 주인
		AActor* DamageCauser) override;	// 데미지를 일으킨 액터 
	
	UFUNCTION()
	void Move(const FInputActionValue& value);	// 구조체는 크기가 커서 복사대신 참조를 통해서 가져온다
	UFUNCTION()
	void StartJump(const FInputActionValue& value);
	UFUNCTION()
	void StopJump(const FInputActionValue& value);
	UFUNCTION()
	void Look(const FInputActionValue& value);
	UFUNCTION()
	void StartSprint(const FInputActionValue& value);
	UFUNCTION()
	void StopSprint(const FInputActionValue& value);

	void OnDeath();
	void UpdateOverheadHP();

	// 디버프 제거 함수들 
	void RemoveSlowingDebuff(int32 StackIndex);
	void RemoveReverseControlDebuff();
	void RemoveBlindedDebuff();

	// UI 업데이트 함수
	void UpdateDebuffUI();
	void StartDebuffUIUpdateTimer();
	void StopDebuffUIUpdateTimer();
	
private:
	float NormalSpeed;
	float SprintSpeedMultipier;
	float SprintSpeed;

	// 디버프 상태 변수들
	TArray<FDebuffInfo> ActiveDebuffs;

	// Slowing 디버프 ( 중첩 가능 )
	int32 SlowingStackCount;
	TArray<FTimerHandle> SlowingTimerHandles;
	float CurrentSlowMultiplier;

	// ReverseControl 디버프
	bool bIsReverseControlActive;
	FTimerHandle ReverseControlTimerHandle;

	// Blind 디버프
	bool bIsBlindActive;
	FTimerHandle BlindTimerHandle;
	float OriginalMinPitch;
	float OriginalMaxPitch;

	//UI 업데이트 타이머 ( 0.1 초마다 실행)
	FTimerHandle UIUpdateTimerHandle;
	float UIUpdateInterval; // UI 갱신 주기 
};


