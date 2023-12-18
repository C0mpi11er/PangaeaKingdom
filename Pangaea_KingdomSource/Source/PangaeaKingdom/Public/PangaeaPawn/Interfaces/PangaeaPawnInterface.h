// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CommonActivatableWidget.h"
#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "PangaeaPawnInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UPangaeaPawnInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class PANGAEAKINGDOM_API IPangaeaPawnInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
       
	  virtual void BeginBuild(TSubclassOf<class AInteractableObject> InteractableClass);
	  virtual  void SwitchBuildMode(bool CanSwitchToBuildMode);
	  virtual void AddUserInterface(TSubclassOf<class UCommonActivatableWidget>NewUIClass);
};




