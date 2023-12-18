// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "UObject/ObjectPtr.h"
#include "CoreMinimal.h"
#include "CommonActivatableWidget.h"
#include "CommonButtonBase.h"
#include "PangaeaGameUI.generated.h"


/**
 * 
 */
UCLASS()
class PANGAEAKINGDOM_API UPangaeaGameUI : public UCommonActivatableWidget
{
	GENERATED_BODY()

public:
	void AddStackItem(TSubclassOf<UCommonActivatableWidget> WidgetToAdd);
     
protected:
	virtual void NativeOnActivated() override;
     

	FCommonButtonBaseClicked ButtonBaseClicked;


	void UIAutoGenFunction();

	UFUNCTION()
	void OnBuildButtonClick();
	
    
	
private:
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<class UPangaeaCommonButtonBase> MBuildButton;

	UPROPERTY()
	TObjectPtr<class APangaeaPlayerController> MPangaeaPlayerController;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<class UCommonActivatableWidgetStack> MMainStack;
	
};
