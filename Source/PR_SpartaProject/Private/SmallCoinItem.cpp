// Fill out your copyright notice in the Description page of Project Settings.


#include "SmallCoinItem.h"

#include "SpartaCharacter.h"

ASmallCoinItem::ASmallCoinItem()
{
	PointValue = 10;
	ItemType = "SmallCoin";

	// 디버프 설정
	DebuffChance = 0.3f;	// 30% 확률
	DebuffDuration = 3.0f;
	SlowMultiplier = 0.7f;	// 70% 속도
}

void ASmallCoinItem::ActivateItem(AActor* Activator)
{
	Super::ActivateItem(Activator);

	// 디버프 적용 로직
	if (Activator && Activator->ActorHasTag("Player"))
	{
		// 확률 계산
		float RandomValue = FMath::FRandRange(0.0f, 1.0f);
		if (RandomValue <= DebuffChance)
		{
			//디버프 적용
			if (ASpartaCharacter* PlayerCharacter = Cast<ASpartaCharacter>(Activator))
			{
				PlayerCharacter->ApplySlowingDebuff(DebuffDuration, SlowMultiplier);

				// 로그
				GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Orange, TEXT(" 작은 코인이 함정이였다! 이동속도 감소! "));
				UE_LOG(LogTemp, Warning, TEXT("SmallCoin : 디버프 적용! (확률: %.0f%%)"), DebuffChance * 100.0f);
			}
		}
	}
}
