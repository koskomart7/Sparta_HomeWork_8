// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ItemInterface.h"
#include "BaseItem.generated.h"

class USphereComponent;
UCLASS()
class PR_SPARTAPROJECT_API ABaseItem : public AActor, public IItemInterface
{
	GENERATED_BODY()
	
public:	

	// Called every frame
	virtual void Tick(float DeltaTime) override;
	// Sets default values for this actor's properties
	ABaseItem();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	virtual void OnItemOverlap(
		UPrimitiveComponent* OverlappedComp,	// 오버랩이 발생된 자기 자신 
		AActor* OtherActor,	// 콜리전에 부딪힌 상대 액터 
		UPrimitiveComponent* OtherComp,	//	상대 액터에서 충돌이 발생된 컴포넌트 
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult) override;	// ( 순수가상함수 )'= 0' 을 할당하면 반드시 override 해야한다
	
	virtual void OnItemEndOverlap(
		UPrimitiveComponent* OverlappedComp,	// 오버랩이 발생된 자기 자신 
		AActor* OtherActor,	// 콜리전에 부딪힌 상대 액터 
		UPrimitiveComponent* OtherComp,	//	상대 액터에서 충돌이 발생된 컴포넌트 
		int32 OtherBodyIndex) override;	// 인터페이스는 반드시 구현해야하기 때문  
	virtual void ActivateItem(AActor* Activator) override;
	virtual FName GetItemType() const override;

	virtual void DestroyItem();
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
	FName ItemType;

	UPROPERTY(visibleAnywhere, BlueprintReadWrite, Category = "Item|Component")
	USceneComponent* Scene;
	UPROPERTY(visibleAnywhere, BlueprintReadWrite, Category = "Item|Component")
	USphereComponent* Collision;
	UPROPERTY(visibleAnywhere, BlueprintReadWrite, Category = "Item|Component")
	UStaticMeshComponent* StaticMesh;
	UPROPERTY(editAnywhere, BlueprintReadWrite, Category = "Item|Effects")
	UParticleSystem* PickupParticle;
	UPROPERTY(editAnywhere, BlueprintReadWrite, Category = "Item|Effects")
	USoundBase* PickupSound;
	

};
