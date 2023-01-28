// Copyright Epic Games, Inc. All Rights Reserved.

#include "DWGameMode.h"
#include "DWCharacter.h"
#include "UObject/ConstructorHelpers.h"

ADWGameMode::ADWGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPersonCPP/Blueprints/ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}
