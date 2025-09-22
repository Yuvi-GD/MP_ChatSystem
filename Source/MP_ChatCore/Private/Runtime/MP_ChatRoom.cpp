// Fill out your copyright notice in the Description page of Project Settings.
// Path: Plugins/MP_ChatSystem/Source/MP_ChatCore/Private/Runtime/AMP_ChatRoom.cpp

#include "Runtime/MP_ChatRoom.h"
#include "Net/UnrealNetwork.h"
#include "Interfaces/MP_Chat_PlayerController_I.h"

AMP_ChatRoom::AMP_ChatRoom()
{
    // Enable replication
    bReplicates = true;

    // Ensure this actor is always sent to all clients
    bAlwaysRelevant = false;

    // Ensure actor is loaded on clients when joining late
    //bNetLoadOnClient = true;

    // Tick not needed for chat room logic
    PrimaryActorTick.bCanEverTick = false;

    // Prevent movement replication (optional)
    SetReplicateMovement(false);

#if ENGINE_MAJOR_VERSION == 5 && ENGINE_MINOR_VERSION < 4
    NetUpdateFrequency = 0.01f; // Very low; ForceNetUpdate used for instant updates
#else
    SetNetUpdateFrequency(0.01f); // UE 5.4+ API
#endif

    // Initialize root
    USceneComponent* Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
    SetRootComponent(Root);

}

void AMP_ChatRoom::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    // Replicate RoomID to everyone
    DOREPLIFETIME(AMP_ChatRoom, RoomDesc);

    // Replicate Messages FastArray
    DOREPLIFETIME(AMP_ChatRoom, MsgArray);
}

void AMP_ChatRoom::BeginPlay()
{
    Super::BeginPlay();
    if (!HasAuthority())
    {
        UE_LOG(LogTemp, Warning, TEXT("CLIENT has room: %s | Replicated: %d"), *RoomDesc.RoomID, GetIsReplicated());
    }

    UE_LOG(LogTemp, Warning, TEXT("Room Pointer: %p | Authority: %d"), this, HasAuthority());
    // Ensure FastArray owns this actor on both server and clients
    //Messages.SetOwningActor(this);
}

//void AMP_ChatRoom::Tick(float DeltaTime)
//{
//    Super::Tick(DeltaTime);
//
//    if (HasAuthority())
//    {
//        FString Log = FString::Printf(TEXT("Room %s | Replicated: %d | Items: %d"),
//            *RoomID, GetIsReplicated(), Messages.Items.Num());
//        UE_LOG(LogTemp, Warning, TEXT("MP_ChatRoom::ServerTick: %s"), *Log);
//    }
//    else
//    {
//        UE_LOG(LogTemp, Warning, TEXT("MP_ChatRoom::ClientTick: %s | OnRep Not Called Yet"), *RoomID);
//    }
//}

bool AMP_ChatRoom::IsNetRelevantFor(const AActor* RealViewer, const AActor* Viewer, const FVector& SrcLocation) const
{
	UE_LOG(LogTemp, Warning, TEXT("MP_ChatRoom::IsNetRelevantFor check: RoomID = %s"), *RoomDesc.RoomID);

    if (RealViewer && RealViewer->GetClass()->ImplementsInterface(UMP_Chat_PlayerController_I::StaticClass()))
    {
        UE_LOG(LogTemp, Warning, TEXT("MP_ChatRoom::IsNetRelevantFor running PC Interface"));
        FString PlayerID = IMP_Chat_PlayerController_I::Execute_GetSenderID(RealViewer);
        UE_LOG(LogTemp, Warning, TEXT("MP_ChatRoom::IsNetRelevantFor check: %s for %s"), *RoomDesc.RoomID, *PlayerID);
        return SubscribedPlayerIDs.Contains(PlayerID);
    }
    return false;
}

void AMP_ChatRoom::OnRep_Messages()
{
    UE_LOG(LogTemp, Warning, TEXT("MP_ChatRoom::OnRep_Messages() TRIGGERED! Item count = %d"), MsgArray.Messages.Max());

    // Called on client when messages are updated
    if (MsgArray.Messages.Num() > 0)
    {
        const FMP_ChatMessage& LastMsg = MsgArray.Messages.Last();
        OnMessageReceived.Broadcast(RoomDesc.RoomID, LastMsg);
    }

    //// Called on clients whenever the Messages FastArray receives new items
    //int32 CurrentNum = Messages.Items.Num();
    //if (CurrentNum >= LastReadIndex)
    //{
    //    // Broadcast each newly added message
    //    for (int32 i = LastReadIndex; i < CurrentNum; ++i)
    //    {
    //        const FMP_ChatMessage& NewMsg = Messages.Items[i].Message;
    //        UE_LOG(LogTemp, Warning, TEXT("MP_ChatRoom:: Broadcasting NewMsg: %s"), *NewMsg.Body);

    //        if (OnMessageReceived.IsBound())
    //        {
    //            OnMessageReceived.Broadcast(RoomID, NewMsg);
    //        }
    //    }
    //    LastReadIndex = CurrentNum;
    //}
    UE_LOG(LogTemp, Warning, TEXT("MP_ChatRoom:: OnRep fired. Total: LastReadIndex =%d"), MsgArray.Messages.Num());
}

bool AMP_ChatRoom::AddSubscriber(const FString& PlayerID)
{
    if (PlayerID.IsEmpty() || SubscribedPlayerIDs.Contains(PlayerID))
    {
        UE_LOG(LogTemp, Warning, TEXT("MP_ChatRoom::AddSubscriber: Invalid PlayerID or already subscribed: %s"), *PlayerID);
        return false;
    }

    SubscribedPlayerIDs.Add(PlayerID);
    ForceNetUpdate();
    return true;
}

bool AMP_ChatRoom::RemoveSubscriber(const FString& PlayerID)
{
    if (SubscribedPlayerIDs.Remove(PlayerID))
    {
        ForceNetUpdate();
        // Destroy/dormant logic handled by manager/outside this class for now
        return true;
    }
    return false;
}

void AMP_ChatRoom::AddChatMessage(const FMP_ChatMessage& NewMsg)
{
    if (!HasAuthority()) return;


    UE_LOG(LogTemp, Warning, TEXT("MP_ChatRoom::AddChatMessage() Server added message: %s"), *NewMsg.MessageText);

    MsgArray.AddMessage(NewMsg, RoomDesc.MaxMessages);
    ForceNetUpdate();
    OnMessageReceived.Broadcast(RoomDesc.RoomID, NewMsg);
}

bool AMP_ChatRoom::EditChatMessage(int32 MsgIndex, const FString& NewText)
{
    bool bEdited = MsgArray.EditMessage(MsgIndex, NewText);
    if (bEdited)
    {
        ForceNetUpdate();
        if (MsgArray.Messages.IsValidIndex(MsgIndex))
        {
            OnMessageReceived.Broadcast(RoomDesc.RoomID, MsgArray.Messages[MsgIndex]);
        }
    }
    return bEdited;
}

bool AMP_ChatRoom::DeleteChatMessage(int32 MsgIndex)
{
    bool bDeleted = MsgArray.DeleteMessage(MsgIndex);
    if (bDeleted)
    {
        ForceNetUpdate();
    }
    return bDeleted;
}

TArray<FMP_ChatMessage> AMP_ChatRoom::GetRoomChatMessage()
{
	TArray<FMP_ChatMessage> Messages;
	for (const FMP_ChatMessage& Msg : MsgArray.Messages)
	{
	    Messages.Add(Msg);
	}
	if (Messages.Num() > 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("MP_ChatRoom::GetRoomChatMessage() - RoomID: %s, Messages Count: %d"), *RoomDesc.RoomID, Messages.Num());
		return Messages;
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("MP_ChatRoom::GetRoomChatMessage() - RoomID: %s has no messages."), *RoomDesc.RoomID);
		return TArray<FMP_ChatMessage>();
	}
}
