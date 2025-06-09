// Path: Plugins/MP_ChatSystem/Source/MP_ChatCore/Private/Runtime/MP_ChatManager.cpp

#include "Runtime/MP_ChatManager.h"
#include "GameFramework/PlayerController.h"
#include "Interfaces/MP_Chat_PlayerController_I.h"
#include "Network/MP_ChatRepGraph.h"
#include "Net/UnrealNetwork.h"
#include "EngineUtils.h"
#include "Engine/World.h"
#include "Engine/NetDriver.h"

AMP_ChatManager::AMP_ChatManager()
{
    bReplicates = false; // Manager does not need to replicate

    SetActorHiddenInGame(true);
    PrimaryActorTick.bCanEverTick = false;
}

void AMP_ChatManager::BeginPlay()
{
    Super::BeginPlay();

    //if (HasAuthority())
    //{
    //    if (UNetDriver* NetDriver = GetNetDriver())
    //    {
    //        CachedRepGraph = Cast<UMP_ChatRepGraph>(NetDriver->GetReplicationDriver());
    //    }

    //    UE_LOG(LogTemp, Warning, TEXT("MP_ChatManager:: BeginPlay. IsAuthority: %d"), HasAuthority());
    //    if (UNetDriver* Driver = GetNetDriver())
    //    {
    //        UReplicationGraph* RawGraph = Cast<UReplicationGraph>(Driver->GetReplicationDriver());
    //        CachedRepGraph = Cast<UMP_ChatRepGraph>(RawGraph);
    //        UE_LOG(LogTemp, Warning, TEXT("MP_ChatManager:: RepGraph assigned: %s"), *GetNameSafe(CachedRepGraph));
    //    }
    //}
}

AMP_ChatRoom* AMP_ChatManager::CreateRoom(const FMP_ChatRoomDesc& RoomDesc, APlayerController* PlayerController)
{
    if (!HasAuthority() || !PlayerController) return nullptr;

    if (RoomDesc.RoomID.IsEmpty())
        return nullptr;

    if (AMP_ChatRoom** FoundPtr = ChatRooms.Find(RoomDesc.RoomID))
        return *FoundPtr;

    FActorSpawnParameters SpawnParams;
    SpawnParams.Owner = this;
    SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
    AMP_ChatRoom* NewRoom = GetWorld()->SpawnActor<AMP_ChatRoom>(AMP_ChatRoom::StaticClass(), FVector::ZeroVector, FRotator::ZeroRotator, SpawnParams);
    if (NewRoom)
    {
        NewRoom->RoomDesc = RoomDesc;

		// Set the room ID Tag for easy identification
        NewRoom->Tags.Add(FName(*RoomDesc.RoomID));

        // Set relevancy
        if (RoomDesc.ChannelType == EMP_ChatChannel::Global ||
            RoomDesc.ChannelType == EMP_ChatChannel::Team ||
            RoomDesc.ChannelType == EMP_ChatChannel::Party ||
            RoomDesc.ChannelType == EMP_ChatChannel::System)
        {
            NewRoom->bAlwaysRelevant = true;
        }
        else
        {
            NewRoom->bAlwaysRelevant = false;
        }
        ChatRooms.Add(RoomDesc.RoomID, NewRoom);

		//Add PlayerID To Subscribers Created Room
        FString PlayerID;
        if (PlayerController && PlayerController->GetClass()->ImplementsInterface(UMP_Chat_PlayerController_I::StaticClass()))
        {
            PlayerID = IMP_Chat_PlayerController_I::Execute_GetSenderID(PlayerController);
			NewRoom->RoomDesc.CreatorPlayerID = PlayerID; // Set creator ID in room description
        }

        if (PlayerID.IsEmpty())
        {
            UE_LOG(LogTemp, Warning, TEXT("MP_ChatManager::JoinRoom: PlayerID is empty for PlayerController %s"), *PlayerController->GetName());
            return nullptr;
        }
    NewRoom->AddSubscriber(PlayerID);
    return NewRoom;
    }
	return nullptr;
}

AMP_ChatRoom* AMP_ChatManager::GetRoom(const FString& RoomID) const
{
    if (RoomID.IsEmpty()) return nullptr;
    AMP_ChatRoom* const* RoomPtr = ChatRooms.Find(RoomID);
    return RoomPtr ? *RoomPtr : nullptr;
}

AMP_ChatRoom* AMP_ChatManager::JoinRoom(const FString& RoomID, APlayerController* PlayerController)
{
    if (!HasAuthority() || !PlayerController) return nullptr;

    AMP_ChatRoom* Room = GetRoom(RoomID);
    if (!Room)
    {
		UE_LOG(LogTemp, Warning, TEXT("MP_ChatManager::JoinRoom: Room not found for ID: %s"), *RoomID);
        return nullptr;
    }
	FString PlayerID;
    if (PlayerController && PlayerController->GetClass()->ImplementsInterface(UMP_Chat_PlayerController_I::StaticClass()))
    {
        PlayerID = IMP_Chat_PlayerController_I::Execute_GetSenderID(PlayerController);
    }
    if (PlayerID.IsEmpty())
    {
		UE_LOG(LogTemp, Warning, TEXT("MP_ChatManager::JoinRoom: PlayerID is empty for PlayerController %s"), *PlayerController->GetName());
        return nullptr;
    }

    bool bJoined = Room->AddSubscriber(PlayerID);
    return bJoined ? Room : nullptr;
}

bool AMP_ChatManager::LeaveRoom(const FString& RoomID, APlayerController* PlayerController)
{
    if (!HasAuthority() || !PlayerController) return false;

    AMP_ChatRoom* Room = GetRoom(RoomID);
    if (!Room) return false;

    FString PlayerID = GetPlayerID(PlayerController);
    if (PlayerID.IsEmpty())
    {
        UE_LOG(LogTemp, Warning, TEXT("MP_ChatManager::LeavRoom: PlayerID is empty for PlayerController %s"), *PlayerController->GetName());
        return false;
    }

    bool bRemoved = Room->RemoveSubscriber(PlayerID);
    if (bRemoved && !Room->RoomDesc.bPersistent && Room->SubscribedPlayerIDs.Num() == 0)
    {
        CleanupRoomIfEmpty(Room);
    }
    return bRemoved;
}

bool AMP_ChatManager::SendMessage(const FString& RoomID, const FString& MessageText, APlayerController* PlayerController)
{
    if (!HasAuthority() || !PlayerController) return false;

    AMP_ChatRoom* Room = GetRoom(RoomID);
    if (!Room) return false;

    FString PlayerID = GetPlayerID(PlayerController);
    if (PlayerID.IsEmpty())
    {
        UE_LOG(LogTemp, Warning, TEXT("MP_ChatManager::SendMessage: PlayerID is empty for PlayerController %s"), *PlayerController->GetName());
        return false;
    }

    // Policy: only subscribed users may post
    if (!Room->SubscribedPlayerIDs.Contains(PlayerID))
    {
		UE_LOG(LogTemp, Warning, TEXT("MP_ChatManager::SendMessage: Player %s not subscribed to room %s"), *PlayerID, *RoomID);
        return false;
    }

    // Admin-only enforcement
    if (Room->RoomDesc.bAdminOnly)
    {
        if (!PlayerController->IsLocalPlayerController()) // Replace this with your real admin check!
            return false;
    }

    FMP_ChatMessage NewMsg;
    if (PlayerController && PlayerController->GetClass()->ImplementsInterface(UMP_Chat_PlayerController_I::StaticClass()))
    {
        NewMsg.SenderName =IMP_Chat_PlayerController_I::Execute_GetDisplayName(PlayerController);
		UE_LOG(LogTemp, Warning, TEXT("MP_ChatManager::SendMessage: Player %s sent message: %s"), *NewMsg.SenderName, *MessageText);
    }

    FDateTime CurrentTime = FDateTime::Now();
	int32 Hours = CurrentTime.GetHour();
    int32 Minutes = CurrentTime.GetMinute();
    float HourMin = Hours + (Minutes / 100.0f);



	NewMsg.SenderID = PlayerID;
    NewMsg.MessageText = MessageText;
    NewMsg.Timestamp = HourMin;
    NewMsg.MessageFlags = 0; // Normal message

    Room->AddChatMessage(NewMsg);
    return true;
}

uint16 PackTime(int32 Hours, int32 Minutes)
{
    return (Hours << 8) | (Minutes & 0xFF); // Compress hours & minutes into 2 bytes
}

bool AMP_ChatManager::EditMessage(const FString& RoomID, int32 MsgIndex, const FString& NewText, APlayerController* PlayerController)
{
    if (!HasAuthority() || !PlayerController) return false;

    AMP_ChatRoom* Room = GetRoom(RoomID);
    if (!Room) return false;

    FString PlayerID = GetPlayerID(PlayerController);
    if (PlayerID.IsEmpty()) return false;

    // Policy: only subscribed users may edit their own message (unless admin)
    if (!Room->SubscribedPlayerIDs.Contains(PlayerID))
        return false;

    if (!Room->MsgArray.Messages.IsValidIndex(MsgIndex))
        return false;

    // Only allow edit if sender is self or admin
    const FMP_ChatMessage& Msg = Room->MsgArray.Messages[MsgIndex];
    bool bIsAdmin = PlayerController->IsLocalPlayerController(); // Replace with your real admin logic
    if (Msg.SenderID != PlayerID && !bIsAdmin)
        return false;

    return Room->EditChatMessage(MsgIndex, NewText/*, NewFlags=1*/);
}

bool AMP_ChatManager::DeleteMessage(const FString& RoomID, int32 MsgIndex, APlayerController* PlayerController)
{
    if (!HasAuthority() || !PlayerController) return false;

    AMP_ChatRoom* Room = GetRoom(RoomID);
    if (!Room) return false;

    FString PlayerID = GetPlayerID(PlayerController);
    if (PlayerID.IsEmpty()) return false;

    if (!Room->MsgArray.Messages.IsValidIndex(MsgIndex))
        return false;

    // Only allow delete if sender is self or admin
    const FMP_ChatMessage& Msg = Room->MsgArray.Messages[MsgIndex];
    bool bIsAdmin = PlayerController->IsLocalPlayerController(); // Replace with your real admin logic
    if (Msg.SenderID != PlayerID && !bIsAdmin)
        return false;

    return Room->DeleteChatMessage(MsgIndex);
}

TArray<AMP_ChatRoom*> AMP_ChatManager::FindRoomsByPlayer(APlayerController* PlayerController) const
{
    TArray<AMP_ChatRoom*> Result;
    if (!PlayerController) return Result;

    FString PlayerID = GetPlayerID(PlayerController);
    if (PlayerID.IsEmpty()) return Result;

    for (const auto& Pair : ChatRooms)
    {
        if (Pair.Value && Pair.Value->SubscribedPlayerIDs.Contains(PlayerID))
        {
            Result.Add(Pair.Value);
        }
    }
    return Result;
}

AMP_ChatManager* AMP_ChatManager::GetChatManager(UObject* WorldContext)
{
    if (UWorld* World = WorldContext->GetWorld())
    {
        for (TActorIterator<AMP_ChatManager> It(World); It; ++It)
        {
            return *It;
        }
    }
    return nullptr;
}

void AMP_ChatManager::CleanupRoomIfEmpty(AMP_ChatRoom* Room)
{
    if (!Room) return;
    ChatRooms.Remove(Room->RoomDesc.RoomID);
    Room->Destroy();
	UE_LOG(LogTemp, Warning, TEXT("MP_ChatManager::CleanupRoomIfEmpty: Room '%s' destroyed"), *Room->RoomDesc.RoomID);
}

FString AMP_ChatManager::GetPlayerID(APlayerController* PC) const
{
    if (!PC) return FString();
    //const IMP_Chat_PlayerController_I* ChatInterface = Cast<IMP_Chat_PlayerController_I>(PC);
    //if (ChatInterface)
    //{
    //    return ChatInterface->GetSenderID();
    //}

    if (PC && PC->GetClass()->ImplementsInterface(UMP_Chat_PlayerController_I::StaticClass()))
    {
        return IMP_Chat_PlayerController_I::Execute_GetSenderID(PC);
    }

    // fallback: use PlayerState name
    return FString();
}




// Uncomment if you want to use ReplicationGraph for room management

//AMP_ChatRoom* AMP_ChatManager::GetOrCreateRoomActor(const FString& RoomID)
//{
//    if (!HasAuthority() || RoomID.IsEmpty())
//    {
//        return nullptr;
//    }
//
//    // If we already have this actor, return it
//    if (AMP_ChatRoom** Found = ChatRooms.Find(RoomID))
//    {
//        return *Found;
//    }
//
//    // Otherwise, spawn a new AMP_ChatRoom actor
//    FActorSpawnParameters Params;
//    Params.Owner = this;
//    AMP_ChatRoom* NewRoom = GetWorld()->SpawnActor<AMP_ChatRoom>(
//        AMP_ChatRoom::StaticClass(),
//        /* Location */ FVector::ZeroVector,
//        /* Rotation */ FRotator::ZeroRotator,
//        Params
//    );
//
//    if (NewRoom)
//    {
//        UE_LOG(LogTemp, Warning, TEXT("MP_ChatManager::GetOrCreateRoomActor: Spawned Room '%s'"), *RoomID);
//
//        // Initialize the room’s ID on the server
//        NewRoom->InitializeRoom(RoomID);
//        NewRoom->SetReplicates(true);
//        NewRoom->bAlwaysRelevant = false; // Only if you're using IsNetRelevantFor()
//
//        // Tell the ReplicationGraph about this actor
//        if (CachedRepGraph)
//        {
//            CachedRepGraph->RegisterRoomActor(RoomID, NewRoom);
//        }
//
//        // Store it in our map
//        ChatRooms.Add(RoomID, NewRoom);
//    }
//
//    return NewRoom;
//}

//UNetReplicationGraphConnection* AMP_ChatManager::GetRGConnectionForPlayer(APlayerController* Player) const
//{
//    if (!Player || !CachedRepGraph)
//    {
//        return nullptr;
//    }
//    return CachedRepGraph->FindConnection(Player);
//}

//void AMP_ChatManager::Server_JoinRoom_Implementation(APlayerController* Player, const FString& RoomID)
//{
//    if (!HasAuthority() || RoomID.IsEmpty())
//    {
//        UE_LOG(LogTemp, Error, TEXT("MP_ChatManager::JoinRoom failed: room not created!"));
//        return;
//    }
//
//    UNetReplicationGraphConnection* RGConn = GetRGConnectionForPlayer(Player);
//    if (RGConn && CachedRepGraph)
//    {
//        CachedRepGraph->SubscribeConnectionToRoom(RGConn, RoomID);
//        UE_LOG(LogTemp, Warning, TEXT("MP_ChatManager::Player %s joined room %s"), *Player->GetName(), *RoomID);
//    }
//    else
//    {
//        UE_LOG(LogTemp, Error, TEXT("MP_ChatManager::JoinRoom failed: missing RepGraph connection"));
//    }
//}


//void AMP_ChatManager::Server_LeaveRoom_Implementation(APlayerController* Player, const FString& RoomID)
//{
//    if (!HasAuthority() || RoomID.IsEmpty())
//    {
//        return;
//    }
//
//    UNetReplicationGraphConnection* RGConn = GetRGConnectionForPlayer(Player);
//    if (RGConn && CachedRepGraph)
//    {
//        CachedRepGraph->UnsubscribeConnectionFromRoom(RGConn, RoomID);
//    }
//}

