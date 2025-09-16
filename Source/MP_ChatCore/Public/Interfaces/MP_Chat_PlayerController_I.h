// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Data/MP_ChatData.h"
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

	/** Toggles the chat UI visibility */
    UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "MP_ChatCore|IMP_Chat_PlayerController_I")
    void ToggleChat();

	/** Joins or creates a chat room with the given ID and name.
	 * If the room already exists, it will join it; otherwise, it will create a new one.
	 * This is useful for quick access to rooms by ID.
	 */
    UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "MP_ChatCore|IMP_Chat_PlayerController_I")
    void JoinOrCreateChatRoom(const FString& RoomID, const FString& RoomName);

	/** Creates a new chat room with the given description.
	 * This is useful for creating rooms with specific settings or metadata.
	 */
    UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "MP_ChatCore|IMP_Chat_PlayerController_I")
    void CreateChatRoom(FMP_ChatRoomDesc RoomDesc);

	/** Joins an existing chat room by ID.
	 * This is useful for joining rooms that have already been created.
	 */
    UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "MP_ChatCore|IMP_Chat_PlayerController_I")
    void JoinChatRoom(const FString& RoomID);

	/** Leaves a chat room by ID.**/
    UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "MP_ChatCore|IMP_Chat_PlayerController_I")
    void LeaveChatRoom(const FString& RoomID);

	/** Sends a chat message to the specified room.**/
    UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "MP_ChatCore|IMP_Chat_PlayerController_I")
    void SendChatMessage(const FString& RoomID, const FString& MessageText);


};
