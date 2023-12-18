// Fill out your copyright notice in the Description page of Project Settings.

//this class is being given instructions to carry out  by the pangaea pawn
#pragma once


#include "UObject/ObjectPtr.h"
#include "Interfaces//VillagerInterface.h"
#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "GameFramework/Pawn.h"
#include "PangaeaVillager.generated.h"


USTRUCT(BlueprintType)
struct FVillagerJobs:public FTableRowBase
{
	GENERATED_BODY()

	//to play the montage of the specified job
	UPROPERTY(EditAnywhere)
	TObjectPtr<class UAnimMontage>MJobMontage;

	UPROPERTY(EditAnywhere)
	TObjectPtr<class UStaticMesh>MJobTool;

	UPROPERTY(EditAnywhere)
	TObjectPtr<class UBehaviorTree>MJobBehaviourTree;
	
};


UCLASS()
class PANGAEAKINGDOM_API APangaeaVillager : public APawn,public IVillagerInterface 
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	APangaeaVillager();

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	//called from the pangaea pawn and tells the villager what to do
	virtual void VillagerAction(AActor* ActionToPerform) override;
	//to assign the villager a new job,passed from the action event which is a triggered interface from the player
     virtual void ChangeVillagerJob(const FName& NewJob) override;
     //to reset the job stat of the villager
	void ResetVillagerJob();
private:
	//store the name of the new job te villager is meant o carry out
	FName MNewJob{};
	//contains all the jobs and data for the villager
	UPROPERTY(EditAnywhere)
	TObjectPtr<class UDataTable> MDataTable_Jobs;
	UPROPERTY(EditAnywhere)
	TObjectPtr<class UCapsuleComponent> MCapsuleComponent;
	UPROPERTY(EditAnywhere)
	TObjectPtr<USkeletalMeshComponent> MVillagerMesh;
	//to render the shadow of the villager
	UPROPERTY(EditAnywhere)
	TObjectPtr<class UDecalComponent>MShadowDecal;

	
    	

	


	

};
