// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "UObject/ObjectPtr.h"
#include "CoreMinimal.h"
#include "CommonUI/PangaeaCommonButtonBase.h"
#include "BuildItemButton.generated.h"

/**
 * 
 */

USTRUCT(BlueprintType)
struct FResource
{
	GENERATED_BODY()
	UPROPERTY(EditAnywhere)
	FName MTitle{};

	UPROPERTY(EditAnywhere)
	TSubclassOf<class AInteractableObject>MInteractableObjectClass;
	
};
UCLASS()
class PANGAEAKINGDOM_API UBuildItemButton : public UPangaeaCommonButtonBase
{
	GENERATED_BODY()

protected:
	virtual void NativePreConstruct() override;
     virtual void NativeOnClicked() override;


private:
	UPROPERTY(EditAnywhere)
	FResource MResourceType;
	
	TSubclassOf<class AInteractableObject>MInteractableClassRefClass;

	UPROPERTY()
	TObjectPtr<class APangaeaPlayerController>MPangaeaPlayerController;
public:
	void SetItemName() const;
	void SetInteractableClass();
};
