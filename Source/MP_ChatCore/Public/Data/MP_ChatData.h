#pragma once

#include "CoreMinimal.h"
#include "Net/Serialization/FastArraySerializer.h"
#include "Data/MP_ChatChannelTypes.h"
#include "MP_ChatData.generated.h"

/**
 * FMP_ChatRoomDesc
 * Describes a chat room, including its ID, name, channel type, and settings.
 * This is used to create or configure chat rooms in the system.
 */
USTRUCT(BlueprintType)
struct FMP_ChatRoomDesc
{
    GENERATED_BODY()

    /** The key for this room (ID + channel). */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MP_ChatCore|ChatRoomDesc")
    FString RoomID;

	// Room Name (display name for the room)
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MP_ChatCore|ChatRoomDesc")
    FString RoomName;
    
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "MP_ChatCore|Room")
    FString CreatorPlayerID; // Who created the room

	// Channel type for this room, e.g. Global, Team, Party, etc.
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MP_ChatCore|ChatRoomDesc")
    EMP_ChatChannel ChannelType = EMP_ChatChannel::Global;

    // If true, this room’s history should be persisted (local or server).
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MP_ChatCore|ChatRoomDesc")
    bool bPersistent = false;

	// If true, only admins can send messages in this room.
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MP_ChatCore|ChatRoomDesc")
    bool bAdminOnly = false;

	 // Maximum number of messages to keep in this room (0 means unlimited).
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MP_ChatCore|ChatRoomDesc")
    int32 MaxMessages = 0;

    // Default constructor
    FMP_ChatRoomDesc()
        : RoomID(TEXT("")), RoomName(TEXT("")),
        ChannelType(EMP_ChatChannel::Global),
        bPersistent(false),
        bAdminOnly(false),
        MaxMessages(0) 
    {
    }
};

/**
 * FMP_ChatMessage
 * Single chat message. Inherits FastArray item for efficient network delta.
 */
USTRUCT(BlueprintType)
struct FMP_ChatMessage : public FFastArraySerializerItem
{
    GENERATED_BODY()

    /** Who sent this message (PlayerID as string). */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MP_ChatCore|ChatMessage")
    FString SenderID;

    /** Display name of the sender. */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MP_ChatCore|ChatMessage")
    FString SenderName;

    /** The text body of the message. */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MP_ChatCore|ChatMessage")
    FString MessageText;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "MP_ChatCore|Message")
    float Timestamp = 0.0;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "MP_ChatCore|Message")
    uint8 MessageFlags = 0;

    FMP_ChatMessage()
        : SenderID(TEXT(""))
        , SenderName(TEXT(""))
        , MessageText(TEXT(""))
        , Timestamp(0.0)
        , MessageFlags(0)
    {
    }

	// NetSerialize for efficient network replication for FastArray items.
    bool NetSerialize(FArchive& Ar, class UPackageMap* Map, bool& bOutSuccess);
    
	// Custom equality check for FastArray items.
    bool Identical(const FMP_ChatMessage* Other, uint32 PortFlags) const
    {
        if (!Other) return false;
        return
            SenderID == Other->SenderID &&
            SenderName == Other->SenderName &&
            MessageText == Other->MessageText &&
            Timestamp == Other->Timestamp &&
            MessageFlags == Other->MessageFlags;
    }

};

// This tells Unreal that FMP_ChatMessage has a custom NetSerialize function.
template<>
struct TStructOpsTypeTraits<FMP_ChatMessage> : public TStructOpsTypeTraitsBase2<FMP_ChatMessage>
{
    enum
    {
        WithNetSerializer = true,
        WithIdentical = true
    };

};
