// Fill out your copyright notice in the Description page of Project Settings.


#include "MineItem.h"

#include "SpartaCharacter.h"
#include "Components/SphereComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystemComponent.h"

AMineItem::AMineItem()
{
	ExplosionCollision = CreateDefaultSubobject<USphereComponent>("ExplosionCollision");
	ExplosionCollision -> InitSphereRadius(ExplosionRadius);
	ExplosionCollision -> SetCollisionProfileName(TEXT("OverlapAllDynamic"));
	ExplosionCollision-> SetupAttachment(Scene);
	
	ExplosionDelay = 0.7f;
	ExplosionRadius = 300.0f;
	ExplosionDamage = 30.0f;
	ItemType = "Mine";
	bHasExploded = false;

	//  디버프 설정
	SlowDebuffDuration = 5.0f;
	SlowMultiplier = 0.5f; // 50% 속도
	BlindDebuffDuration = 5.0f;

}

void AMineItem::ActivateItem(AActor* Activator)
{
	if (bHasExploded) return;
	
	Super::ActivateItem(Activator);
	// 게임 월드 -> 타이머 매니저 -> 타이머 핸들러
	GetWorld()->GetTimerManager().SetTimer(
		ExplosionTimerHandle,
		this,
		&AMineItem::Explode,
		ExplosionDelay,
		false);
	
	bHasExploded = true;
}

void AMineItem::Explode()
{
	UParticleSystemComponent* Particle = nullptr;
	if (ExplosionParticle)
	{
		Particle = UGameplayStatics::SpawnEmitterAtLocation(
		  GetWorld(),
		  ExplosionParticle,
		  GetActorLocation(),
		  GetActorRotation(),
		  false);
	}
	if (ExplosionSound)
	{
		UGameplayStatics::PlaySoundAtLocation(
			GetWorld(),
			ExplosionSound,
			GetActorLocation()
			);
	}
	// 범위내 겹치는 액터 검색
	TArray<AActor*> OverlappingActors;
	ExplosionCollision->GetOverlappingActors(OverlappingActors);

	for ( AActor* Actor : OverlappingActors )
	{
		if (Actor && Actor->ActorHasTag("Player"))
		{
			UGameplayStatics::ApplyDamage(
				Actor,
				ExplosionDamage,
				nullptr,
				this,
				UDamageType::StaticClass()
				);
			//디버프 적용
			if (ASpartaCharacter* PlayerCharacter = Cast<ASpartaCharacter>(Actor))
			{
				// Slowing 디버프
				PlayerCharacter->ApplySlowingDebuff(SlowDebuffDuration, SlowMultiplier);
				// Blind 디버프
				PlayerCharacter->ApplyBlindedDebuff(BlindDebuffDuration);
				// 로그 
				GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Red, 
					TEXT("💣 지뢰 폭발! 이동 속도 감소 + 시야 제한!"));
				UE_LOG(LogTemp, Warning, TEXT("MineItem: 폭발! Slowing + Blind 디버프 동시 적용"));
			}
		}
	}
	
	DestroyItem();

	if (Particle)
	{
		FTimerHandle DestroyParticleTimerHandle;
		TWeakObjectPtr<UParticleSystemComponent> WeakParticle = Particle;

		GetWorld()->GetTimerManager().SetTimer(
			DestroyParticleTimerHandle,
			[WeakParticle]()
			{
				if (WeakParticle.IsValid())
				{
					WeakParticle->DestroyComponent();
				}
			},
			2.0f,
			false );
	}
	
}
