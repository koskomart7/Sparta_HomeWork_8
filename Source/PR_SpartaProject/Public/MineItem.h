// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseItem.h"
#include "MineItem.generated.h"

/**
 *   지뢰 아이템 - 폭발 시 Slowing + Blind 동시 디버프
 */
UCLASS()
class PR_SPARTAPROJECT_API AMineItem : public ABaseItem
{
	GENERATED_BODY()

public:
	AMineItem();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Item|Component")
	USphereComponent* ExplosionCollision;

	UPROPERTY(editAnywhere, BlueprintReadWrite, Category = "Item|Effects")
	UParticleSystem* ExplosionParticle;
	UPROPERTY(editAnywhere, BlueprintReadWrite, Category = "Item|Effects")
	USoundBase* ExplosionSound;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mine")
	float ExplosionDelay;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mine")
	float ExplosionRadius;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mine")
	int32 ExplosionDamage;

	// 디버프 설정
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mine|Debuff")
	float SlowDebuffDuration;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mine|Debuff")
	float SlowMultiplier;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mine|Debuff")
	float BlindDebuffDuration;


	bool bHasExploded;
	FTimerHandle ExplosionTimerHandle;
	
	virtual void ActivateItem(AActor* Activator) override;

	void Explode();
};
