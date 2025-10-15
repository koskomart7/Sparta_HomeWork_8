// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "SpartaGameMode.generated.h"

/**
 * 
 */
UCLASS()
class PR_SPARTAPROJECT_API ASpartaGameMode : public AGameMode
{
	GENERATED_BODY()

public:
	ASpartaGameMode();

	virtual void BeginPlay() override;

protected:
	class ASpartaGameState* SpartaGameState;

	//웨이브 관리
	UFUNCTION(BlueprintCallable, Category = "Wave")
	void InitializeWaves();
};
