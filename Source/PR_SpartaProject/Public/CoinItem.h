// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseItem.h"
#include "CoinItem.generated.h"

/**
 * 
 */
UCLASS()
class PR_SPARTAPROJECT_API ACoinItem : public ABaseItem
{
	GENERATED_BODY()

public:
	ACoinItem();

protected:
	UPROPERTY(editAnywhere, BlueprintReadWrite, Category = "Item")
	int32 PointValue;		// 점수 변수 

	virtual void ActivateItem(AActor* Activator) override;
};
