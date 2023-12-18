// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "UObject/ObjectPtr.h"
#include "CoreMinimal.h"
#include "CommonActivatableWidget.h"
#include "PangaeaConfirmBuildUI.generated.h"

/**
 * 
 */
UCLASS()
class PANGAEAKINGDOM_API UPangaeaConfirmBuildUI : public UCommonActivatableWidget
{
	GENERATED_BODY()


protected:
	virtual void NativeOnActivated() override;

	void UIAutoGenFunction();

	UFUNCTION()
	void OnConfirmButtonClicked();

	UFUNCTION()
	void OnRotateButtonClicked();

	UFUNCTION()
	void OnCancelBuildButtonClicked();
private:
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<class UPangaeaCommonButtonBase>MConfirmBuildButton;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<class UPangaeaCommonButtonBase>MRotateBuildButton;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<class UPangaeaCommonButtonBase>MCancelBuildButton;

	UPROPERTY()
	TObjectPtr<class APangaeaPlayerController>MPangaeaPlayerController;
};
