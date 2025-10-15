// Fill out your copyright notice in the Description page of Project Settings.


#include "Character_SpartaHomeWork_7.h"
#include "SpartaPlayerController.h"
#include "EnhancedInputComponent.h"
#include "InputMappingContext.h"
#include "BaseGizmos/GizmoElementShared.h"


class ASpartaPlayerController;
// Sets default values
ACharacter_SpartaHomeWork_7::ACharacter_SpartaHomeWork_7()
{
	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	CapsuleComponent = CreateDefaultSubobject<UCapsuleComponent>(TEXT("CapsuleComponent"));
	RootComponent = CapsuleComponent;
	CapsuleComponent->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	CapsuleComponent->SetCapsuleHalfHeight(90.0f);
	CapsuleComponent->SetCapsuleRadius(40.0f);
	CapsuleComponent->SetCollisionObjectType(ECC_Pawn);
	CapsuleComponent->SetCollisionResponseToAllChannels(ECR_Block);
	CapsuleComponent->SetCollisionProfileName("Pawn");
	CapsuleComponent->SetSimulatePhysics(false);

	SkeletalMeshComponent = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("SkeletalMeshComponent"));
	SkeletalMeshComponent->SetupAttachment(RootComponent);
	SkeletalMeshComponent->SetRelativeLocation(FVector(0.0f, 0.0f, -90.0f));
	SkeletalMeshComponent->SetRelativeRotation(FRotator(0.f, -90.f, 0.f));
	SkeletalMeshComponent->SetSimulatePhysics(false);
	SkeletalMeshComponent->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	SkeletalMeshComponent->SetCollisionResponseToAllChannels(ECR_Ignore);
	SkeletalMeshComponent->SetCollisionResponseToChannel(ECC_WorldStatic, ECR_Block);
	SkeletalMeshComponent->SetCollisionResponseToChannel(ECC_WorldDynamic, ECR_Block);
		
	SpringArmComponent = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArmComponent"));
	SpringArmComponent->SetupAttachment(RootComponent);
	SpringArmComponent->TargetArmLength = 400.0f;
	SpringArmComponent->bUsePawnControlRotation = true;
	SpringArmComponent->bEnableCameraLag = true;
	SpringArmComponent->CameraLagSpeed = 10.f;
	SpringArmComponent->SetRelativeLocation(FVector(0.f, 0.f, 60.f));

	CameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraComponent"));
	CameraComponent->SetupAttachment(SpringArmComponent, USpringArmComponent::SocketName);
	CameraComponent->bUsePawnControlRotation = false;

	bUseControllerRotationYaw = true;
	bUseControllerRotationPitch = true;
	bUseControllerRotationRoll = false;
}

void ACharacter_SpartaHomeWork_7::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	CurrentRollDeg = FMath::FInterpTo(CurrentRollDeg, TargetRollDeg, DeltaTime, RollReturnSpeed);
	
	FRotator NewRel = MeshBaseRelativeRot;
	NewRel.Pitch = MeshBaseRelativeRot.Pitch + CurrentRollDeg; 
	SkeletalMeshComponent->SetRelativeRotation(NewRel);
	ApplyArtificialGravity(DeltaTime); 
}

void ACharacter_SpartaHomeWork_7::BeginPlay()
{
	Super::BeginPlay();
	if (SkeletalMeshComponent->IsSimulatingPhysics())
	{
		SkeletalMeshComponent->SetSimulatePhysics(false);
	}
	MeshBaseRelativeRot = SkeletalMeshComponent->GetRelativeRotation();
	
}

void ACharacter_SpartaHomeWork_7::Move(const FInputActionValue& value)
{
	if (!Controller) return;
	
	const FVector2D MoveInput = value.Get<FVector2D>();
	const float Dt = GetWorld() ? GetWorld()->GetDeltaSeconds() : 0.016f;
	FVector Local(MoveInput.X, MoveInput.Y, 0.f);
	if (Local.IsNearlyZero()) return;

	const float Ratio = bIsGrounded ? 1.0f : AirControlRatio;   // [*]
	AddActorLocalOffset(Local.GetSafeNormal() * (MoveSpeed * Ratio) * Dt, true); // [*]
}

void ACharacter_SpartaHomeWork_7::Look(const FInputActionValue& value)
{
	FVector2D LookAxisVector = value.Get<FVector2D>();
	
	AddControllerYawInput(LookAxisVector.X * LookYawSpeed);
	AddControllerPitchInput(LookAxisVector.Y * LookPitchSpeed);
}

void ACharacter_SpartaHomeWork_7::Elevate(const FInputActionValue& value)
{
	const float AxisZ = value.Get<float>();
	LastElevateAxis = AxisZ;
	const float Dt = GetWorld() ? GetWorld()->GetDeltaSeconds() : 0.016f;
	if (!FMath::IsNearlyZero(AxisZ))
	{
		AddActorLocalOffset(FVector(0.f, 0.f, AxisZ) * (MoveSpeed * Dt), /*bSweep=*/true);
     
		bIsGrounded = false;
	}
}

void ACharacter_SpartaHomeWork_7::Roll(const FInputActionValue& value)
{
	const float Axis = FMath::Clamp(value.Get<float>(), -1.f, 1.f); // Q=-1, E=+1, 뗄 때 0
	TargetRollDeg = Axis * MaxRollDeg;
}

void ACharacter_SpartaHomeWork_7::ApplyArtificialGravity(float DeltaTime)
{
	if (!GetWorld()) return;
	if (LastElevateAxis > 0.f)
	{
		bIsGrounded = false;     
		VerticalVelocity = 0.f;  
		return;                  
	}
	// 1) 바로 아래 지면이 있는지(스탠딩) 확인
	FHitResult GroundHit;
	const bool bOnGroundNow = GroundCheck(GroundHit);

	if (bOnGroundNow && VerticalVelocity <= 0.f)
	{
		bIsGrounded = true;
		VerticalVelocity = 0.f;

		return;
	}

	// 2) 공중: 중력 적분
	bIsGrounded = false;
	VerticalVelocity = FMath::Clamp(VerticalVelocity + GravityAccel * DeltaTime, -MaxFallSpeed, MaxFallSpeed);

	const FVector GravityDelta(0.f, 0.f, VerticalVelocity * DeltaTime);
	FHitResult FallHit;
	AddActorWorldOffset(GravityDelta, /*bSweep=*/true, &FallHit);

	if (FallHit.bBlockingHit && VerticalVelocity < 0.f && FallHit.ImpactNormal.Z > 0.5f)
	{
		bIsGrounded = true;
		VerticalVelocity = 0.f;
	}
}

bool ACharacter_SpartaHomeWork_7::GroundCheck(FHitResult& OutHit) const
{
	if (!GetWorld() || !CapsuleComponent) return false;

	const FVector Start = GetActorLocation();
	const float HalfHeight = CapsuleComponent->GetScaledCapsuleHalfHeight();
	const float TraceLen = HalfHeight + GroundCheckDistance;
	const FVector End = Start - FVector(0.f, 0.f, TraceLen);

	FCollisionQueryParams Params(SCENE_QUERY_STAT(GroundCheck), false, this);
	const bool bHit = GetWorld()->LineTraceSingleByChannel(OutHit, Start, End, ECC_Visibility, Params);

	// 디버그선
	DrawDebugLine(GetWorld(), Start, End, bHit ? FColor::Green : FColor::Red, false, 0.f, 0, 1.f);

	return bHit && OutHit.ImpactNormal.Z > 0.5f; 
}

// Called to bind functionality to input
void ACharacter_SpartaHomeWork_7::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (UEnhancedInputComponent* EnhancedInput = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		// IA를 가져오기 위해 현재 소유 중인 Controller를 ASpartaPlayerController로 캐스팅
		if (ASpartaPlayerController* PlayerController = Cast<ASpartaPlayerController>(GetController()))
		{
			if (PlayerController->MoveAction)
			{
				// IA_Move 액션 키를 "키를 누르고 있는 동안" Move() 호출
				EnhancedInput->BindAction(
					PlayerController->MoveAction,
					ETriggerEvent::Triggered,
					this,
					&ACharacter_SpartaHomeWork_7::Move
				);
			}
			if (PlayerController->LookAction)
			{
				EnhancedInput->BindAction(
					PlayerController->LookAction,
					ETriggerEvent::Triggered,
					this,
					&ACharacter_SpartaHomeWork_7::Look
					);
			}
			if (PlayerController->ElevateAction)
			{
				EnhancedInput->BindAction(PlayerController->ElevateAction
					, ETriggerEvent::Triggered
					, this
					, &ACharacter_SpartaHomeWork_7::Elevate);
			}

			if (PlayerController->RollAction)
			{
				EnhancedInput->BindAction(PlayerController->RollAction
					, ETriggerEvent::Triggered
					, this
					, &ACharacter_SpartaHomeWork_7::Roll);
				EnhancedInput->BindAction(PlayerController->RollAction, ETriggerEvent::Completed, this, &ACharacter_SpartaHomeWork_7::Roll);
			}
		}
	}
	
}



