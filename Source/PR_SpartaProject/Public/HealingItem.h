// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseItem.h"
#include "HealingItem.generated.h"

/**
 *  치료 아이템 - 20% 확률로 시야 제한 부작용
 */
UCLASS()
class PR_SPARTAPROJECT_API AHealingItem : public ABaseItem
{
	GENERATED_BODY()

public:
	AHealingItem();

	UPROPERTY(editAnywhere, BlueprintReadWrite, Category = "Item")
	int32 HealAmount;

	virtual void ActivateItem(AActor* Activator) override;

protected:
	// 디버프 발생 확률
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item|Debuff", meta = (ClampMin = "0.0", ClampMax = "1.0"))
	float DebuffChance;

	// 디버프 지속 시간
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item|Debuff")
	float DebuffDuration;

};
