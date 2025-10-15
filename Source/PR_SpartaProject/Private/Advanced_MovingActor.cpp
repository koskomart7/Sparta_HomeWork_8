// Fill out your copyright notice in the Description page of Project Settings.


#include "Advanced_MovingActor.h"

#include "MaterialHLSLTree.h"
#include "DataWrappers/ChaosVDParticleDataWrapper.h"
#include "TimerManager.h"

// Sets default values
AAdvanced_MovingActor::AAdvanced_MovingActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	SceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("SceneRoot"));
	RootComponent = SceneRoot;

	StaticMeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMeshComp"));
	StaticMeshComp->SetupAttachment(SceneRoot);

	static ConstructorHelpers::FObjectFinder<UStaticMesh> MeshAsset(TEXT("/Game/Resources/Props/Wall_Door_400x300.Wall_Door_400x300"));
	if (MeshAsset.Succeeded())
	{
		StaticMeshComp->SetStaticMesh(MeshAsset.Object);
	}

	static ConstructorHelpers::FObjectFinder<UMaterial> MaterialAsset(TEXT("/Game/Resources/Materials/M_Metal_Steel.M_Metal_Steel"));
	if (MaterialAsset.Succeeded())
	{
		StaticMeshComp->SetMaterial(0, MaterialAsset.Object);
	}

	RotationSpeed = 50.0f;
	//MovementSpeed = FVector(200.0f, 0.0f, 0.0f);
	
}

// Called when the game starts or when spawned
void AAdvanced_MovingActor::BeginPlay()
{
	Super::BeginPlay();

	//StartLocation = GetActorLocation();
	StartNextCycle();
	
}

void AAdvanced_MovingActor::DisappearPlatform()
{
	if (!bIsDisappeared)
	{
		//플랫폼을 보이지 않게 하고 충돌 비활성화
		StaticMeshComp->SetVisibility(false);
		StaticMeshComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		bIsDisappeared = true;

		UE_LOG(LogTemp, Warning, TEXT("Disappearing Platform"));
		// DisappearDuration 시간 후에 다시 나타나도록 타이머 설정
		GetWorld()->GetTimerManager().SetTimer(
			DisappearTimerHandle,	// 타이머를 조작할 핸들
			// 함수를 호출할 객체 
			this,
			&AAdvanced_MovingActor::ReappearPlatform,  // 호출할 함수 포인터
			// 지연시간 
			DisappearDuration, false ); // 반복 여부 
	}
}

void AAdvanced_MovingActor::ReappearPlatform()
{
	if (bIsDisappeared)
	{
		// 플랫폼을 다시 보이게 하고 충돌 활성화
		StaticMeshComp->SetVisibility(true);
		StaticMeshComp->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		bIsDisappeared = false;

		UE_LOG(LogTemp, Warning, TEXT("액터 등장"));

		// 다음 사라지기 사이클 시작 (복잡도가 높으면)
		if (bEnableDisappearing && FMath::RandBool())
		{
			GetWorld()->GetTimerManager().SetTimer(
				DisappearTimerHandle,
				this,
				&AAdvanced_MovingActor::DisappearPlatform,
				DisappearDelay,
				false
			);
		}
	}
}

void AAdvanced_MovingActor::MoveToNextPoint()
{
	if (MovementPoints.Num() == 0 || bIsDisappeared) return;

	FVector TargetPoint = MovementPoints[CurrentPointIndex];
	FVector CurrentLocation = GetActorLocation();
    
	// 목표 지점으로 이동
	FVector NewLocation = FMath::VInterpTo(CurrentLocation, TargetPoint, MovementInterval, 2.0f);
	SetActorLocation(NewLocation);
    
	// 목표 지점에 도달했는지 체크
	if (FVector::Dist(NewLocation, TargetPoint) < 50.0f)
	{
		CurrentPointIndex = (CurrentPointIndex + 1) % MovementPoints.Num();
		UE_LOG(LogTemp, Warning, TEXT("Advanced_MovingActor reached point %d"), CurrentPointIndex);
	}
}

void AAdvanced_MovingActor::OnMovementComplete()
{
	UE_LOG(LogTemp, Error, TEXT("이동완료! %f 초후에 액터가 사라집니다. "), DisappearDelay );
	// 이동 완료 후 DisappearDelay 시간 뒤에 사라지기 시작
	GetWorld()->GetTimerManager().SetTimer
	(	DisappearTimerHandle,
		this,
		&AAdvanced_MovingActor::DisappearPlatform,
		DisappearDelay,
		false
		);
	
}

void AAdvanced_MovingActor::StartNextCycle()
{
	//	이동 포인트가 충분히 있는지 확인
	StartAdvancedTimerSystem();
}

// Called every frame
void AAdvanced_MovingActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	/*if (!FMath::IsNearlyZero(RotationSpeed))
	{
		AddActorLocalRotation(FRotator(0.0f, RotationSpeed*DeltaTime, 0.0f));
	}

	FVector CurrentLocation = GetActorLocation();
	CurrentLocation += MovementSpeed * DeltaTime;
	SetActorLocation(CurrentLocation);

	float DistanceMoved = FVector::Dist(StartLocation, CurrentLocation);

	if (DistanceMoved > MaxRange)
	{
		FVector MoveDirection = MovementSpeed.GetSafeNormal();
		StartLocation = StartLocation + MoveDirection * MaxRange;
		SetActorLocation(StartLocation);
		MovementSpeed = -MovementSpeed;
	}*/

}








