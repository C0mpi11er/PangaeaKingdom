// Fill out your copyright notice in the Description page of Project Settings.


#include "Interactable/InteractableObject.h"
#include "Components/BoxComponent.h"

// Sets default values
AInteractableObject::AInteractableObject()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	MInteractableMesh=CreateDefaultSubobject<UStaticMeshComponent>(TEXT("InteractableMesh"));
	MInteractableMesh->SetupAttachment(RootComponent);
	
	MCollisionBoxComponent=CreateDefaultSubobject<UBoxComponent>(TEXT("CollisionBox"));
	MCollisionBoxComponent->SetupAttachment(MInteractableMesh);

}

// Called when the game starts or when spawned
void AInteractableObject::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AInteractableObject::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

