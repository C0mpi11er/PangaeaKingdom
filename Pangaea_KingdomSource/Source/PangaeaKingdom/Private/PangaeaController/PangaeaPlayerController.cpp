// Fill out your copyright notice in the Description page of Project Settings.


#include "PangaeaController/PangaeaPlayerController.h"

void APangaeaPlayerController::BeginPlay()
{
	Super::BeginPlay();
	
}

void APangaeaPlayerController::CallKeySwitchDelegate(EGameInputType GameInputType)
{
	//this function broad casts the change of input type
	switch (GameInputType)
	{
	case EGameInputType::EGT_Gamepad: MInputType= EGameInputType::EGT_Gamepad;break;
	case EGameInputType::EGT_Mouse: MInputType= EGameInputType::EGT_Mouse;break;
	case EGameInputType::EGT_Unknown: MInputType= EGameInputType::EGT_Unknown;break;
		default:break;
	}
	M_GameKeySwitchDelegate.Broadcast(GameInputType);
}
