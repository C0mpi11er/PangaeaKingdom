// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "PangaeaKingdom/PangaeaKingdom.h"
#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "PangaeaPlayerController.generated.h"

/**
 * 
 */


DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FGameKeySwitch,EGameInputType,GameInput);

UCLASS()
class PANGAEAKINGDOM_API APangaeaPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	FGameKeySwitch M_GameKeySwitchDelegate;
	
protected:
	virtual void BeginPlay() override;

	/*TODO:on key detect events set the game input type
	 *
	 */
	UFUNCTION(BlueprintCallable)
	void CallKeySwitchDelegate(EGameInputType GameInputType);
private:
	//the pangaea game mode pointer
	UPROPERTY()
	TObjectPtr<class APangaeaKingdomGameModeBase> MPangaeaGameMode;

	//the bp class of the game ui
	UPROPERTY(EditAnywhere)
	TSubclassOf<class UPangaeaGameUI> MPangaeaGameUIClass;

	//pointer to the GameUI
	UPROPERTY()
	TObjectPtr<UPangaeaGameUI> MPangaeaGameUIPtr;

	//the bp class to the pangaea build bp class
	UPROPERTY(EditAnywhere)
	TSubclassOf<class UPangaeaUIBuild> MPangaeaBuildUIClass;

	UPROPERTY(EditAnywhere)
	TSubclassOf<class UPangaeaConfirmBuildUI>MPangaeaConfirmBuildClass;
    UPROPERTY()
	EGameInputType MInputType{EGameInputType::EGT_Mouse};
	
	
public:
	FORCEINLINE UPangaeaGameUI* GetPangaeaGameUI() const { return MPangaeaGameUIPtr; }
	FORCEINLINE void SetPangaeaGameUI(UPangaeaGameUI* PangaeaGameUI) { MPangaeaGameUIPtr = PangaeaGameUI; }
	FORCEINLINE TSubclassOf<UPangaeaGameUI> GetPangaeaUIClass() const { return MPangaeaGameUIClass; }
	FORCEINLINE TSubclassOf<UPangaeaUIBuild>GetPangaeaBuildUIClass()const {return MPangaeaBuildUIClass;}
	FORCEINLINE TSubclassOf<UPangaeaConfirmBuildUI>GetPangaeConfirmBuildClass()const {return MPangaeaConfirmBuildClass;}
};
