// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CoinItem.h"
#include "BigCoinItem.generated.h"

/**
 *	// 큰 코인 - 50% 확률로 조작 반전 디버프
 */
UCLASS()
class PR_SPARTAPROJECT_API ABigCoinItem : public ACoinItem
{
	GENERATED_BODY()

public:
	ABigCoinItem();

	virtual void ActivateItem(AActor* Activator) override;

protected:
	// 🆕 디버프 발생 확률
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item|Debuff", meta = (ClampMin = "0.0", ClampMax = "1.0"))
	float DebuffChance;

	// 🆕 디버프 지속 시간
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item|Debuff")
	float DebuffDuration;
};
