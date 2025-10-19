// Fill out your copyright notice in the Description page of Project Settings.


#include "HealingItem.h"

#include "SpartaCharacter.h"

AHealingItem::AHealingItem()
{
	HealAmount = 20.0f;
	ItemType = "Healing";

	// 디버프 설정
	DebuffChance = 0.7f; // 70% 확률
	DebuffDuration = 3.0f; // 3초
}

void AHealingItem::ActivateItem(AActor* Activator)
{
	Super::ActivateItem(Activator);

	if (Activator && Activator->ActorHasTag("Player"))
	{
		if ( ASpartaCharacter* PlayerCharacter = Cast<ASpartaCharacter>(Activator))
		{
			PlayerCharacter -> AddHealth(HealAmount);

			// 디버프 적용 로직
			float RandomValue = FMath::RandRange(0.0f, 1.0f);
			if (RandomValue <= DebuffChance)
			{
				PlayerCharacter -> ApplyBlindedDebuff(DebuffDuration);

				// 로그
				GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Yellow, 
					TEXT(" 치료의 부작용! 일시적으로 시야가 흐려진다..."));
				UE_LOG(LogTemp, Warning, TEXT("HealingItem: Blind 디버프 적용! (확률: %.0f%%)"), DebuffChance * 100.0f);
			}
			else
			{
				// 디버프 미적용 시 긍정 메시지
				GEngine->AddOnScreenDebugMessage(-1, 1.5f, FColor::Green, 
					TEXT(" 완벽한 치료! 부작용 없음"));
			}
		}
		
		DestroyItem();
	}
}
