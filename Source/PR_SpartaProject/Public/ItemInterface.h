// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "ItemInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UItemInterface : public UInterface	// 리플렉션용 클래스 
{
	GENERATED_BODY()
};

/**
 * 
 */
class PR_SPARTAPROJECT_API IItemInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	UFUNCTION()
	virtual void OnItemOverlap(
		UPrimitiveComponent* OverlappedComp,	// 오버랩이 발생된 자기 자신 
		AActor* OtherActor,	// 콜리전에 부딪힌 상대 액터 
		UPrimitiveComponent* OtherComp,	//	상대 액터에서 충돌이 발생된 컴포넌트 
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult ) = 0;
	UFUNCTION()
	virtual void OnItemEndOverlap(
		UPrimitiveComponent* OverlappedComp,	// 오버랩이 발생된 자기 자신 
		AActor* OtherActor,	// 콜리전에 부딪힌 상대 액터 
		UPrimitiveComponent* OtherComp,	//	상대 액터에서 충돌이 발생된 컴포넌트 
		int32 OtherBodyIndex ) = 0;  

	
	virtual void ActivateItem(AActor* Activator) = 0;  // ( 순수가상함수 )'= 0' 을 할당하면 반드시 override 해야한다  // 인터페이스는 반드시 구현해야하기 때문
	virtual FName GetItemType() const = 0;	// Fname은 Fstring 보다 가볍고 타입을 빠르게 알아내기 좋다 
	
};
