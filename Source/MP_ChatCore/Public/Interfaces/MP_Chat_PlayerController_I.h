// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "MP_Chat_PlayerController_I.generated.h"

UINTERFACE(MinimalAPI)
class UMP_Chat_PlayerController_I : public UInterface
{
    GENERATED_BODY()
};

/**
 * Interface that lets a PlayerController define its chat identity:
 * - Sender ID (persistent unique ID like GUID string)
 * - Display name
 * - Optional tag (e.g., @mention or role)
 */
class MP_CHATCORE_API IMP_Chat_PlayerController_I
{
    GENERATED_BODY()

public:
    /** Returns a unique sender ID (e.g., "Player-324D1...") */
    UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "MP_ChatCore|IMP_Chat_PlayerController_I")
    FString GetSenderID() const;

    /** Returns a user-friendly name to show in chat (e.g., "Yuvi") */
    UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "MP_ChatCore|IMP_Chat_PlayerController_I")
    FString GetDisplayName() const;

    /** Optional: Get a tag string like @admin or [DEV] */
    UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "MP_ChatCore|IMP_Chat_PlayerController_I")
    FString GetSenderTag() const;
};
