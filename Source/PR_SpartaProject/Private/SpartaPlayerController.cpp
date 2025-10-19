// Fill out your copyright notice in the Description page of Project Settings.


#include "SpartaPlayerController.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "inputmappingcontext.h"
#include "InputAction.h"
#include "UObject/ConstructorHelpers.h"
#include "SpartaGameInstance.h"
#include "SpartaGameState.h"
#include "Blueprint/UserWidget.h"
#include "Components/TextBlock.h"
#include "Kismet/GameplayStatics.h"

ASpartaPlayerController::ASpartaPlayerController()
{
	PlayerPawn = nullptr;	// 명시적으로 비어있다를 표시 
	InputMappingContext = nullptr;
	MoveAction = nullptr;
	JumpAction = nullptr;
	LookAction = nullptr;
	SprintAction = nullptr;
	ElevateAction = nullptr;
	RollAction = nullptr;
	HUDWidgetClass = nullptr;
	HUDWidgetInstance = nullptr;
	MainMenuWidgetClass = nullptr;
	MainMenuWidgetInstance = nullptr;
	GamePauseAction = nullptr;
	PauseMenuWidgetClass = nullptr; // 일시정지 메뉴 초기화
	PauseMenuWidgetInstance = nullptr;
	bIsGamePaused = false; // 일시정지 상태 초기화 
}

UUserWidget* ASpartaPlayerController::GetHUDWidget() const
{
	return HUDWidgetInstance;
}

void ASpartaPlayerController::ShowGameHud()
{
	if (HUDWidgetInstance)
	{
		HUDWidgetInstance->RemoveFromParent();
		HUDWidgetInstance = nullptr;
	}

	if (MainMenuWidgetInstance)
	{
		MainMenuWidgetInstance->RemoveFromParent();
		MainMenuWidgetInstance = nullptr;
	}

	if (HUDWidgetClass)
	{
		HUDWidgetInstance = CreateWidget<UUserWidget>(this, HUDWidgetClass);
		if (HUDWidgetInstance)
		{
			HUDWidgetInstance->AddToViewport();
			// 마우스를 ui만 사용할수 있게 처리 
			bShowMouseCursor = false;
			SetInputMode(FInputModeGameOnly());
		}

		ASpartaGameState* SpartaGameState = GetWorld() ? GetWorld()->GetGameState<ASpartaGameState>() : nullptr;
		if (SpartaGameState)
		{
			SpartaGameState->UpdateHUD();
		}
	}
}

void ASpartaPlayerController::ShowMainMenu(bool bIsRestart)
{
	// HUD가 켜져 있다면 닫기 
	if (HUDWidgetInstance)
	{
		HUDWidgetInstance->RemoveFromParent();
		HUDWidgetInstance = nullptr;
	}
	// 이미 메뉴가 떠 있으면 제거 
	if (MainMenuWidgetInstance)
	{
		MainMenuWidgetInstance->RemoveFromParent();
		MainMenuWidgetInstance = nullptr;
	}
	// 메뉴 UI 생성 
	if (MainMenuWidgetClass)
	{
		MainMenuWidgetInstance = CreateWidget<UUserWidget>(this, MainMenuWidgetClass);
		if (MainMenuWidgetInstance)
		{
			MainMenuWidgetInstance->AddToViewport();
			// 마우스를 ui만 사용할수 있게 처리 
			bShowMouseCursor = true;
			SetInputMode(FInputModeUIOnly());
		}

		if (UTextBlock* ButtonText = Cast<UTextBlock>(MainMenuWidgetInstance->GetWidgetFromName(TEXT("StartButtonText"))))
		{
			if (bIsRestart)
			{
				ButtonText->SetText(FText::FromString(TEXT("ReStart")));
			}
			else
			{
				ButtonText->SetText(FText::FromString(TEXT("Start")));
			}
		}

		if (bIsRestart)
		{
			UFunction* PlayAnimFunc = MainMenuWidgetInstance->FindFunction(FName("PlayGameOverAnim"));
			if (PlayAnimFunc)
			{
				MainMenuWidgetInstance->ProcessEvent(PlayAnimFunc, nullptr);
			}

			if (UTextBlock* TotalScoreText = Cast<UTextBlock>(MainMenuWidgetInstance->GetWidgetFromName("TotalScoreText")))
			{
				if (USpartaGameInstance* SpartaGameInstance = Cast<USpartaGameInstance>(UGameplayStatics::GetGameInstance(this)))
				{
					TotalScoreText->SetText(FText::FromString(
						FString::Printf(TEXT("Total Score: %d"), SpartaGameInstance->TotalScore)
					));
				}
			}
		}
	}
}

void ASpartaPlayerController::StartGame()
{
	if (USpartaGameInstance* SpartaGameInstance = Cast<USpartaGameInstance>(UGameplayStatics::GetGameInstance(this)))
	{
		SpartaGameInstance ->CurrentLevelIndex = 0;
		SpartaGameInstance ->TotalScore = 0;
	}

	UGameplayStatics::OpenLevel(GetWorld(), FName("BasicLevel"));
	SetPause(false);
}

void ASpartaPlayerController::QuitGame()
{
	UKismetSystemLibrary::QuitGame(
		GetWorld(),
		this,
		EQuitPreference::Quit,
		false);
}

void ASpartaPlayerController::TogglePauseGame()
{
	// 메인 메뉴나 게임 오버 상태에서는 일시정지를 사용하지 않음
	FString CurrentMapName = GetWorld()->GetMapName();
	if (CurrentMapName.Contains("MenuLevel"))
	{
		return;
	}
	if (bIsGamePaused)
	{
		// 게임 재개
		ResumeGame();
	}
	else
	{
		//게임일시 정지
		SetPause(true);
		bIsGamePaused = true;
		ShowPauseMenu();

		UE_LOG(LogTemp, Warning, TEXT("게임 일시정지"));
		GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Yellow, TEXT("게임 일시정지 (Q로 재개)"));
	}
}
// 일시정지 메뉴 표시
void ASpartaPlayerController::ShowPauseMenu()
{
	if (PauseMenuWidgetClass)
	{
		//기존 일시정지 메뉴가 있으면 제거
		if (PauseMenuWidgetInstance)
		{
			PauseMenuWidgetInstance->RemoveFromParent();
			PauseMenuWidgetInstance = nullptr;
		}

		PauseMenuWidgetInstance = CreateWidget(this, PauseMenuWidgetClass);
		if (PauseMenuWidgetInstance)
		{
			PauseMenuWidgetInstance -> AddToViewport(100);

			// 마우스 커서 표시 및 입력 모드 설정
			bShowMouseCursor = true;
			// UI와 게임 동시 입력 허용 (마우스는 UI에 잠기지 않음)
			FInputModeGameAndUI InputMode;
			InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
			InputMode.SetHideCursorDuringCapture(false);
			SetInputMode(InputMode);
		}
	}
}
// 일시정지 메뉴 숨기기 
void ASpartaPlayerController::HidePauseMenu()
{
	if (PauseMenuWidgetInstance)
	{
		PauseMenuWidgetInstance->RemoveFromParent();
		PauseMenuWidgetInstance = nullptr;
	}
}
// 게임 재개
void ASpartaPlayerController::ResumeGame()
{
	HidePauseMenu();
	SetPause(false);
	bIsGamePaused = false;

	bShowMouseCursor = false;
	// 입력 모드를 게임 전용으로 완전히 재설정
	FInputModeGameOnly InputMode;
	InputMode.SetConsumeCaptureMouseDown(false);
	SetInputMode(InputMode);

	// 뷰포트로 포커스 강제 이동
	if (GEngine && GEngine->GameViewport)
	{
		FViewport* Viewport = GEngine->GameViewport->Viewport;
		if (Viewport)
		{
			Viewport->CaptureMouse(true);
		}
	}
    
	// 입력 컴포넌트 재활성화 확인
	if (InputComponent)
	{
		InputComponent->bBlockInput = false;
	}
	
	UE_LOG(LogTemp, Warning, TEXT("게임 재개 - 입력 모드 복원"));
	GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Green, TEXT("게임 재개"));
}

void ASpartaPlayerController::BeginPlay()
{
	Super::BeginPlay();
	// 현재 PlayerController 에 연결된 Local Player 객체를 가져옴
	if (ULocalPlayer* LocalPlayer = GetLocalPlayer())
	{
		// Local Player에서 EnhancedInputLocalPlayerSubsystem을 획득
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem =
			LocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>())
		{
			if (InputMappingContext != nullptr)
			{
				// Subsystem을 통해 우리가 할당한 IMC를 활성화
				// 우선순위는 0이 가장 높은 순위
				Subsystem->AddMappingContext(InputMappingContext, 0);
			}
		}
	}

	FString CurrentMapName = GetWorld()->GetMapName();
	if (CurrentMapName.Contains("MenuLevel"))
	{
		ShowMainMenu(false);
	}
}
// SetupInputComponent 구현 (Q 키 바인딩) 
void ASpartaPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	if (UEnhancedInputComponent* EnhancedInput = Cast<UEnhancedInputComponent>(InputComponent))
	{
		// GamePauseAction 바인딩 ( Q키 )
		if (GamePauseAction)
		{
			EnhancedInput->BindAction(
				GamePauseAction,
				ETriggerEvent::Started,
				this,
				&ASpartaPlayerController::TogglePauseGame);
			UE_LOG(LogTemp, Warning, TEXT("GamePauseAction 바인딩 성공"));
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("GamePauseAction이 설정되지 않았습니다!"));
		}
	}
}
