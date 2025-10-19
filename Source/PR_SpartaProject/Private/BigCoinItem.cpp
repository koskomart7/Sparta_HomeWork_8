// Fill out your copyright notice in the Description page of Project Settings.


#include "BigCoinItem.h"

#include "SpartaCharacter.h"

ABigCoinItem::ABigCoinItem()
{
	PointValue = 50;
	ItemType = "BigCoin";

	// 🆕 디버프 설정
	DebuffChance = 0.5f; // 50% 확률
	DebuffDuration = 4.0f; // 4초

}

void ABigCoinItem::ActivateItem(AActor* Activator)
{
	Super::ActivateItem(Activator);

	// 디버프 적용 로직
	if (Activator && Activator->ActorHasTag("Player"))
	{
		float RandomValue = FMath::FRandRange(0.0f, 1.0f);

		if (RandomValue <= DebuffChance)
		{
			if (ASpartaCharacter* PlayerCharacter = Cast<ASpartaCharacter>(Activator))
			{
				PlayerCharacter->ApplyReverseControlDebuff(DebuffDuration);
				// 시각 피드백
				GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Red, 
					TEXT(" 큰 보상엔 큰 대가! 조작이 반전됐다!"));

				UE_LOG(LogTemp, Warning, TEXT("BigCoin: ReverseControl 디버프 적용! (확률: %.0f%%)"), DebuffChance * 100.0f);
			}
		}
	}
}
