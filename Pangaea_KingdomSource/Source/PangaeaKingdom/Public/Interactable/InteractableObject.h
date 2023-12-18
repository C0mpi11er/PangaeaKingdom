// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "UObject/ObjectPtr.h"
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "InteractableObject.generated.h"

UCLASS()
class PANGAEAKINGDOM_API AInteractableObject : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AInteractableObject();
   // Called every frame
   	virtual void Tick(float DeltaTime) override;
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

private:
	UPROPERTY(EditAnywhere)
	TObjectPtr<class UStaticMeshComponent>MInteractableMesh;

	//use to determine if nave mesh is overlapping with other actors or not
	UPROPERTY(EditAnywhere)
	TObjectPtr<class UBoxComponent>MCollisionBoxComponent;
public:
	FORCEINLINE UStaticMeshComponent*GetMesh()const{return  MInteractableMesh;}
     FORCEINLINE UBoxComponent*GetCollisionBox()const{return MCollisionBoxComponent;}
};
