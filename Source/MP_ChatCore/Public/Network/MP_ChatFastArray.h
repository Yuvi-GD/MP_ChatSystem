// Fill out your copyright notice in the Description page of Project Settings.
// MP_ChatSystem/Source/MP_ChatCore/Public/Network/MP_ChatFastArray.h
#pragma once

#include "CoreMinimal.h"
#include "Data/MP_ChatData.h"
//#include "UObject/NoExportTypes.h"
//#include "Engine/NetSerialization.h"
#include "Net/Serialization/FastArraySerializer.h"
#include "MP_ChatFastArray.generated.h"

class AMP_ChatRoom;

/**
 * Item wrapper for a single chat message in a FastArray.
 * Inherit from FFastArraySerializerItem so that Unreal can track when
 * each item is added/removed/changed.
 */

//USTRUCT()
//struct FMP_ChatMsgItem : public FFastArraySerializerItem
//{
//    GENERATED_BODY()
//
//    /** The actual chat message payload (sender ID, body, timestamp, etc.). */
//    UPROPERTY()
//    FMP_ChatMessage Message;
//};

/**
 * FastArray that holds all messages for a single chat room.
 * This struct tells Unreal: “I have an array of FMP_ChatMsgItem; replicate
 * it efficiently by sending only changes.”
 */
USTRUCT()
struct FMP_ChatMsgArray : public FFastArraySerializer
{
    GENERATED_BODY()

    /** All chat messages in this room. Unreal will replicate only the added/
     *  removed/changed items in this array. */
    UPROPERTY()
    TArray<FMP_ChatMessage> Messages;

    // Add a new message and mark dirty. Trims to MaxMessages if needed.
    void AddMessage(const FMP_ChatMessage& NewMsg, int32 MaxMessages);

    // Remove oldest messages if array is too long.
    void TrimToRecent(int32 MaxMessages);

    // Edit a message at index (body/time/flags)
    bool EditMessage(int32 MsgIndex, const FString& NewText);

    // Remove message at index (hard delete)
    bool DeleteMessage(int32 MsgIndex);

    /** Pointer to the owning actor (AMP_ChatRoom) that actually manages this array.
     *  When you add a new item, call:
     *      ChatMsgArray.SetOwningActor(this);
     *      ChatMsgArray.AddMessage(...)
     *  so Unreal knows which actor “owns” the dirty state.
     */
    //UPROPERTY()
    //AMP_ChatRoom* OwningActor = nullptr;

    ///** Tell the FastArray which actor owns it, so that Wake and replication know where
    // *  to route notifications. Must be called on the server before calling AddMessage.
    // */
    //FORCEINLINE void SetOwningActor(AMP_ChatRoom* InOwner)
    //{
    //    OwningActor = InOwner;
    //}

    ///**
    // * Convenience function: call this whenever you add a new FMP_ChatMsgItem
    // * to Items[]. Unreal will mark that item “dirty,” which triggers replication.
    // *
    // * @param NewMsg  - The chat message struct (sender, body, timestamp, etc.)
    // */
    //void AddMessage(const FMP_ChatMessage& NewMsg)
    //{
    //    FMP_ChatMsgItem NewItem;
    //    NewItem.Message = NewMsg;

    //    // Add the new item and mark it dirty so replication knows it changed
    //    Items.Add(NewItem);
    //    MarkItemDirty(NewItem);
    //}

	/** This is required for FastArray serialization to work correctly. */
    bool NetDeltaSerialize(FNetDeltaSerializeInfo& DeltaParms)
    {
        return FastArrayDeltaSerialize(Messages, DeltaParms, *this);
    }
};
template<>
struct TStructOpsTypeTraits<FMP_ChatMsgArray> : public TStructOpsTypeTraitsBase2<FMP_ChatMsgArray>
{
    enum { WithNetDeltaSerializer = true };
};