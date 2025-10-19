// Fill out your copyright notice in the Description page of Project Settings.


#include "SpartaCharacter.h"
#include "SpartaPlayerController.h"
#include "EnhancedInputComponent.h"
#include "SpartaGameState.h"
#include "Camera/CameraComponent.h"
#include "Components/TextBlock.h"
#include "Components/WidgetComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"


// Sets default values
ASpartaCharacter::ASpartaCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	SpringArmComp = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArmComp->SetupAttachment(RootComponent);
	SpringArmComp->TargetArmLength = 300.0f;
	SpringArmComp->bUsePawnControlRotation = true;
	
	CameraComp = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	CameraComp->SetupAttachment(SpringArmComp, USpringArmComponent::SocketName);
	CameraComp->bUsePawnControlRotation = false;

	OverheadWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("OverheadWidget"));
	OverheadWidget->SetupAttachment(GetMesh());
	OverheadWidget->SetWidgetSpace(EWidgetSpace::Screen);
	
	NormalSpeed = 600.0f;
	SprintSpeedMultipier = 1.7f;
	SprintSpeed = NormalSpeed * SprintSpeedMultipier;

	GetCharacterMovement()->MaxWalkSpeed = NormalSpeed;

	MaxHealth = 100.0f;
	Health = MaxHealth;

	// 디버프 초기화
	SlowingStackCount = 0;
	CurrentSlowMultiplier = 1.0f;
	bIsReverseControlActive = false;
	bIsBlindActive = false;
	OriginalMinPitch = -80.0f;
	OriginalMaxPitch = 80.0f;

	// UI 업데이트 주기 설정 ( 0.1초 )
	UIUpdateInterval = 0.1f;
}

float ASpartaCharacter::GetHealth() const
{
	return Health;
}

void ASpartaCharacter::AddHealth(float Amount)
{
	// 체력을 회복시킴. 최대 체력을 초과하지 않도록 제한함
	Health = FMath::Clamp(Health + Amount, 0.0f, MaxHealth);
	UE_LOG(LogTemp, Warning, TEXT("헬스 증가 : %f"), Health);

	UpdateOverheadHP();
}

// ========== 디버프 시스템 구현 (최적화) ==========
//
//===============================================
void ASpartaCharacter::ApplySlowingDebuff(float Duration, float SlowMultiplier)
{
	if (!GetWorld()) return;
	SlowingStackCount++;
	CurrentSlowMultiplier *= SlowMultiplier;

	// 현재 속도에 디버프 적용
	if (GetCharacterMovement())
	{
		GetCharacterMovement()->MaxWalkSpeed = NormalSpeed * CurrentSlowMultiplier;
	}
	// 디버프 정보 추가 (시작 시간 기록)
	float StartTime = GetWorld()->GetTimeSeconds();
	FDebuffInfo NewDebuff(EDebuffType::Slowing, Duration, StartTime);
	ActiveDebuffs.Add(NewDebuff);

	// 타이머 설정 (디버프 제거용)
	FTimerHandle NewTimerHandle;
	SlowingTimerHandles.Add(NewTimerHandle);
	int32 StackIndex = SlowingStackCount - 1;

	GetWorld()->GetTimerManager().SetTimer(
		SlowingTimerHandles[StackIndex],
		[this, StackIndex]()
		{
			RemoveSlowingDebuff(StackIndex);
		},
		Duration,
		false
		);
	//UI 업데이트 타이머 시작 ( 첫 디버프일 때만 )
	StartDebuffUIUpdateTimer();

	FString DebugMsg = FString::Printf(TEXT("Slowing 디버프 적용! (스택 : %d, 속도: %.0f%%)"),
		SlowingStackCount, CurrentSlowMultiplier * 100.0f);
	GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Blue, DebugMsg);
	UE_LOG(LogTemp, Warning, TEXT("%s"), *DebugMsg
		);
}

void ASpartaCharacter::ApplyReverseControlDebuff(float Duration)
{
	if (!GetWorld()) return;
	bIsReverseControlActive = true;

	// 디버프 정보 추가
	float StartTime = GetWorld()->GetTimeSeconds();
	FDebuffInfo NewDebuff(EDebuffType::ReverseControl, Duration, StartTime);
	ActiveDebuffs.Add(NewDebuff);

	GetWorld() -> GetTimerManager().SetTimer(
		ReverseControlTimerHandle,
		this,
		&ASpartaCharacter::RemoveReverseControlDebuff,
		Duration,
		false
	);

	// UI 업데이트 타이머 시작
	StartDebuffUIUpdateTimer();

	GEngine -> AddOnScreenDebugMessage(-1, 3.0f, FColor::Red, TEXT("컨트롤 반전 디버프 적용!"));
	UE_LOG(LogTemp, Warning, TEXT("ReverseControl 디버프 적용! (지속시간 : %.1f 초"), Duration);
}

void ASpartaCharacter::ApplyBlindedDebuff(float Duration)
{
	if (!GetWorld()) return;
	bIsBlindActive = true;

	// 디버프 정보 추가
	float StartTime = GetWorld()->GetTimeSeconds();
	FDebuffInfo NewDebuff(EDebuffType::Blind, Duration, StartTime);
	ActiveDebuffs.Add(NewDebuff);

	// 카메라 Pitch 제한 설정
	if (APlayerController* PC = Cast<APlayerController>(GetController()))
	{
		PC->PlayerCameraManager->ViewPitchMin = -150.0f;
		PC->PlayerCameraManager->ViewPitchMax = 50.0f;
	}
	GetWorld()->GetTimerManager().SetTimer(
		BlindTimerHandle,
		this,
		&ASpartaCharacter::RemoveBlindedDebuff,
		Duration,
		false );
	
	// UI 업데이트 타이머 시작 
	StartDebuffUIUpdateTimer();
	GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Yellow, TEXT("시야 제한 디버프 적용!"));
	UE_LOG(LogTemp, Warning, TEXT("Blind 디버프 적용! (지속시간: %.1f초)"), Duration);
}

bool ASpartaCharacter::IsDebuffActive(EDebuffType DebuffType) const
{
	for (const FDebuffInfo& Debuff : ActiveDebuffs)
	{
		if (Debuff.DebuffType == DebuffType)
		{
			return true;
		}
	}
	return false;
}

TArray<FDebuffInfo> ASpartaCharacter::GetActiveDebuffs() const
{
	return ActiveDebuffs;
}


FString ASpartaCharacter::GetDebuffStatusText() const
{
	if (!GetWorld() || ActiveDebuffs.Num() == 0)
	{
		return TEXT("");
	}
	FString DebuffStatusText = TEXT("[ 디버프 상태 ]\n");
	float CurrentTime = GetWorld()->GetTimeSeconds();

	for (const FDebuffInfo& Debuff : ActiveDebuffs)
	{
		FString DebuffName;
		switch (Debuff.DebuffType)
		{
		case EDebuffType::Slowing:
			DebuffName = FString::Printf(TEXT("이동속도 감소 x%d"), SlowingStackCount);
			break;
		case EDebuffType::ReverseControl:
			DebuffName = TEXT("조작 반전");
			break;
		case EDebuffType::Blind:
			DebuffName = TEXT("시야 제한");
			break;
		default:
			DebuffName = TEXT("알 수 없음");
			break;
		}
		float RemainingTime = Debuff.GetRemainingTime(CurrentTime);
		DebuffStatusText += FString::Printf(TEXT("%s: %.1f초 \n"), *DebuffName, RemainingTime);
	}
	return DebuffStatusText;
}
// =============================================================================
//==============================================================================
// Called when the game starts or when spawned
void ASpartaCharacter::BeginPlay()
{
	Super::BeginPlay();

	UpdateOverheadHP();
}

// Called every frame
/*void ASpartaCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	
}*/

// Called to bind functionality to input
void ASpartaCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (UEnhancedInputComponent* EnhancedInput = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		if (ASpartaPlayerController* PlayerController = Cast<ASpartaPlayerController>(GetController()))
		{
			if (PlayerController->MoveAction)
			{
				EnhancedInput->BindAction(
					PlayerController->MoveAction,
					ETriggerEvent::Triggered,
					this,
					&ASpartaCharacter::Move
					);
			}
			if (PlayerController->JumpAction)
			{
				EnhancedInput->BindAction(
					PlayerController->JumpAction,
					ETriggerEvent::Triggered,
					this,
					&ASpartaCharacter::StartJump
					);
				EnhancedInput->BindAction(
					PlayerController->JumpAction,
					ETriggerEvent::Completed,
					this,
					&ASpartaCharacter::StopJump
					);
			}
			if (PlayerController->LookAction)
			{
				EnhancedInput->BindAction(
					PlayerController->LookAction,
					ETriggerEvent::Triggered,
					this,
					&ASpartaCharacter::Look
					);
			}
			if (PlayerController->SprintAction)
			{
				EnhancedInput->BindAction(
					PlayerController->SprintAction,
					ETriggerEvent::Triggered,
					this,
					&ASpartaCharacter::StartSprint
					);
				EnhancedInput->BindAction(
					PlayerController->SprintAction,
					ETriggerEvent::Completed,
					this,
					&ASpartaCharacter::StopSprint
					);
			}
			
		}
	}
}

float ASpartaCharacter::TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent,
	class AController* EventInstigator, AActor* DamageCauser)
{
	float ActualDamage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

	Health = FMath::Clamp(Health - DamageAmount, 0.0f, MaxHealth);
	UE_LOG(LogTemp, Warning, TEXT("헬스감소 : %f "), Health);
	UpdateOverheadHP();
	
	if (Health <= 0.0f)
	{
		OnDeath();
	}
	return ActualDamage;
}

void ASpartaCharacter::Move(const FInputActionValue& value)
{
	if (!Controller) return;

	FVector2D MoveInput = value.Get<FVector2D>();

	// ReverseControl 디버프 적용  
	if (bIsReverseControlActive)
	{
		MoveInput.X = -MoveInput.X;
		MoveInput.Y = -MoveInput.Y;
	}
	if (!FMath::IsNearlyZero(MoveInput.X))
	{
		AddMovementInput(GetActorForwardVector(), MoveInput.X);
	}
	if (!FMath::IsNearlyZero(MoveInput.Y))
	{
		AddMovementInput(GetActorRightVector(), MoveInput.Y);
	}
}

void ASpartaCharacter::StartJump(const FInputActionValue& value)
{
	if (value.Get<bool>())
	{
		Jump();	// 자체함수 내부에 Controller 체크가 있다 
	}
}

void ASpartaCharacter::StopJump(const FInputActionValue& value)
{
	if (!value.Get<bool>())
	{
		StopJumping();
	}
}

void ASpartaCharacter::Look(const FInputActionValue& value)
{
	FVector2D LookInput = value.Get<FVector2D>();

	//  Blind 디버프 적용 - 카메라 회전 제한
	if (bIsBlindActive)
	{
		LookInput *= 0.3f; // 회전 속도 70% 감소 
	}
	AddControllerYawInput(LookInput.X);
	AddControllerPitchInput(LookInput.Y);
}

void ASpartaCharacter::StartSprint(const FInputActionValue& value)
{
	if (GetCharacterMovement())
	{
		GetCharacterMovement()->MaxWalkSpeed = SprintSpeed * CurrentSlowMultiplier;
	}
}

void ASpartaCharacter::StopSprint(const FInputActionValue& value)
{
	if (GetCharacterMovement())
	{
		GetCharacterMovement()->MaxWalkSpeed = NormalSpeed * CurrentSlowMultiplier;
	}
}

void ASpartaCharacter::OnDeath()
{
	ASpartaGameState* SpartaGameState = GetWorld() ? GetWorld()->GetGameState<ASpartaGameState>() : nullptr;
	if (SpartaGameState)
	{
		SpartaGameState->OnGameOver();
	}
}

void ASpartaCharacter::UpdateOverheadHP()
{
	if (!OverheadWidget) return;

	UUserWidget* OverheadWidgetInstance = OverheadWidget->GetUserWidgetObject();
	if (!OverheadWidgetInstance) return;

	if (UTextBlock* HPText = Cast<UTextBlock>(OverheadWidgetInstance->GetWidgetFromName(TEXT("OverHeadHP"))))
	{
		HPText->SetText(FText::FromString(FString::Printf(TEXT("%.0f / %.0f"), Health, MaxHealth)));
	}
}

//==================== 디버프 제거 함수 ========================

//============================================================
void ASpartaCharacter::RemoveSlowingDebuff(int32 StackIndex)
{
	if (SlowingStackCount > 0)
	{
		SlowingStackCount--;
		CurrentSlowMultiplier /= CurrentSlowMultiplier;	

		// 속도 갱신
		if (GetCharacterMovement())
		{
			GetCharacterMovement()->MaxWalkSpeed = NormalSpeed * CurrentSlowMultiplier;
		}
		// 디버프 목록에서 제거
		for (int32 i = ActiveDebuffs.Num() - 1; i >= 0; i--)
		{
			if (ActiveDebuffs[i].DebuffType == EDebuffType::Slowing)
			{
				ActiveDebuffs.RemoveAt(i);
				break;
			}
		}
		// 모든 디버프가 제거되면 UI 타이머 정지
		if (ActiveDebuffs.Num() == 0)
		{
			StopDebuffUIUpdateTimer();
		}

		FString DebuMsg = FString::Printf(TEXT("Slowing 디버프 해제! (남은 스택 : %d)"), SlowingStackCount);
		GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Green, DebuMsg);
		UE_LOG(LogTemp, Warning, TEXT("%s"), *DebuMsg);
	}
}

void ASpartaCharacter::RemoveReverseControlDebuff()
{
	bIsReverseControlActive = false;

	// 디버프 목록에서 제거
	for (int32 i = ActiveDebuffs.Num() - 1; i >= 0; i--)
	{
		if (ActiveDebuffs[i].DebuffType == EDebuffType::ReverseControl)
		{
			ActiveDebuffs.RemoveAt(i);
			break;
		}
	}
	// 모든 디버프가 제거되면 UI 타이머 정지
	if (ActiveDebuffs.Num() == 0)
	{
		StopDebuffUIUpdateTimer();
	}
	GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Green, TEXT("컨트롤 반전 디버프 해제!"));
	UE_LOG(LogTemp, Warning, TEXT("ReverseControl 디버프 해제!"));
}

void ASpartaCharacter::RemoveBlindedDebuff()
{
	bIsBlindActive = false;

	// 카메라 Pitch 제한 복구
	if (APlayerController* PC = Cast<APlayerController>(GetController()))
	{
		PC->PlayerCameraManager->ViewPitchMin = OriginalMinPitch;
		PC->PlayerCameraManager->ViewPitchMax = OriginalMaxPitch;
	}

	// 디버프 목록에서 제거
	for (int32 i = ActiveDebuffs.Num() - 1; i >= 0; i--)
	{
		if (ActiveDebuffs[i].DebuffType == EDebuffType::Blind)
		{
			ActiveDebuffs.RemoveAt(i);
			break;
		}
	}

	// 모든 디버프가 제거되면 UI 타이머 정지
	if (ActiveDebuffs.Num() == 0)
	{
		StopDebuffUIUpdateTimer();
	}
	GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Green, TEXT("시야 제한 디버프 해제!"));
	UE_LOG(LogTemp, Warning, TEXT("Blind 디버프 해제!"));
}

// ===================== UI 업데이트 함수 ============================
//        Timer 기반
// =================================================================
void ASpartaCharacter::UpdateDebuffUI()
{
	// GameState의 UpdateHUD를 호출하여 UI 갱신
	if ( UWorld* World = GetWorld())
	{
		if (ASpartaGameState* GameState = World->GetGameState<ASpartaGameState>())
		{
			GameState->UpdateHUD();
		}
	}
}

void ASpartaCharacter::StartDebuffUIUpdateTimer()
{
	if (!GetWorld()) return;

	// 이미 타이머가 실행 중이면 무시
	if (GetWorld()->GetTimerManager().IsTimerActive(UIUpdateTimerHandle))
	{
		return;
	}

	// UI 업데이트 타이머 시작 ( 0.1초마다 반복 )
	GetWorld()->GetTimerManager().SetTimer(
		UIUpdateTimerHandle,
		this,
		&ASpartaCharacter::UpdateDebuffUI,
		UIUpdateInterval,
		true
		);
	UE_LOG(LogTemp, Log, TEXT("디버프 UI 업데이트 타이머 시작"));
}

void ASpartaCharacter::StopDebuffUIUpdateTimer()
{
	if (!GetWorld()) return;

	// 타이머 정지
	if (GetWorld()->GetTimerManager().IsTimerActive(UIUpdateTimerHandle))
	{
		GetWorld()->GetTimerManager().ClearTimer(UIUpdateTimerHandle);
		UE_LOG(LogTemp, Log, TEXT("디버프 UI 업데이트 타이머 정지"));
	}

	// 마지막 UI 업데이트 ( 빈 상태 표시 )
	UpdateDebuffUI();
}
