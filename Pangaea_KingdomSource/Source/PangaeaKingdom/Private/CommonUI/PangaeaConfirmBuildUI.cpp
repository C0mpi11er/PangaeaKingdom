// Fill out your copyright notice in the Description page of Project Settings.


#include "CommonUI/PangaeaConfirmBuildUI.h"

#include "Blueprint/WidgetBlueprintLibrary.h"
#include "PangaeaController/PangaeaPlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "PangaeaPawn/PangaeaPawn.h"
#include "CommonUI/PangaeaCommonButtonBase.h"


void UPangaeaConfirmBuildUI::NativeOnActivated()
{
	Super::NativeOnActivated();

	MPangaeaPlayerController = !MPangaeaPlayerController
							   ? Cast<APangaeaPlayerController>(
								   UGameplayStatics::GetPlayerController(this, 0))
							   : MPangaeaPlayerController;

	if(MPangaeaPlayerController)
	{
		UIAutoGenFunction();
		if(APangaeaPawn*PangaeaPawn{(Cast<APangaeaPawn>(MPangaeaPlayerController->GetPawn()))})
		{
			PangaeaPawn->EnableInput(MPangaeaPlayerController);
			PangaeaPawn->SetActorTickEnabled(true);
			UWidgetBlueprintLibrary::SetFocusToGameViewport();
		}	
	}

	if(MConfirmBuildButton)
		MConfirmBuildButton->OnClicked().AddUFunction(this,FName{"OnConfirmButtonClicked"});

	if(MRotateBuildButton)
		MRotateBuildButton->OnClicked().AddUFunction(this,FName{"OnRotateButtonClicked"});

	if(MCancelBuildButton)
		MCancelBuildButton->OnClicked().AddUFunction(this,FName{"OnCancelBuildButtonClicked"});
}

void UPangaeaConfirmBuildUI::UIAutoGenFunction()
{
	//set ui mode to switch between game and ui

	if (MPangaeaPlayerController)
	{
		if (MPangaeaPlayerController)
		{
			MPangaeaPlayerController->SetShowMouseCursor(true);
			UWidgetBlueprintLibrary::SetInputMode_GameAndUIEx(MPangaeaPlayerController, (UWidget*)nullptr,
															  EMouseLockMode::DoNotLock, true, false);
			UWidgetBlueprintLibrary::SetFocusToGameViewport();
			
		}
	}
}

void UPangaeaConfirmBuildUI::OnConfirmButtonClicked()
{
	if(MPangaeaPlayerController)
	{
		if(APangaeaPawn*PangaeaPawn{(Cast<APangaeaPawn>(MPangaeaPlayerController->GetPawn()))})
		{
			UE_LOG(LogTemp,Warning,TEXT("ConfirmBuildPressed"))
			PangaeaPawn->SpawnInteractableObject();
			
		}
	}
}

void UPangaeaConfirmBuildUI::OnRotateButtonClicked()
{
	if(MPangaeaPlayerController)
	{
		if(APangaeaPawn*PangaeaPawn{(Cast<APangaeaPawn>(MPangaeaPlayerController->GetPawn()))})
		{
			UE_LOG(LogTemp,Warning,TEXT("RotateBuildPressed"))
			PangaeaPawn->RotateSpawnInteractable();
			
		}
	}
}

void UPangaeaConfirmBuildUI::OnCancelBuildButtonClicked()
{
	if(MPangaeaPlayerController)
	{
		if(APangaeaPawn*PangaeaPawn{(Cast<APangaeaPawn>(MPangaeaPlayerController->GetPawn()))})
		{
			  UE_LOG(LogTemp,Warning,TEXT("CancelBuildPressed"))
			PangaeaPawn->DestroySpawnInteractableObject();
			
		}
	}
}
