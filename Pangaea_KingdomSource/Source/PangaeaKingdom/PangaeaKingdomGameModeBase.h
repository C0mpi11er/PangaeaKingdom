// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "PangaeaPawn/Interfaces/PangaeaPawnInterface.h"
#include "UObject/ObjectPtr.h"
#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "PangaeaKingdomGameModeBase.generated.h"

/**
 * 
 */
UCLASS()
class PANGAEAKINGDOM_API APangaeaKingdomGameModeBase : public AGameModeBase,public IPangaeaPawnInterface
{
	GENERATED_BODY()
public:
	void IntializeUI();
    virtual void AddUserInterface(TSubclassOf<UCommonActivatableWidget> NewUIClass) override;
	
protected:
	virtual void BeginPlay() override;

private:
	
	UPROPERTY()
	TObjectPtr<class APangaeaPlayerController>MPangaeaPlayerController;

	//the current ui hud
	UPROPERTY()
	TObjectPtr<class UCommonActivatableWidget>MUserInterfaceHUD;

public:
	FORCEINLINE void SetPangaeaPlayerController(APangaeaPlayerController*PangaeaPlayerController)
	{
		MPangaeaPlayerController=PangaeaPlayerController;
		
	}
};
