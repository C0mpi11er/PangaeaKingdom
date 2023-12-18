// Fill out your copyright notice in the Description page of Project Settings.


#include "CommonUI/PangaeaUIBuild.h"
#include "Kismet/GameplayStatics.h"
#include "PangaeaController/PangaeaPlayerController.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "CommonUI/BuildItemButton.h"
#include "CommonUI/PangaeaCommonButtonBase.h"
#include "PangaeaPawn/PangaeaPawn.h"


void UPangaeaUIBuild::NativeOnActivated()
{
	//Set Default Focus, Disable player input so background game doesn't move
	Super::NativeOnActivated();

	if (MBackButton)
		MBackButton->OnClicked().AddUFunction(this, FName{"OnBackButtonPressed"});

	MPangaeaPlayerController = !MPangaeaPlayerController
		                           ? Cast<APangaeaPlayerController>(
			                           UGameplayStatics::GetPlayerController(GetWorld(), 0))
		                           : MPangaeaPlayerController;


	if (MPangaeaPlayerController)
	{
		UWidgetBlueprintLibrary::SetInputMode_UIOnlyEx(MPangaeaPlayerController);
		if (GetDesiredFocusTarget())
		{
			GetDesiredFocusTarget()->SetFocus();
		}

		MPawn = !MPawn ? Cast<APangaeaPawn>(MPangaeaPlayerController->GetPawn()) : MPawn;
		if (MPawn)
		{
			MPawn->SetActorTickEnabled(false);
			MPawn->DisableInput(MPangaeaPlayerController);
			//automatically add build input actions when true
			MPawn->SwitchBuildMode(true);
		}
	}
}

void UPangaeaUIBuild::NativePreConstruct()
{
	Super::NativePreConstruct();
	if(MBackButton)
		MBackButton->SetButtonName(TEXT("Back"));
	if(MBuildItemOne)
		MBuildItemOne->SetButtonName(TEXT("BuildItemOne"));

	if(MBuildItemTwo)
		MBuildItemTwo->SetButtonName(TEXT("BuildItemTwo"));
}

void UPangaeaUIBuild::OnBackButtonPressed()
{
	MPangaeaPlayerController=!MPangaeaPlayerController?
	  Cast<APangaeaPlayerController>(UGameplayStatics::GetPlayerController(GetWorld(), 0)):MPangaeaPlayerController;
	if(MPangaeaPlayerController)
	{
		MPawn = !MPawn?Cast<APangaeaPawn>(MPangaeaPlayerController->GetPawn()):MPawn;
		if (MPawn)
		{
			
			MPawn->SetActorTickEnabled(true);
			MPawn->EnableInput(MPangaeaPlayerController);
			MPawn->SwitchBuildMode(false);
			UWidgetBlueprintLibrary::SetInputMode_GameAndUIEx(MPangaeaPlayerController);
		}
	}
	DeactivateWidget();
}
