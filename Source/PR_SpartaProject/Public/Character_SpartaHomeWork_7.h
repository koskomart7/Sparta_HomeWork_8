// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "Components/CapsuleComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Character_SpartaHomeWork_7.generated.h"

class UInputAction;
class UInputMappingContext;
struct FInputActionValue;

UCLASS()
class PR_SPARTAPROJECT_API ACharacter_SpartaHomeWork_7 : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	ACharacter_SpartaHomeWork_7();
	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;


	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	class UCapsuleComponent* CapsuleComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	class USkeletalMeshComponent* SkeletalMeshComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* SpringArmComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* CameraComponent;
	
	

	
	UFUNCTION()
	void Move(const FInputActionValue& value);	// 구조체는 크기가 커서 복사대신 참조를 통해서 가져온다
	UFUNCTION()
	void Look(const FInputActionValue& value);
	UFUNCTION()
	void Elevate(const FInputActionValue& value);
	UFUNCTION()
	void Roll(const FInputActionValue& value);


private:

	// Movement Params
	UPROPERTY(EditAnywhere, Category = "Movement")
	float MoveSpeed = 600.0f;      // 유닛/초

	UPROPERTY(EditAnywhere, Category = "Movement")
	float LookYawSpeed = 0.1f;     // 마우스 X → Yaw(도/틱)

	UPROPERTY(EditAnywhere, Category = "Movement")
	float LookPitchSpeed = 0.1f;   // 마우스 Y → Pitch(도/틱)

	UPROPERTY(EditAnywhere, Category = "Movement")
	float MinPitch = -80.0f;

	UPROPERTY(EditAnywhere, Category = "Movement")
	float CurrentPitch = 0.0f;

	UPROPERTY(EditAnywhere, Category = "Movement")
	float MaxRollDeg = 30.0f;      

	UPROPERTY(EditAnywhere, Category = "Movement")
	float RollReturnSpeed = 8.0f; 
	float TargetRollDeg = 0.0f;
	float CurrentRollDeg = 0.0f;
	FRotator MeshBaseRelativeRot;

	UPROPERTY(EditAnywhere, Category="Gravity")
	float GravityAccel = -980.f;          // [+] 중력 가속도 (cm/s^2)

	UPROPERTY(EditAnywhere, Category="Gravity")
	float MaxFallSpeed = 4000.f;          // [+] 최대 낙하 속도 (cm/s)

	UPROPERTY(EditAnywhere, Category="Gravity")
	float GroundCheckDistance = 5.f;      // [+] 지면 감지 여유 거리

	UPROPERTY(EditAnywhere, Category="Gravity")
	float AirControlRatio = 0.4f;         // [+] 공중 이동 배율 (0.3~0.5 권장)

	float VerticalVelocity = 0.f;         // [+] 현재 Z축 속도 (cm/s)
	bool  bIsGrounded = false;            // [+] 지상 여부
	float LastElevateAxis = 0.f;
	
	// [+] 내부 함수 선언
	void ApplyArtificialGravity(float DeltaTime);
	bool GroundCheck(FHitResult& OutHit) const;
};
