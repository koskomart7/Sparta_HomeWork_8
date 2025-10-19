// Fill out your copyright notice in the Description page of Project Settings.


#include "SpartaGameState.h"

#include "CoinItem.h"
#include "SpartaCharacter.h"
#include "SpartaGameInstance.h"
#include "SpartaPlayerController.h"
#include "SpawnVolume.h"
#include "Blueprint/UserWidget.h"
#include "Components/TextBlock.h"
#include "Kismet/GameplayStatics.h"

ASpartaGameState::ASpartaGameState()
{
	Score = 0;
	SpawnedCoinCount = 0;
	CollectedCoinCount = 0;
	LevelDuration = 120.0f; // 전체 레벨 시간 증가 
	CurrentLevelIndex = 0;
	MaxLevels = 3;

	//웨이브 시스템 초기화
	CurrentWaveIndex = 0;
	RemainingWaveTime = 0.0f;
	bIsWaveActive = false;
	bAllWavesCompleted = false;

	// 기본 웨이브 데이터 설정 (에디터에서 수정 가능)
	FWaveData Wave1;
	Wave1.WaveNumber = 1;
	Wave1.WaveTime = 30.0f;
	Wave1.ItemsToSpawn = 15;

	FWaveData Wave2;
	Wave2.WaveNumber = 2;
	Wave2.WaveTime = 40.0f;
	Wave2.ItemsToSpawn = 25;

	FWaveData Wave3;
	Wave3.WaveNumber = 3;
	Wave3.WaveTime = 50.0f;
	Wave3.ItemsToSpawn = 35;

	Waves.Add(Wave1);
	Waves.Add(Wave2);
	Waves.Add(Wave3);
}

void ASpartaGameState::BeginPlay()
{
	Super::BeginPlay();
	StartLevel();
	
	GetWorldTimerManager().SetTimer(
		HUDUpdateTimerHandle,
		this,
		&ASpartaGameState::UpdateHUD,
		0.1f,
		true);
}

int32 ASpartaGameState::GetScore() const
{
	return Score;
}

void ASpartaGameState::AddScore(int32 Amount)
{
	if (UGameInstance* GameInstance = GetGameInstance())
	{
		USpartaGameInstance* SpartaGameInstance = Cast<USpartaGameInstance>(GameInstance);
		if (SpartaGameInstance)
		{
			SpartaGameInstance->AddToScore(Amount);
		}
	}
}

void ASpartaGameState::OnGameOver()
{
	if (APlayerController* PlayerController = GetWorld()->GetFirstPlayerController())
	{
		if (ASpartaPlayerController* SpartaPlayerController = Cast<ASpartaPlayerController>(PlayerController))
		{
			SpartaPlayerController->SetPause(true);
			SpartaPlayerController->ShowMainMenu(true);
		}
	}
}

void ASpartaGameState::StartLevel()
{
	// 레벨이 오픈될때마다 hud ui를 켜준다 
	if (APlayerController* PlayerController = GetWorld()->GetFirstPlayerController())
	{
		if (ASpartaPlayerController* SpartaPlayerController = Cast<ASpartaPlayerController>(PlayerController))
		{
			SpartaPlayerController->ShowGameHud();
		}
	}
	
	if (UGameInstance* GameInstance = GetGameInstance())
	{
		USpartaGameInstance* SpartaGameInstance = Cast<USpartaGameInstance>(GameInstance);
		if (SpartaGameInstance)
		{
			CurrentLevelIndex = SpartaGameInstance->CurrentLevelIndex;
		}
	}
	//웨이브 시스템 시작 
	bAllWavesCompleted = false;

	FString LevelStartMsg = FString::Printf(TEXT("===== Level %d 시작 ====="), CurrentLevelIndex + 1);
	UE_LOG(LogTemp, Warning, TEXT("%s"), *LevelStartMsg);
	GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Green, LevelStartMsg);

	// 현재 레벨 이름 표시 
	if (LevelMapNames.IsValidIndex(CurrentLevelIndex))
	{
		FName CurrentLevelName = LevelMapNames[CurrentLevelIndex];
		FString TransitionMsg = FString::Printf(TEXT("현재 레벨 : %s"), *CurrentLevelName.ToString());
		UE_LOG(LogTemp, Warning, TEXT("%s"), *TransitionMsg);
		GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Cyan, TransitionMsg);
	}

	StartWave();
	
}

void ASpartaGameState::OnLevelTimeUp()
{
	// 더이상 사용되지 않지만 호환성을 위해 유지 
	EndLevel();
}

void ASpartaGameState::OnCoinCollected()
{
	CollectedCoinCount++;
	UE_LOG(LogTemp, Warning, TEXT("Coin Collected %d / %d "),
		CollectedCoinCount, SpawnedCoinCount);
	
	// 현재 웨이브의 모든 코인을 수집했다면 다음 웨이브로
	if (bIsWaveActive && SpawnedCoinCount > 0 && CollectedCoinCount >= SpawnedCoinCount )
	{
		GetWorldTimerManager().ClearTimer(WaveTimerHandle);
		FString CompleteMessage = FString::Printf(TEXT("Wave %d 완료! 다음 웨이브로..."),
			Waves[CurrentWaveIndex].WaveNumber);
		GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Green, CompleteMessage);

		AdvanceWave();
	}
}

void ASpartaGameState::EndLevel()
{
	// 모든 타이머 정리 
	GetWorldTimerManager().ClearTimer(LevelTimerHandle);
	GetWorldTimerManager().ClearTimer(WaveTimerHandle);
	bIsWaveActive = false;
	
	if (UGameInstance* GameInstance = GetGameInstance())
	{
		USpartaGameInstance* SpartaGameInstance = Cast<USpartaGameInstance>(GameInstance);
		if (SpartaGameInstance)
		{
			AddScore(Score);
			CurrentLevelIndex++;
			SpartaGameInstance->CurrentLevelIndex = CurrentLevelIndex;
		}
	}
	
	if (CurrentLevelIndex >= MaxLevels)
	{
		// 모든 레벨 완료
		FString GameCompleteMsg = TEXT("===== 게임 완료! 모든 레벨 클리어! =====");
		UE_LOG(LogTemp, Warning, TEXT("%s"), *GameCompleteMsg);
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Magenta, GameCompleteMsg);
		
		OnGameOver();
		return;
	}
	// 다음 레벨 맵으로 전환
	if (LevelMapNames.IsValidIndex(CurrentLevelIndex))
	{
		FName NextLevelName = LevelMapNames[CurrentLevelIndex];
		FString TransitionMsg = FString::Printf(TEXT("다음 레벨로 이동: %s"), *NextLevelName.ToString());
		UE_LOG(LogTemp, Warning, TEXT("%s"), *TransitionMsg);
		GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Cyan, TransitionMsg);
		// 레벨 전환
		UGameplayStatics::OpenLevel(GetWorld(), LevelMapNames[CurrentLevelIndex]);
	}
	else
	{
		OnGameOver();
	}
	
}

void ASpartaGameState::UpdateHUD()
{
	if (APlayerController* PlayerController = GetWorld()->GetFirstPlayerController())
	{
		if (ASpartaPlayerController* SpartaPlayerController = Cast<ASpartaPlayerController>(PlayerController))
		{
			if (UUserWidget* HUDWidget = SpartaPlayerController->GetHUDWidget())
			{
				// 웨이브 시간 표시
				if (UTextBlock* TimeText = Cast<UTextBlock>(HUDWidget->GetWidgetFromName(TEXT("Time"))))
				{
					if (bIsWaveActive)
					{
						float RemainingTime = GetWorldTimerManager().GetTimerRemaining(WaveTimerHandle);
						TimeText->SetText(FText::FromString(FString::Printf(TEXT("WaveTime : %.1f"), RemainingTime)));
					}
					else if (bAllWavesCompleted)
					{
						TimeText->SetText(FText::FromString(TEXT("Level Complete")));
					}
					else
					{
						TimeText->SetText(FText::FromString(TEXT("Next Wave...")));
					}
				}
				// 점수 표시 
				if (UTextBlock* ScoreText = Cast<UTextBlock>(HUDWidget->GetWidgetFromName(TEXT("Score"))))
				{
					if ( UGameInstance* GameInstance = GetGameInstance())
					{
						USpartaGameInstance* SpartaGameInstance = Cast<USpartaGameInstance>(GameInstance);
						if (SpartaGameInstance)
						{
							ScoreText->SetText(FText::FromString(FString::Printf(TEXT("Score: %d"), SpartaGameInstance->TotalScore)));
						}
					}
				}
				//웨이브 정보 표시
				if (UTextBlock* WaveText = Cast<UTextBlock>(HUDWidget->GetWidgetFromName(TEXT("Wave"))))
				{
					if (bAllWavesCompleted)
					{
						WaveText->SetText(FText::FromString(TEXT("Wave Complete")));
					}
					else if (const FWaveData* CurrentWave = GetCurrentWaveData())
					{
						WaveText->SetText(FText::FromString(FString::Printf(TEXT("Wave : %d / %d"), CurrentWave->WaveNumber, Waves.Num())));
					}
				}
				// 레벨 표시 
				if (UTextBlock* LevelIndexText = Cast<UTextBlock>(HUDWidget->GetWidgetFromName(TEXT("Level"))))
				{
					LevelIndexText->SetText(FText::FromString(FString::Printf(TEXT("Level : %d / %d"), CurrentLevelIndex + 1, MaxLevels)));
				}
				// 디버프 UI 업데이트
				if (APawn* PlayerPawn = PlayerController->GetPawn())
				{
					if (ASpartaCharacter* PlayerCharacter = Cast<ASpartaCharacter>(PlayerPawn))
					{
						// Character의 GetDebuffStatusText() 호출
						FString DebuffStatusText = PlayerCharacter->GetDebuffStatusText();
						// UI 텍스트 블록에 표시
						if (UTextBlock* DebuffText = Cast<UTextBlock>(HUDWidget->GetWidgetFromName(TEXT("DebuffStatus"))))
						{
							DebuffText->SetText(FText::FromString(DebuffStatusText));
						}
					}
				}
			}
		}
	}
}
// 웨이브 함수 구현부 ===============
void ASpartaGameState::StartWave()
{
	if (!Waves.IsValidIndex(CurrentWaveIndex))
	{
		// 모든 웨이브 완료
		CompleteAllWaves();
		UE_LOG(LogTemp, Warning, TEXT("모든 웨이브 완료!"));
		return;
	}
	const FWaveData& CurrentWave = Waves[CurrentWaveIndex];
	RemainingWaveTime = CurrentWave.WaveTime;
	bIsWaveActive = true;

	// 디버그 메세지 출력 ( 필수 조건 )
	FString WaveMessage = FString::Printf(TEXT("Wave %d 시작!"), CurrentWave.WaveNumber);
	UE_LOG(LogTemp, Warning, TEXT("%s"), *WaveMessage);
	GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, WaveMessage);

	// 상세 정보도 출력
	FString DetailMessage = FString::Printf(TEXT("시간: %.0f 초, 아이템: %d 개"),
		CurrentWave.WaveTime, CurrentWave.ItemsToSpawn);
	GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Red, DetailMessage);

	// 웨이브 시작시, 코인 개수 초기화 
	SpawnedCoinCount = 0;
	CollectedCoinCount = 0;

	// 현재 맵에 배치된 모든 SpawnVolume을 찾아 아이템 40개를 스폰 
	TArray<AActor*> FoundVolumes;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ASpawnVolume::StaticClass(), FoundVolumes);

	
	for (int32 i = 0; i < CurrentWave.ItemsToSpawn; i++)
	{
		if (FoundVolumes.Num() > 0 )
		{
			ASpawnVolume* SpawnVolume = Cast<ASpawnVolume>(FoundVolumes[0]);
			if (SpawnVolume)
			{
				AActor* SpawnedActor = SpawnVolume->SpawnRandomItem();
				if (SpawnedActor && SpawnedActor->IsA(ACoinItem::StaticClass())) //코인아이템 인지 아닌지 확인
				{
					SpawnedCoinCount++;
				}
			}
		}
	}
	//웨이브 타이머 시작
	GetWorldTimerManager().SetTimer(
	WaveTimerHandle,
	this,
	&ASpartaGameState::OnWaveTimeUp,
	CurrentWave.WaveTime,
	false
	);
}

void ASpartaGameState::AdvanceWave()
{
	CurrentWaveIndex++;
	if (Waves.IsValidIndex(CurrentWaveIndex))
	{
		// 다음 웨이브로 진행
		FString NextWaveMsg = FString::Printf(TEXT("다음 웨이브 준비 중..."));
		GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Orange, NextWaveMsg);
        
		// 약간의 딜레이 후 다음 웨이브 시작
		FTimerHandle DelayHandle;
		GetWorldTimerManager().SetTimer(DelayHandle,
			this,
			&ASpartaGameState::StartWave,
			2.0f,
			false);
	}
	else
	{
		CompleteAllWaves();
	}
}

void ASpartaGameState::OnWaveTimeUp()
{
	bIsWaveActive = false;
	
	FString TimeUpMessage = FString::Printf(TEXT("Wave %d 시간 종료!"),
		Waves[CurrentWaveIndex].WaveNumber);
	GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Orange, TimeUpMessage);

	AdvanceWave();
}

void ASpartaGameState::CompleteAllWaves()
{
	bAllWavesCompleted = true;
	bIsWaveActive = false;

	GetWorldTimerManager().ClearTimer(WaveTimerHandle);

	FString CompleteMsg = FString::Printf(TEXT("===== 모든 웨이브 완료! Level %d 클리어! ====="), CurrentLevelIndex + 1);
	UE_LOG(LogTemp, Warning, TEXT("%s"), *CompleteMsg);
	GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Green, CompleteMsg);

	// 2초 후 다음 레벨로 전환
	FTimerHandle TransitionHandle;
	GetWorldTimerManager().SetTimer(
		TransitionHandle,
		this,
		&ASpartaGameState::EndLevel,
		3.0f,
		false);
}

const FWaveData* ASpartaGameState::GetCurrentWaveData() const
{
	if (Waves.IsValidIndex(CurrentWaveIndex))
	{
		return &Waves[CurrentWaveIndex];
	}
	return nullptr;
}

