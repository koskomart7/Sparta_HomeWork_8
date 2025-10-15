// Fill out your copyright notice in the Description page of Project Settings.


#include "RotatingPlatform.h"

// Sets default values
ARotatingPlatform::ARotatingPlatform()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	SceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("SceneRoot"));
	RootComponent = SceneRoot;

	StaticMeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMeshComp"));
	StaticMeshComp->SetupAttachment(SceneRoot);

	static ConstructorHelpers::FObjectFinder<UStaticMesh> MeshAsset(TEXT("/Game/Resources/Props/SM_Coin_A.SM_Coin_A"));
	if (MeshAsset.Succeeded())
	{
		StaticMeshComp->SetStaticMesh(MeshAsset.Object);
	}

	static ConstructorHelpers::FObjectFinder<UMaterial> MaterialAsset(TEXT("/Game/Resources/Materials/M_Metal_Gold.M_Metal_Gold"));
	if (MaterialAsset.Succeeded())
	{
		StaticMeshComp->SetMaterial(0, MaterialAsset.Object);
	}

	// 기본값 초기화
	RotationSpeed = 90.0f;
	RotationInterval = 0.02f;
	RotationAxis = FVector(0.0f, 0.0f, 1.0f);

	// 미리 정의된 회전축들 설정
	PresetRotationAxes.Add(FVector(1.0f, 0.0f, 0.0f)); // X축
	PresetRotationAxes.Add(FVector(0.0f, 1.0f, 0.0f)); // Y축
	PresetRotationAxes.Add(FVector(0.0f, 0.0f, 1.0f)); // Z축
	PresetRotationAxes.Add(FVector(1.0f, 1.0f, 0.0f).GetSafeNormal()); // XY 대각선
	PresetRotationAxes.Add(FVector(1.0f, 0.0f, 1.0f).GetSafeNormal()); // XZ 대각선
	
}

// Called when the game starts or when spawned
void ARotatingPlatform::BeginPlay()
{
	Super::BeginPlay();
	// 스케일 설정 
	SetActorScale3D(FVector(3.0f));
	
	//주기적 회전 타이머 설정
	GetWorld()->GetTimerManager().SetTimer(
	RotationTimerHandle,
	this,
	&ARotatingPlatform::RotatePlatform,
	RotationInterval,
	true
	);

	// 고급 타이머 시스템 설정
	SetupAdvancedTimers();
}

void ARotatingPlatform::RotatePlatform()
{
	if (bIsDisappeared) return;
	// 현재 회전에서 추가 회전 적용
	FRotator CurrentRotation = GetActorRotation();
	FRotator RotationDelta = FRotator::ZeroRotator;

	// 회전 축에 따른 회전 적용
	float DeltaRotation = RotationSpeed * RotationInterval;

	if (RotationAxis.X != 0) RotationDelta.Pitch = DeltaRotation * RotationAxis.X;
	if (RotationAxis.Y != 0) RotationDelta.Roll = DeltaRotation * RotationAxis.Y;
	if (RotationAxis.Z != 0) RotationDelta.Yaw = DeltaRotation * RotationAxis.Z;

	SetActorRotation(CurrentRotation + RotationDelta);
}

void ARotatingPlatform::ChangeRotationSpeed()
{
	float NewSpeed = GetRandomRotationSpeed();
	RotationSpeed = NewSpeed;
	
	UE_LOG(LogTemp, Warning, TEXT("Rotation speed changed to: %f"), NewSpeed);
}

void ARotatingPlatform::DisappearPlatform()
{
	if (!bIsDisappeared)
	{
		// 플랫폼 사라지기
		StaticMeshComp->SetVisibility(false);
		StaticMeshComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		bIsDisappeared = true;

		UE_LOG(LogTemp, Warning, TEXT("Rotating Platform disappeared!"));

		// 재등장 타이머 설정
		GetWorld()->GetTimerManager().SetTimer(
			ReappearTimerHandle,
			this,
			&ARotatingPlatform::ReappearPlatform,
			DisappearDuration,
			false
		);
	}
}

void ARotatingPlatform::ReappearPlatform()
{
	if (bIsDisappeared)
	{
		// 플랫폼 재등장
		StaticMeshComp->SetVisibility(true);
		StaticMeshComp->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		bIsDisappeared = false;

		UE_LOG(LogTemp, Warning, TEXT("Rotating Platform reappeared!"));

		// 다음 사라지기 타이머 설정 (반복하는 경우)
		if (bEnableDisappearing)
		{
			float NextDisappearTime = DisappearDelay + FMath::RandRange(-2.0f, 3.0f);
			GetWorld()->GetTimerManager().SetTimer(
				DisappearTimerHandle,
				this,
				&ARotatingPlatform::DisappearPlatform,
				NextDisappearTime,
				false
			);
		}
	}
}

void ARotatingPlatform::ChangeRotationAxis()
{
	FVector NewAxis;
	
	// 미리 정의된 축이 있다면 사용
	if (PresetRotationAxes.Num() > 0)
	{
		CurrentAxisIndex = (CurrentAxisIndex + 1) % PresetRotationAxes.Num();
		NewAxis = PresetRotationAxes[CurrentAxisIndex];
	}
	else
	{
		// 랜덤 축 생성
		NewAxis = GetRandomRotationAxis();
	}

	RotationAxis = NewAxis;
	UE_LOG(LogTemp, Warning, TEXT("Rotation axis changed to: %s"), *NewAxis.ToString());
}

void ARotatingPlatform::SetupAdvancedTimers()
{
	// 주기적 속도 변경 활성화
	if (bEnablePeriodicSpeedChange)
	{
		StartPeriodicSpeedChange();
	}

	// 사라지기 기능 활성화
	if (bEnableDisappearing)
	{
		GetWorld()->GetTimerManager().SetTimer(
			DisappearTimerHandle,
			this,
			&ARotatingPlatform::DisappearPlatform,
			DisappearDelay,
			false
		);
	}

	// 회전축 변경 기능 활성화
	if (bEnableAxisChange)
	{
		StartAxisChanges();
	}
}

FVector ARotatingPlatform::GetRandomRotationAxis()
{
	// 랜덤한 회전축 생성 (정규화됨)
	FVector RandomAxis = FVector(
		FMath::RandRange(-1.0f, 1.0f),
		FMath::RandRange(-1.0f, 1.0f),
		FMath::RandRange(-1.0f, 1.0f)
	);
	
	return RandomAxis.GetSafeNormal();
}

float ARotatingPlatform::GetRandomRotationSpeed()
{
	return FMath::RandRange(MinRotationSpeed, MaxRotationSpeed);
}

// Called every frame
void ARotatingPlatform::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ARotatingPlatform::StartPeriodicSpeedChange()
{
	GetWorld()->GetTimerManager().SetTimer(
		SpeedChangeTimerHandle,
		this,
		&ARotatingPlatform::ChangeRotationSpeed,
		SpeedChangeInterval,
		true ); // 반복
	UE_LOG(LogTemp, Warning, TEXT("Periodic speed change started"));
}

void ARotatingPlatform::StopPeriodicSpeedChange()
{
	GetWorld()->GetTimerManager().ClearTimer(SpeedChangeTimerHandle);
	UE_LOG(LogTemp, Warning, TEXT("Periodic speed change stopped"));
}

void ARotatingPlatform::TriggerDisappear()
{
	DisappearPlatform();
}

void ARotatingPlatform::StartAxisChanges()
{
	GetWorld()->GetTimerManager().SetTimer(
		AxisChangeTimerHandle,
		this,
		&ARotatingPlatform::ChangeRotationAxis,
		AxisChangeInterval,
		true  // 반복
	);
	UE_LOG(LogTemp, Warning, TEXT("Axis changes started"));
}

void ARotatingPlatform::StopAxisChanges()
{
	GetWorld()->GetTimerManager().ClearTimer(AxisChangeTimerHandle);
}

void ARotatingPlatform::SetRandomProperties(float NewRotationSpeed, FVector NewRotationAxis)
{
	// 회전 속도 설정 (음수면 랜덤 생성)
	if (NewRotationSpeed < 0.0f)
	{
		RotationSpeed = GetRandomRotationSpeed();
	}
	else
	{
		RotationSpeed = NewRotationSpeed;
	}

	// 회전축 설정 (제로벡터면 랜덤 생성)
	if (NewRotationAxis.IsNearlyZero())
	{
		RotationAxis = GetRandomRotationAxis();
	}
	else
	{
		RotationAxis = NewRotationAxis.GetSafeNormal();
	}

	UE_LOG(LogTemp, Warning, TEXT("Random properties set - Speed: %f, Axis: %s"), RotationSpeed, *RotationAxis.ToString());
}

