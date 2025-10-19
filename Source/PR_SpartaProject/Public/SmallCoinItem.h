// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CoinItem.h"
#include "SmallCoinItem.generated.h"

/**
 *	// 30% 확률로 이동속도 감소 디버프
 */
UCLASS()
class PR_SPARTAPROJECT_API ASmallCoinItem : public ACoinItem
{
	GENERATED_BODY()

	ASmallCoinItem();

	virtual void ActivateItem(AActor* Activator) override;

protected:
	//  디버프 발생 확률 (0.0 ~ 1.0)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item|Debuff", meta = (ClampMin = "0.0", ClampMax = "1.0"))
	float DebuffChance;

	// 디버프 지속 시간
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item|Debuff")
	float DebuffDuration;

	// 속도 감소 배율
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item|Debuff")
	float SlowMultiplier;
};
