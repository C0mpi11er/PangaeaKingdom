// Fill out your copyright notice in the Description page of Project Settings.


#include "CommonUI/PangaeaCommonButtonBase.h"
#include "CommonActionWidget.h"
#include "CommonTextBlock.h"
#include "Components/TextBlock.h"

void UPangaeaCommonButtonBase::NativePreConstruct()
{
	Super::NativePreConstruct();

   if(MGamePadIcon)
   	  MGamePadIcon->SetInputAction(TriggeredInputAction);
   	
	
}

void UPangaeaCommonButtonBase::SetButtonName(const FString& ButtonName)
{
	const FText ButtonText{FText::FromString(ButtonName)};
	if(MButtonName)
		MButtonName->SetText(ButtonText);
}


