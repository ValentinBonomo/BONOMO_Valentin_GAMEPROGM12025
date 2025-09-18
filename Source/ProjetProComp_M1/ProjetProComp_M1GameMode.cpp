// Copyright Epic Games, Inc. All Rights Reserved.

#include "ProjetProComp_M1GameMode.h"
#include "ProjetProComp_M1Character.h"
#include "UObject/ConstructorHelpers.h"

AProjetProComp_M1GameMode::AProjetProComp_M1GameMode()
{
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPerson/Blueprints/BP_ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}
