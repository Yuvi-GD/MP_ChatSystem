// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MP_ChatChannelTypes.generated.h"

/**
 * EMP_ChatChannel
 * Defines the standard channel types for MP_ChatSystem Core.
 */

UENUM(BlueprintType)
enum class EMP_ChatChannel : uint8
{
    Global  UMETA(DisplayName = "Global"),
    Team    UMETA(DisplayName = "Team"),
    Party   UMETA(DisplayName = "Party"),
    Private UMETA(DisplayName = "Private"),
    System  UMETA(DisplayName = "System"),
    Admin   UMETA(DisplayName = "Admin")
};
