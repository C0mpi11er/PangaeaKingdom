// Fill out your copyright notice in the Description page of Project Settings.


#include "PangaeaVillager/PangaeaVillager.h"
#include "Components/CapsuleComponent.h"
#include "Components/DecalComponent.h"
#include "Kismet/KismetArrayLibrary.h"

// Sets default values
APangaeaVillager::APangaeaVillager()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	MCapsuleComponent=CreateDefaultSubobject<UCapsuleComponent>(TEXT("CapsuleComponent"));
	MCapsuleComponent->SetupAttachment(RootComponent);
	MVillagerMesh=CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("VillagerMesh"));
	MVillagerMesh->SetupAttachment(MCapsuleComponent);
	MShadowDecal=CreateDefaultSubobject<UDecalComponent>(TEXT("ShadowDecal"));
	MShadowDecal->SetupAttachment(MVillagerMesh);

}

// Called when the game starts or when spawned
void APangaeaVillager::BeginPlay()
{
	Super::BeginPlay();
	
}

void APangaeaVillager::VillagerAction(AActor* ActionToPerform)
{
	if(!ActionToPerform)return;
	IVillagerInterface::VillagerAction(ActionToPerform);
	UE_LOG(LogTemp,Warning,TEXT("villagerActionname:%s"),*ActionToPerform->GetName())
	if(ActionToPerform->Tags.IsValidIndex(0))
	{
		
		const FName JobToPerform{ActionToPerform->Tags[0]};
		UE_LOG(LogTemp,Warning,TEXT("JobName:%s"),*JobToPerform.ToString())
		ChangeVillagerJob(JobToPerform);
		/*TODO:find out why the job name is not showing on the log*/
		 /*TODO:update all villagers*/
	}
}

void APangaeaVillager::ChangeVillagerJob(const FName& NewJob)
{
	IVillagerInterface::ChangeVillagerJob(NewJob);
	MNewJob=NewJob;
	if(MDataTable_Jobs)
	{
		//to get the job type from the villager data table
		const auto VillagerJobRow{MDataTable_Jobs->FindRow<FVillagerJobs>(MNewJob,TEXT(""))};
		if(VillagerJobRow)
		{
			//to be sure the villager is not already assigned the job particular job
			if(Tags.Find(MNewJob))
			{
				// do nothing
			}
			else
			{
				//make job the first index in tag
				Tags[0]=MNewJob;
			}
			
		}
		else
		{
			FString DebugText{FString::Printf(TEXT("failed to load job"))};
			if(GetWorld()&&GEngine)
				GEngine->AddOnScreenDebugMessage(1,5.f,FColor::Orange,DebugText);
		}
	}
		
	
	
}

void APangaeaVillager::ResetVillagerJob()
{
	/*stop the job and remove all necessary accessories init*/
	/*TODO:StopJob*/
}

// Called every frame
void APangaeaVillager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void APangaeaVillager::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

