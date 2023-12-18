// Fill out your copyright notice in the Description page of Project Settings.


#include "PangaeaPawn/Interfaces/PangaeaPawnInterface.h"
#include "Interactable/InteractableObject.h"
// Add default functionality here for any IPangaeaPawnInterface functions that are not pure virtual.

void IPangaeaPawnInterface::BeginBuild(TSubclassOf<AInteractableObject> InteractableClass)
{
	UE_LOG(LogTemp,Warning,TEXT("beginbuild interface called"));
}

void IPangaeaPawnInterface::SwitchBuildMode(bool CanSwitchToBuildMode)
{
	UE_LOG(LogTemp,Warning,TEXT("switched to build mode"))
}

void IPangaeaPawnInterface::AddUserInterface(TSubclassOf<UCommonActivatableWidget> NewUIClass)
{
	UE_LOG(LogTemp,Warning,TEXT("Added new ui interface"))
}
