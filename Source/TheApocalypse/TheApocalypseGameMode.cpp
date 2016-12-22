// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.

#include "TheApocalypse.h"
#include "TheApocalypseGameMode.h"
#include "TheApocalypseHUD.h"
#include "TheApocalypseCharacter.h"

ATheApocalypseGameMode::ATheApocalypseGameMode()
	: Super()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnClassFinder(TEXT("/Game/FirstPersonCPP/Blueprints/FirstPersonCharacter"));
	DefaultPawnClass = PlayerPawnClassFinder.Class;

	// use our custom HUD class
	HUDClass = ATheApocalypseHUD::StaticClass();
}
