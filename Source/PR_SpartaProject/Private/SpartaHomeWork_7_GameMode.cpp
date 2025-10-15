// Fill out your copyright notice in the Description page of Project Settings.


#include "SpartaHomeWork_7_GameMode.h"

#include "Character_SpartaHomeWork_7.h"
#include "SpartaPlayerController.h"

ASpartaHomeWork_7_GameMode::ASpartaHomeWork_7_GameMode()
{
	DefaultPawnClass = ACharacter_SpartaHomeWork_7::StaticClass();	// Uclass 형태의 타입 반환. 리플렉션시스템 지원
	PlayerControllerClass = ASpartaPlayerController::StaticClass();
}
