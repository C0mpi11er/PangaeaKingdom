// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

//the type of input
UENUM(Blueprintable)
enum class EGameInputType:uint8
{
	EGT_Mouse UMETA(DisplayName="Mouse"),
	EGT_Gamepad UMETA(DisplayName="GamePad"),
	EGT_Unknown UMETA(DisplayName="Unknown"),
	EGT_Max
};