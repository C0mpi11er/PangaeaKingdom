// Fill out your copyright notice in the Description page of Project Settings.


#include "CommonUI/PangaeaGameUI.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "PangaeaKingdom/PangaeaKingdomGameModeBase.h"
#include "PangaeaController/PangaeaPlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "PangaeaPawn/PangaeaPawn.h"
#include "PangaeaKingdom/Public/CommonUI/PangaeaCommonButtonBase.h"
#include "PangaeaKingdom/Public/CommonUI/PangaeaUIBuild.h"
#include "Widgets/CommonActivatableWidgetContainer.h"


void UPangaeaGameUI::AddStackItem(TSubclassOf<UCommonActivatableWidget> WidgetToAdd)
{
	if(!WidgetToAdd)return;
	if(MMainStack)
	{
		 //add widget to screen
		MMainStack->AddWidget(WidgetToAdd);
		
		
	}
}

void UPangaeaGameUI::NativeOnActivated()
{
	Super::NativeOnActivated();


	MPangaeaPlayerController = !MPangaeaPlayerController
		                           ? Cast<APangaeaPlayerController>(
			                           UGameplayStatics::GetPlayerController(GetWorld(), 0))
		                           : MPangaeaPlayerController;
	if (MPangaeaPlayerController)
	{
		UIAutoGenFunction();
		APawn*Pawn{MPangaeaPlayerController->GetPawn()};
		if(Pawn)
		{
			Pawn->EnableInput(MPangaeaPlayerController);
			Pawn->SetActorTickEnabled(true);
			SetIsFocusable(true);
			
		}
	}
	
	
	if(MBuildButton)
	{
		MBuildButton->OnClicked().AddUFunction(this,FName{"OnBuildButtonClick"});
	}
}

void UPangaeaGameUI::UIAutoGenFunction()
{
	//set ui mode to switch between game and ui
   UGameplayStatics::GetPlayerController(GetWorld(),0)->SetShowMouseCursor(false);
	
	MPangaeaPlayerController = !MPangaeaPlayerController
								   ? Cast<APangaeaPlayerController>(
									   UGameplayStatics::GetPlayerController(GetWorld(), 0))
								   : MPangaeaPlayerController;
	if (MPangaeaPlayerController)
	{
		if (MPangaeaPlayerController)
		{
			MPangaeaPlayerController->SetShowMouseCursor(true);
			UWidgetBlueprintLibrary::SetInputMode_GameAndUIEx(MPangaeaPlayerController);
			SetIsFocusable(true);
			
		}
	}
}

void UPangaeaGameUI::OnBuildButtonClick()
{
	if (APangaeaKingdomGameModeBase* PangaeaKingdomGameModeBase{
		Cast<APangaeaKingdomGameModeBase>(UGameplayStatics::GetGameMode(GetWorld()))
	})
	{
		MPangaeaPlayerController = !MPangaeaPlayerController
			                           ? Cast<APangaeaPlayerController>(
				                           UGameplayStatics::GetPlayerController(GetWorld(), 0))
			                           : MPangaeaPlayerController;
		if (MPangaeaPlayerController)
		{
			PangaeaKingdomGameModeBase->AddUserInterface(MPangaeaPlayerController->GetPangaeaBuildUIClass());
		}
	}
}
