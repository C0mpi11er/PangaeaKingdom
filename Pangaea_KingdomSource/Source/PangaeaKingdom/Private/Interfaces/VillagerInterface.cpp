// Fill out your copyright notice in the Description page of Project Settings.


#include "Interfaces/VillagerInterface.h"

// Add default functionality here for any IVillagerInterface functions that are not pure virtual.
void IVillagerInterface::VillagerAction(AActor* ActionToPerform)
{
	UE_LOG(LogTemp,Warning,TEXT("villager action interface class called"))
}


void IVillagerInterface::ChangeVillagerJob(const FName& NewJob)
{
	 UE_LOG( LogTemp,Warning,TEXT("changed villager job"))
}
