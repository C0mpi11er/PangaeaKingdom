// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "UObject/ObjectPtr.h"
#include "CoreMinimal.h"
#include "CommonButtonBase.h"
#include "PangaeaCommonButtonBase.generated.h"

/**
 * 
 */
UCLASS()
class PANGAEAKINGDOM_API UPangaeaCommonButtonBase : public UCommonButtonBase
{
	GENERATED_BODY()

protected:
	virtual void NativePreConstruct() override;
	UPROPERTY(BlueprintReadWrite, meta=(BindWidget), meta=(AllowPrivateAccess="true"))
	TObjectPtr<class UTextBlock> MButtonName;
private:
	
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<class UCommonActionWidget> MGamePadIcon;

public:
	void SetButtonName(const FString&ButtonName);
};
