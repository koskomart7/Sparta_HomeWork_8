// Fill out your copyright notice in the Description page of Project Settings.


#include "MovingPlatform.h"

#include "NaniteSceneProxy.h"

// Sets default values
AMovingPlatform::AMovingPlatform()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("SceneRoot"));
	SetRootComponent(SceneRoot);

	StaticMeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMesh"));
	StaticMeshComp->SetupAttachment(SceneRoot);

	static ConstructorHelpers::FObjectFinder<UStaticMesh> MeshAsset(TEXT("/Game/Resources/Shapes/Shape_Plane.Shape_Plane"));
	if (MeshAsset.Succeeded())
	{
		StaticMeshComp->SetStaticMesh(MeshAsset.Object);
	}

	static ConstructorHelpers::FObjectFinder<UMaterial> MaterialAsset(TEXT("/Game/Resources/Materials/M_Metal_Gold.M_Metal_Gold"));
	if (MaterialAsset.Succeeded())
	{
		StaticMeshComp->SetMaterial(0, MaterialAsset.Object);
	}
}

// Called when the game starts or when spawned
void AMovingPlatform::BeginPlay()
{
	Super::BeginPlay();

	//SetActorLocation(FVector(0.0f, 0.0f, 100.0f));
	//SetActorRotation(FRotator(0.0f, 0.0f, 0.0f));
	//SetActorScale3D(FVector(2.0f));

	StartLocation = GetActorLocation();

	FString Name = GetName();
	UE_LOG(LogTemp, Warning, TEXT("BeginPlay: %s"),*Name);
	// 타이머 시스템 설정
	SetupTimers();
}

void AMovingPlatform::TimerBasedMove()
{
	if (bIsDisappeared) return;

	// 새로운 타겟 위치 계산
	TargetLocation = GetRandomLocationInRange(MaxRange);
	bIsMovingToTarget = true;

	UE_LOG(LogTemp, Warning, TEXT("Timer-based move to: %s"), *TargetLocation.ToString());
}

void AMovingPlatform::DisappearPlatform()
{
	if (!bIsDisappeared)
	{
		// 플랫폼 사라지기
		StaticMeshComp->SetVisibility(false);
		StaticMeshComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		bIsDisappeared = true;

		UE_LOG(LogTemp, Warning, TEXT("Platform disappeared!"));

		// 재등장 타이머 설정
		GetWorld()->GetTimerManager().SetTimer(
			ReappearTimerHandle,
			this,
			&AMovingPlatform::ReappearPlatform,
			DisappearDuration,
			false
		);
	}
}

void AMovingPlatform::ReappearPlatform()
{
	if (bIsDisappeared)
	{
		// 플랫폼 재등장
		StaticMeshComp->SetVisibility(true);
		StaticMeshComp->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		bIsDisappeared = false;

		UE_LOG(LogTemp, Warning, TEXT("Platform reappeared!"));

		// 다음 사라지기 타이머 설정 (만약 반복해야 한다면)
		if (bEnableDisappearing)
		{
			float NextDisappearTime = DisappearDelay + FMath::RandRange(-1.0f, 2.0f); // 약간의 랜덤 요소 추가
			GetWorld()->GetTimerManager().SetTimer(
				DisappearTimerHandle,
				this,
				&AMovingPlatform::DisappearPlatform,
				NextDisappearTime,
				false
			);
		}
	}
}

void AMovingPlatform::ChangeToRandomLocation()
{
	if (bIsDisappeared) return;

	FVector NewLocation;
	
	// 미리 정의된 위치가 있다면 사용
	if (PresetLocations.Num() > 0)
	{
		CurrentLocationIndex = (CurrentLocationIndex + 1) % PresetLocations.Num();
		NewLocation = PresetLocations[CurrentLocationIndex];
	}
	else
	{
		// 랜덤 위치 생성
		NewLocation = GetRandomLocationInRange(800.0f);
	}

	SetActorLocation(NewLocation);
	StartLocation = NewLocation; // 새로운 시작 위치로 업데이트

	UE_LOG(LogTemp, Warning, TEXT("Platform moved to new location: %s"), *NewLocation.ToString());
}

void AMovingPlatform::StartPeriodicLocationChange()
{
	GetWorld()->GetTimerManager().SetTimer(
		LocationChangeTimerHandle,
		this,
		&AMovingPlatform::ChangeToRandomLocation,
		LocationChangeInterval,
		true ); // 반복
}

void AMovingPlatform::SetupTimers()
{
	// 타이머 기반 이동 시작
	if (bUseTimerBasedMovement)
	{
		StartTimerBasedMovement();
	}

	// 사라지기 기능 활성화
	if (bEnableDisappearing)
	{
		GetWorld()->GetTimerManager().SetTimer(
			DisappearTimerHandle,
			this,
			&AMovingPlatform::DisappearPlatform,
			DisappearDelay,
			false
		);
	}

	// 주기적 위치 변경 시작
	if (bPeriodicLocationChange)
	{
		StartPeriodicLocationChange();
	}
}

FVector AMovingPlatform::GetRandomLocationInRange(float Range)
{
	FVector RandomOffset = FVector(
		FMath::RandRange(-Range, Range),
		FMath::RandRange(-Range, Range),
		FMath::RandRange(-Range/2, Range/2)
	);
	
	return StartLocation + RandomOffset;
}

void AMovingPlatform::MoveToTarget(float DeltaTime)
{
	if (!bIsMovingToTarget) return;

	FVector CurrentLocation = GetActorLocation();
	FVector Direction = (TargetLocation - CurrentLocation).GetSafeNormal();
	float Distance = FVector::Dist(CurrentLocation, TargetLocation);

	if (Distance <= 10.0f) // 목표에 도달
	{
		SetActorLocation(TargetLocation);
		bIsMovingToTarget = false;
		StartLocation = TargetLocation; // 새로운 시작점으로 업데이트
		UE_LOG(LogTemp, Warning, TEXT("Reached target location"));
		return;
	}

	// 타겟을 향해 이동
	FVector NewLocation = CurrentLocation + Direction * MoveSpeed.Size() * DeltaTime;
	SetActorLocation(NewLocation);
}

// Called every frame
void AMovingPlatform::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bIsDisappeared) return;	// 사라진 상태에서는 이동하지 않음

	// 회전 처리 
	if (!FMath::IsNearlyZero(RotationSpeed))
	{
		AddActorLocalRotation(FRotator(0.0f, RotationSpeed*DeltaTime, 0.0f));
	}

	// 타이머 기반 이동이 활성화된 경우
	if (bUseTimerBasedMovement && bIsMovingToTarget)
	{
		MoveToTarget(DeltaTime);
		return;
	}

	// 기존의 연속 이동 로직 (타이머 기반 이동이 비활성화된 경우에만 )
	if (!bUseTimerBasedMovement)
	{
		FVector CurrentLocation = GetActorLocation();
		CurrentLocation += MoveSpeed * DeltaTime;
		SetActorLocation(CurrentLocation);

		float DistanceMoved = FVector::Dist(StartLocation, CurrentLocation);

		if (DistanceMoved > MaxRange)
		{
			float OverShoot = DistanceMoved - MaxRange;
			FString Name = GetName();
			//UE_LOG(LogTemp, Warning, TEXT("%s Actor OverShoot: %f "), *Name, OverShoot);
		
			FVector MoveDirection = MoveSpeed.GetSafeNormal();
			StartLocation = StartLocation + MoveDirection * MaxRange;
			SetActorLocation(StartLocation);
			MoveSpeed = -MoveSpeed;			// 방향 반전 
		}
	}
}
// 공개 제어 함수들
void AMovingPlatform::StartTimerBasedMovement()
{
	if (bUseTimerBasedMovement && !GetWorld()->GetTimerManager().IsTimerActive(MovementTimerHandle))
	{
		GetWorld()->GetTimerManager().SetTimer(
		MovementTimerHandle,
		this,
		&AMovingPlatform::TimerBasedMove,
		MovementInterval,
		true );  // 반복
	};
	
	UE_LOG(LogTemp, Warning, TEXT("Timer-based movement started for %s"), *GetName());
}

void AMovingPlatform::StopTimerBasedMovement()
{
	GetWorld()->GetTimerManager().ClearTimer(MovementTimerHandle);
	bIsMovingToTarget = false;
	UE_LOG(LogTemp, Warning, TEXT("Timer-based movement stopped for %s"), *GetName());
}

void AMovingPlatform::TriggerDisappear()
{
	DisappearPlatform();
}

void AMovingPlatform::StartLocationChanges()
{
	StartPeriodicLocationChange();
}

void AMovingPlatform::StopLocationChanges()
{
	GetWorld()->GetTimerManager().ClearTimer(LocationChangeTimerHandle);
}

