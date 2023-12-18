// Copyright Epic Games, Inc. All Rights Reserved.

#include "PangaeaKingdomGameModeBase.h"
#include "Blueprint/UserWidget.h"
#include "PangaeaController/PangaeaPlayerController.h"
#include "PangaeaKingdom/Public/CommonUI/PangaeaGameUI.h"
#include "Kismet/GameplayStatics.h"

void APangaeaKingdomGameModeBase::IntializeUI()
{
	if(MPangaeaPlayerController)
	{
		const auto UI{CreateWidget<UPangaeaGameUI>(MPangaeaPlayerController,MPangaeaPlayerController->GetPangaeaUIClass())};
		MPangaeaPlayerController->SetPangaeaGameUI(UI);
		if(UPangaeaGameUI*PangaeaGameUI{MPangaeaPlayerController->GetPangaeaGameUI()})
		{ 
			//init current game ui
			MUserInterfaceHUD=PangaeaGameUI;
			if(MUserInterfaceHUD)
			{
				MUserInterfaceHUD->AddToViewport();
				MUserInterfaceHUD->ActivateWidget();
				
			}
		}
	}
}

void APangaeaKingdomGameModeBase::AddUserInterface(TSubclassOf<UCommonActivatableWidget> NewUIClass)
{
	IPangaeaPawnInterface::AddUserInterface(NewUIClass);

	
	UE_LOG(LogTemp,Warning,TEXT("new ui added"))
	if(!NewUIClass||!MUserInterfaceHUD)return;

	if (UPangaeaGameUI*PangaeaGameUI{Cast<UPangaeaGameUI>(MUserInterfaceHUD)})
	{
		PangaeaGameUI->AddStackItem(NewUIClass);
	}
}

void APangaeaKingdomGameModeBase::BeginPlay()
{
	Super::BeginPlay();
	
	MPangaeaPlayerController=Cast<APangaeaPlayerController>(UGameplayStatics::GetPlayerController(this,0));
   IntializeUI();	
}
