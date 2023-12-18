// Fill out your copyright notice in the Description page of Project Settings.
/*this is the interface that activates wehen the build button is pressed*/
#pragma once

#include "UObject/ObjectPtr.h"
#include "CoreMinimal.h"
#include "CommonActivatableWidget.h"
#include "PangaeaUIBuild.generated.h"

/**
 * 
 */
UCLASS()
class PANGAEAKINGDOM_API UPangaeaUIBuild : public UCommonActivatableWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeOnActivated() override;
    virtual void NativePreConstruct() override;
	UFUNCTION()
	void OnBackButtonPressed();
private:
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<class UBuildItemButton>MBuildItemOne;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<class UPangaeaCommonButtonBase>MBackButton;
	 
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UBuildItemButton>MBuildItemTwo;

	UPROPERTY()
	TObjectPtr<class APangaeaPlayerController>MPangaeaPlayerController;

	UPROPERTY()
	TObjectPtr<class APangaeaPawn>MPawn;
};
