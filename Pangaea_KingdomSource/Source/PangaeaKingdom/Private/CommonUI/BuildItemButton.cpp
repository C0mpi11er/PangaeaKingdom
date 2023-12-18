// Fill out your copyright notice in the Description page of Project Settings.

#include "CommonUI/BuildItemButton.h"

#include "CommonUI/PangaeaConfirmBuildUI.h"
#include "Components/TextBlock.h"
#include "Interactable/InteractableObject.h"
#include "Kismet/GameplayStatics.h"
#include "PangaeaController/PangaeaPlayerController.h"
#include "PangaeaKingdom/PangaeaKingdomGameModeBase.h"
#include "PangaeaPawn/PangaeaPawn.h"


void UBuildItemButton::NativePreConstruct()
{
	Super::NativePreConstruct();
	SetItemName();
	SetInteractableClass();
}

void UBuildItemButton::NativeOnClicked()
{ //this function triggers the begin build and set focus to game
	Super::NativeOnClicked();

	if (APangaeaPlayerController* PangaeaPlayerController{(Cast<APangaeaPlayerController>(GetOwningPlayer()))})
	{
		MPangaeaPlayerController = PangaeaPlayerController;
		if (APangaeaPawn* PangaeaPawn{(Cast<APangaeaPawn>(MPangaeaPlayerController->GetPawn()))}; PangaeaPawn &&
			MInteractableClassRefClass)
		{
			PangaeaPawn->BeginBuild(MInteractableClassRefClass);
			if (APangaeaKingdomGameModeBase* PangaeaKingdomGameModeBase{
				(Cast<APangaeaKingdomGameModeBase>(UGameplayStatics::GetGameMode(GetWorld())))
			})
			{
				PangaeaKingdomGameModeBase->AddUserInterface(MPangaeaPlayerController->GetPangaeConfirmBuildClass());
			}
		};
	}
}

void UBuildItemButton::SetItemName() const
{
	const FText ItemName{FText::FromName(MResourceType.MTitle)};
	if (MButtonName)
		MButtonName->SetText(ItemName);
}

void UBuildItemButton::SetInteractableClass()
{
	if (MResourceType.MInteractableObjectClass)
		MInteractableClassRefClass = MResourceType.MInteractableObjectClass;
}
