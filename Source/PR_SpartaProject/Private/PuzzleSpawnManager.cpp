// Fill out your copyright notice in the Description page of Project Settings.


#include "PuzzleSpawnManager.h"
#include "MovingPlatform.h"
#include "RotatingPlatform.h"
#include "Advanced_MovingActor.h"


// Sets default values
APuzzleSpawnManager::APuzzleSpawnManager()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	// 기본 스폰 구역들 설정
	SpawnZoneCenters.Add(FVector(0.0f, 0.0f, 100.0f));
	SpawnZoneCenters.Add(FVector(500.0f, 0.0f, 100.0f));
	SpawnZoneCenters.Add(FVector(-500.0f, 0.0f, 100.0f));
	SpawnZoneCenters.Add(FVector(0.0f, 500.0f, 100.0f));
	SpawnZoneCenters.Add(FVector(0.0f, -500.0f, 100.0f));

}

// Called when the game starts or when spawned
void APuzzleSpawnManager::BeginPlay()
{
	Super::BeginPlay();
	// 게임 시작 시 자동으로 퍼즐 생성
	SpawnRandomPuzzle();

	// 동적 재스폰 시작
	if (bEnableDynamicRespawn)
	{
		StartDynamicRespawning();
	}

	// 웨이브 시스템 시작
	if (bEnableWaveSystem)
	{
		StartWaveSystem();
	}
}

void APuzzleSpawnManager::SpawnRandomPuzzle()
{
	ClearSpawnedPlatforms();

	if (PlatformClasses.Num() == 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("할당된 플랫폼 클래스 없음!"));
		return;
	}

	// 복잡도 계산 (웨이브 시스템이 활성화된 경우)
	int32 ComplexityLevel = bEnableWaveSystem ? GetComplexityForCurrentWave() : 1;
	
	// 생성할 플랫폼 개수 랜덤 결정 ( 복잡도에 따라 증가 )
	int32 NumPlatforms = FMath::RandRange( MinPlatforms, MaxPlatforms);
	if (bEnableWaveSystem && bIncreaseComplexityPerWave)
	{
		NumPlatforms = FMath::Min(NumPlatforms + (ComplexityLevel * 2), MaxPlatforms * 2);
	}
	
	UE_LOG(LogTemp, Warning, TEXT(" 생성된 플랫폼 수 : %d (웨이브 : %d, 웨이브난이도: %d) "),
		NumPlatforms, CurrentWave, ComplexityLevel);

	for (int32 i = 0; i < NumPlatforms; i++)
	{
		// 랜덤 액터 선택
		int32 ClassIndex = FMath::RandRange(0, PlatformClasses.Num() - 1);
		TSubclassOf<AActor>	SelectedActor = PlatformClasses[ClassIndex];

		// 스폰 위치 결정
		FVector SpawnLocation = bEnableAreaBasedSpawning ? GetZoneBasedSpawnLocation() : GetRandomSpawnLocation();
		// 유효한 스폰 위치 찾기 ( 최대 50번 시도 )
		bool bFoundValidLocation = false;
		for (int32 Attempts = 0; Attempts < 50; Attempts++)
		{
			if (IsValidSpawnLocation(SpawnLocation))
			{
				bFoundValidLocation = true;
				break;
			}
			SpawnLocation = bEnableAreaBasedSpawning? GetZoneBasedSpawnLocation() : GetRandomSpawnLocation();
		}
		
		if (!bFoundValidLocation)
		{
			UE_LOG(LogTemp, Warning, TEXT("플랫폼 %d 개의 유효한 스폰 장소를 찾을수 없다. "), i);
			continue;
		}

		// 랜덤 회전
		FRotator SpawnRotation = FRotator(
			FMath::FRandRange(-180.0f, 180.0f), 
			FMath::FRandRange(-180.0f, 180.0f),
			FMath::FRandRange(-180.0f, 180.0f)
			);

		// 액터 스폰
		FActorSpawnParameters SpawnParams;
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

		AActor* SpawnedActor = GetWorld()->SpawnActor<AActor>(
			SelectedActor,
			SpawnLocation,
			SpawnRotation,
			SpawnParams
			);

		if (SpawnedActor)
		{
			SpawnedPlatforms.Add(SpawnedActor);
			SetAdvancedRandomProperties(SpawnedActor, ComplexityLevel);

			UE_LOG(LogTemp, Warning, TEXT("생성된 액터들의 위치 : %s "),
				*SpawnLocation.ToString());
		}
	}
	UE_LOG(LogTemp, Warning, TEXT(" 랜덤 퍼즐 생성 완료. 총 액터개수 : %d"), 
		SpawnedPlatforms.Num());
}

void APuzzleSpawnManager::ClearSpawnedPlatforms()
{
	for (AActor* Platform : SpawnedPlatforms)
	{
		if (Platform && IsValid(Platform))
		{
			Platform->Destroy();
		}
	}
	SpawnedPlatforms.Empty();

	UE_LOG(LogTemp, Warning, TEXT(" 스폰된 모든 액터를 삭제 "));
}

// 웨이브 시스템 관련 함수들 
void APuzzleSpawnManager::StartWaveSystem()
{
	bWaveSystemActive = true;
	CurrentWave = 0;
	
	UE_LOG(LogTemp, Warning, TEXT("웨이브 시스템 시작!"));
	
	// 첫 번째 웨이브 시작
	SpawnNextWave();
}

void APuzzleSpawnManager::StopWaveSystem()
{
	bWaveSystemActive = false;
	GetWorld()->GetTimerManager().ClearTimer(WaveSpawnTimerHandle);
	
	UE_LOG(LogTemp, Warning, TEXT("웨이브 시스템 종료"));
}

void APuzzleSpawnManager::SpawnNextWave()
{
	if (!bWaveSystemActive || CurrentWave >= WaveCount)
	{
		UE_LOG(LogTemp, Warning, TEXT("모든 웨이브 완료!"));
		StopWaveSystem();
		return;
	}

	CurrentWave++;
	UE_LOG(LogTemp, Warning, TEXT("웨이브 생성중 %d/%d "), CurrentWave, WaveCount);

	// 로그라이크 요소: 각 웨이브마다 새로운 설정 생성
	GenerateRandomConfiguration();
	
	// 퍼즐 생성
	SpawnRandomPuzzle();

	// 다음 웨이브 타이머 설정
	if (CurrentWave < WaveCount)
	{
		GetWorld()->GetTimerManager().SetTimer(
			WaveSpawnTimerHandle,
			this,
			&APuzzleSpawnManager::SpawnNextWave,
			WaveInterval,
			false
		);
	}
}

void APuzzleSpawnManager::SetAdvancedRandomProperties(AActor* Platform, int32 ComplexityLevel)
{
	if (!Platform) return;

	// 복잡도에 따른 속성 강화
	float ComplexityMultiplier = 1.0f + (ComplexityLevel * 0.3f);
	
	// MovingPlatform 속성 설정
	if (AMovingPlatform* MovePlatform = Cast<AMovingPlatform>(Platform))
	{
		// 회전 속도 랜덤화
		float RandomRotationSpeed = FMath::RandRange(
		RotationSpeedRange.X,
		RotationSpeedRange.Y * ComplexityMultiplier
		);

		// 이동 속도 랜덤화
		FVector RandomMoveSpeed = FVector(
			FMath::RandRange(MovementSpeedRange.X, MovementSpeedRange.Y * ComplexityMultiplier),
			FMath::RandRange(-MovementSpeedRange.Y, MovementSpeedRange.Y) * 0.5f,
			FMath::RandRange(-MovementSpeedRange.Y, MovementSpeedRange.Y) * 0.3f
		);

		// 이동 범위 랜덤화
		float RandomMaxRange = FMath::RandRange(
			MovementRangeMinMax.X,
			MovementRangeMinMax.Y * ComplexityMultiplier
		);

		// Setter 함수로 안전하게 설정
		MovePlatform->SetRotationSpeed(RandomRotationSpeed);
		MovePlatform->SetMoveSpeed(RandomMoveSpeed);
		MovePlatform->SetMaxRange(RandomMaxRange);

		// 타이머 기능 활성화 (복잡도에 따라)
		if ( ComplexityLevel > 1 )
		{
			MovePlatform->SetTimerBasedMovement((true));
			MovePlatform->StartTimerBasedMovement();
		}
		
		if ( ComplexityLevel > 2 )
		{
			MovePlatform->SetDisappearSettings((true),
				FMath::RandRange(1.0f, 5.0f),
				FMath::RandRange(1.0f, 3.0f)
				);
			if (FMath::RandBool())
			{
				MovePlatform->TriggerDisappear();
			}
		}
		UE_LOG(LogTemp, Warning, TEXT("이동 플랫폼 무작위 속성 세트 - 회전속도 : %f, 최대범위: %f"), 
				RandomRotationSpeed, RandomMaxRange);
	}

	// RotatingPlatform 속성 설정
	else if (ARotatingPlatform* RotatePlatform =  Cast<ARotatingPlatform>(Platform))
	{
		// 회전 속도와 축 랜덤화
		float RandomRotationSpeed = FMath::RandRange(
			RotationSpeedRange.X,
			RotationSpeedRange.Y * ComplexityMultiplier
		);

		// 랜덤 회전축 생성
		FVector RandomAxis = FVector(
			FMath::RandRange(-1.0f, 1.0f),
			FMath::RandRange(-1.0f, 1.0f),
			FMath::RandRange(-1.0f, 1.0f)
		).GetSafeNormal();

		// RotatingPlatform의 SetRandomProperties 함수 호출
		RotatePlatform->SetRandomProperties(RandomRotationSpeed, RandomAxis);

		// 고급 기능 활성화 (복잡도에 따라)
		if (ComplexityLevel > 2)
		{
			if (FMath::RandBool())
			{
				RotatePlatform->StartPeriodicSpeedChange();
			}
			if (FMath::RandBool())
			{
				RotatePlatform->StartAxisChanges();
			}
			if (FMath::RandBool()) RotatePlatform->TriggerDisappear();
		}
	}
	// Advanced_MovingActor 속성 설정
	else if (AAdvanced_MovingActor* AdvancedActor = Cast<AAdvanced_MovingActor>(Platform))
	{
		// === 완전한 Advanced_MovingActor 랜덤 속성 설정 ===
    
		// 기본 속성들 랜덤 생성
		float RandomRotationSpeed = FMath::RandRange(
			RotationSpeedRange.X, 
			RotationSpeedRange.Y * ComplexityMultiplier
		);
    
		FVector RandomMovementSpeed = FVector(
			FMath::RandRange(MovementSpeedRange.X, MovementSpeedRange.Y * ComplexityMultiplier),
			FMath::RandRange(-MovementSpeedRange.Y, MovementSpeedRange.Y) * 0.5f,
			FMath::RandRange(-MovementSpeedRange.Y, MovementSpeedRange.Y) * 0.3f
		);
    
		float RandomMaxRange = FMath::RandRange(
			MovementRangeMinMax.X,
			MovementRangeMinMax.Y * ComplexityMultiplier
		);

		// Public Setter 함수로 안전하게 설정
		AdvancedActor->SetRotationSpeed(RandomRotationSpeed);
		AdvancedActor->SetMovementSpeed(RandomMovementSpeed);
		AdvancedActor->SetMaxRange(RandomMaxRange);
		
		// 타이머 설정
		float RandomDelay = FMath::RandRange(0.5f, 3.0f) / ComplexityMultiplier;
		float RandomDuration = FMath::RandRange(1.0f, 4.0f);
		float RandomInterval = FMath::RandRange(0.5f, 3.0f) / ComplexityMultiplier;
    
		AdvancedActor->SetDisappearSettings(RandomDelay, RandomDuration, RandomInterval);

		// MovementPoints 랜덤 생성
		TArray<FVector> RandomMovementPoints;
		int32 NumPoints = FMath::RandRange(3, 8);
		FVector CurrentLocation = AdvancedActor->GetActorLocation();
    
		for (int32 i = 0; i < NumPoints; i++)
		{
			FVector RandomPoint = FVector(
				CurrentLocation.X + FMath::RandRange(-RandomMaxRange, RandomMaxRange),
				CurrentLocation.Y + FMath::RandRange(-RandomMaxRange, RandomMaxRange),
				CurrentLocation.Z + FMath::RandRange(-RandomMaxRange * 0.3f, RandomMaxRange * 0.3f)
			);
			RandomMovementPoints.Add(RandomPoint);
		}
    
		AdvancedActor->SetMovementPoints(RandomMovementPoints);

		// 복잡도에 따른 고급 기능 활성화
		if (ComplexityLevel > 2)
		{
			AdvancedActor->StartAdvancedTimerSystem();
		}

		UE_LOG(LogTemp, Warning, TEXT("Advanced_MovingActor fully randomized - Speed: %f, Range: %f, Points: %d"), 
			RandomRotationSpeed, RandomMaxRange, NumPoints);
		
	}
	// 공통 속성들 (모든 액터)
	float RandomScale = FMath::RandRange(ScaleRange.X, ScaleRange.Y);
	Platform->SetActorScale3D(FVector(RandomScale));

	UE_LOG(LogTemp, Warning, TEXT("Advanced random properties applied to %s (Complexity: %d)"), 
		*Platform->GetClass()->GetName(), ComplexityLevel);
		
}

bool APuzzleSpawnManager::IsValidSpawnLocation(const FVector& Location)
{
	// 기존 플랫폼과의 거리 체크
	for (AActor* ExistingPlatform : SpawnedPlatforms)
	{
		if (ExistingPlatform && IsValid((ExistingPlatform)))
		{
			float Distance = FVector::Dist(Location, ExistingPlatform->GetActorLocation());
			if (Distance < MinSpawnDistance)
			{
				return false;
			}
		}
	}
	return true;
}

FVector APuzzleSpawnManager::GetRandomSpawnLocation()
{
	return FVector(
		FMath::FRandRange(SpawnAreaMin.X, SpawnAreaMax.X),
		FMath::FRandRange(SpawnAreaMin.Y, SpawnAreaMax.Y),
		FMath::FRandRange(SpawnAreaMin.Z, SpawnAreaMax.Z)
	);
}

FVector APuzzleSpawnManager::GetZoneBasedSpawnLocation()
{
	if (SpawnZoneCenters.Num() == 0)
	{
		return GetRandomSpawnLocation();
	}

	// 랜덤 구역 선택
	int32 ZoneIndex = FMath::RandRange(0, SpawnZoneCenters.Num() - 1);
	FVector ZoneCenter = SpawnZoneCenters[ZoneIndex];

	// 구역 내 랜덤 위치 생성
	FVector RandomOffset = FVector(
		FMath::RandRange(-SpawnZoneRadius, SpawnZoneRadius),
		FMath::RandRange(-SpawnZoneRadius, SpawnZoneRadius),
		FMath::RandRange(-SpawnZoneRadius * 0.5f, SpawnZoneRadius * 0.5f)
	);

	return ZoneCenter + RandomOffset;
}

int32 APuzzleSpawnManager::GetComplexityForCurrentWave()
{
	// 웨이브에 따른 복잡도 계산
	return FMath::Max(1, CurrentWave + 1);
}

void APuzzleSpawnManager::GenerateRandomConfiguration()
{
	// 로그라이크 스타일 설정 생성
	MinPlatforms = FMath::RandRange(3, 8);
	MaxPlatforms = FMath::RandRange(MinPlatforms + 5, MinPlatforms + 15);
	
	// 랜덤 스폰 구역 생성
	SpawnZoneCenters.Empty();
	int32 NumZones = FMath::RandRange(3, 7);
	for (int32 i = 0; i < NumZones; i++)
	{
		FVector RandomZone = FVector(
			FMath::RandRange(-1000.0f, 1000.0f),
			FMath::RandRange(-1000.0f, 1000.0f),
			FMath::RandRange(0.0f, 300.0f)
		);
		SpawnZoneCenters.Add(RandomZone);
	}

	UE_LOG(LogTemp, Warning, TEXT("무작위 구성 생성 - 구역 : %d, 액터 : %d-%d"), 
		NumZones, MinPlatforms, MaxPlatforms);
}

void APuzzleSpawnManager::CreateDifficultySpike(int32 WaveNumber)
{
	// 난이도 스파이크 생성 (예: 보스 플랫폼, 특수 패턴 등)
	UE_LOG(LogTemp, Warning, TEXT("웨이브에 난이도 스파이크 생성 %d"), WaveNumber);
	// 여기에 특수한 플랫폼 생성 로직 추가
}

void APuzzleSpawnManager::DynamicRespawn()
{
	UE_LOG(LogTemp, Warning, TEXT("동적 리스폰 트리거"));
	
	// 기존 플랫폼의 일부를 제거하고 새로운 것으로 교체
	int32 PlatformsToReplace = FMath::Max(1, SpawnedPlatforms.Num() / 3);
	
	for (int32 i = 0; i < PlatformsToReplace && SpawnedPlatforms.Num() > 0; i++)
	{
		int32 RandomIndex = FMath::RandRange(0, SpawnedPlatforms.Num() - 1);
		if (SpawnedPlatforms[RandomIndex] && IsValid(SpawnedPlatforms[RandomIndex]))
		{
			SpawnedPlatforms[RandomIndex]->Destroy();
			SpawnedPlatforms.RemoveAt(RandomIndex);
		}
	}

	// 새로운 플랫폼들 생성
	int32 NewPlatformCount = FMath::RandRange(2, 5);
	for (int32 i = 0; i < NewPlatformCount && PlatformClasses.Num() > 0; i++)
	{
		int32 ClassIndex = FMath::RandRange(0, PlatformClasses.Num() - 1);
		TSubclassOf<AActor> SelectedClass = PlatformClasses[ClassIndex];

		FVector SpawnLocation = GetRandomSpawnLocation();
		FRotator SpawnRotation = FRotator(FMath::FRandRange(-180.0f, 180.0f), 
			FMath::FRandRange(-180.0f, 180.0f), FMath::FRandRange(-180.0f, 180.0f));

		FActorSpawnParameters SpawnParams;
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

		AActor* NewActor = GetWorld()->SpawnActor<AActor>(SelectedClass, SpawnLocation, SpawnRotation, SpawnParams);
		if (NewActor)
		{
			SpawnedPlatforms.Add(NewActor);
			SetAdvancedRandomProperties(NewActor, FMath::Max(1, CurrentWave));
		}
	}
}

// 공개 제어 함수들 
void APuzzleSpawnManager::StartDynamicRespawning()
{
	GetWorld()->GetTimerManager().SetTimer(
		DynamicRespawnTimerHandle,
		this,
		&APuzzleSpawnManager::DynamicRespawn,
		DynamicRespawnInterval,
		true ); // 반복
	UE_LOG(LogTemp, Warning, TEXT("동적 리스폰 시작"));
}

void APuzzleSpawnManager::StopDynamicRespawning()
{
	GetWorld()->GetTimerManager().ClearTimer(DynamicRespawnTimerHandle);
	UE_LOG(LogTemp, Warning, TEXT("동적 리스폰이 중지되었습니다."));
}

void APuzzleSpawnManager::SpawnSpecificWave(int32 WaveNumber)
{
	if (WaveNumber > 0 && WaveNumber <= WaveCount)
	{
		CurrentWave = WaveNumber;
		SpawnRandomPuzzle();
		UE_LOG(LogTemp, Warning, TEXT("수동으로 생성된 파도 %d"), WaveNumber);
	}
}

void APuzzleSpawnManager::AddSpawnZone(FVector ZoneCenter)
{
	SpawnZoneCenters.Add(ZoneCenter);
	UE_LOG(LogTemp, Warning, TEXT("다음 위치에 스폰 구역을 추가했습니다.: %s "), *ZoneCenter.ToString());
}

void APuzzleSpawnManager::ClearSpawnZones()
{
	SpawnZoneCenters.Empty();
	UE_LOG(LogTemp, Warning, TEXT("모든 스폰 구역을 클리어했습니다."));
}

// Called every frame
void APuzzleSpawnManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}