// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Network/MP_ChatFastArray.h"
#include "MP_ChatRoom.generated.h"

/** Delegate fired on clients when a new chat message arrives in this room */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(
    FOnRoomMessageReceived,
    const FString&, RoomID,
    const FMP_ChatMessage&, NewMsg
);

/**
 * AMP_ChatRoom
 *
 * Invisible replicated actor that holds one FastArray of chat messages for a single room.
 * - RoomID              (FString) replicated so client knows which room this is.
 * - Messages            (FMP_ChatMsgArray) FastArray of FMP_ChatMessage items.
 * - OnRep_Messages      fires when new items are received, broadcasting via OnMessageReceived.
 * - LastReadIndex       (int32) local index so only new items get broadcast.
 * - InitializeRoom(...) server RPC to set the RoomID on spawn.
 */

UCLASS(meta = (DisplayName = "ChatRoom"))
class MP_CHATCORE_API AMP_ChatRoom : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AMP_ChatRoom();

    /** Room config/metadata (set once on spawn) */
    UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly, Category = "MP_ChatCore|MP_ChatRoom")
    FMP_ChatRoomDesc RoomDesc;

    /** Replicated array of messages (FastArray) */
    UPROPERTY(ReplicatedUsing = OnRep_Messages)
    FMP_ChatMsgArray MsgArray;

    /** Set of joined player IDs (not replicated; only for server relevancy logic) */
    TSet<FString> SubscribedPlayerIDs;

    /** Dispatcher for message received (for local UI/systems) */
    UPROPERTY(BlueprintAssignable, Category = "MP_ChatCore|MP_ChatRoom")
    FOnRoomMessageReceived OnMessageReceived;

    // --- Core Chat Operations ---

    /** Adds a player to the room (by unique ID). Triggers net update for relevancy. */
    UFUNCTION(BlueprintCallable, Category = "MP_ChatCore|MP_ChatRoom")
    bool AddSubscriber(const FString& PlayerID);

    /** Removes a player from the room. Triggers net update; destroys self if room is empty & non-persistent. */
    UFUNCTION(BlueprintCallable, Category = "MP_ChatCore|MP_ChatRoom")
    bool RemoveSubscriber(const FString& PlayerID);

    /** Adds a chat message. Trims old messages, marks FastArray dirty, triggers dispatcher, net update. */
    UFUNCTION(BlueprintCallable, Category = "MP_ChatCore|MP_ChatRoom")
    void AddChatMessage(const FMP_ChatMessage& NewMessage);

    /** Edit an existing message (by array index). Sets 'edited' flag, updates time/text. */
    UFUNCTION(BlueprintCallable, Category = "MP_ChatCore|MP_ChatRoom")
    bool EditChatMessage(int32 MsgIndex, const FString& NewText);

    /** Deletes a message (by array index). Hard delete. */
    UFUNCTION(BlueprintCallable, Category = "MP_ChatCore|MP_ChatRoom")
    bool DeleteChatMessage(int32 MsgIndex);

    /** Called by ChatManager on the server to assign this actor’s RoomID */
    UFUNCTION(BlueprintPure, BlueprintCallable, Category = "MP_ChatCore|MP_ChatRoom")
    TArray<FMP_ChatMessage> GetRoomChatMessage();

protected:

    // --- Replication ---

    virtual void BeginPlay() override;

    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

    /** Custom relevancy: replicate only to joined players (except for global/team rooms). */
    virtual bool IsNetRelevantFor(const AActor* RealViewer, const AActor* Viewer, const FVector& SrcLocation) const override;

    /** Called when Messages (the FastArray) is replicated to clients. */
    UFUNCTION()
    void OnRep_Messages();

};
