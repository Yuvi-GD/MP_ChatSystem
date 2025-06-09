// Path: Plugins/MP_ChatSystem/Source/MP_ChatCore/Private/Network/MP_ChatRepNode.cpp

#include "Network/MP_ChatRepNode.h"  
#include "Network/MP_ChatRepGraph.h"

void UMP_ChatRepNode::RegisterRoomActor(const FString& RoomID, AActor* RoomActor)
{
    if (RoomID.Len() == 0 || RoomActor == nullptr)
    {
        return;
    }
    RoomIDToActor.Add(RoomID, RoomActor);
}

void UMP_ChatRepNode::SubscribeConnectionToRoom(UNetReplicationGraphConnection* Connection, const FString& RoomID)
{
    if (!Connection || RoomID.Len() == 0)
    {
        return;
    }
    TSet<FString>& RoomSet = ConnectionToRooms.FindOrAdd(Connection);
    RoomSet.Add(RoomID);
}

void UMP_ChatRepNode::UnsubscribeConnectionFromRoom(UNetReplicationGraphConnection* Connection, const FString& RoomID)
{
    if (!Connection || RoomID.Len() == 0)
    {
        return;
    }
    if (TSet<FString>* RoomSet = ConnectionToRooms.Find(Connection))
    {
        RoomSet->Remove(RoomID);
        if (RoomSet->Num() == 0)
        {
            ConnectionToRooms.Remove(Connection);
        }
    }
}

void UMP_ChatRepNode::GatherActorListsForConnection(const FConnectionGatherActorListParameters& Params)
{
    //UNetReplicationGraphConnection* Connection = Params.ConnectionManager;
    UNetReplicationGraphConnection* Connection = const_cast<UNetReplicationGraphConnection*>(&Params.ConnectionManager);


    if (!Connection)
    {
        // No valid connection: clear any previously gathered list
        ReplicationActorList.Reset();
        Super::GatherActorListsForConnection(Params);
        return;
    }

    // Determine which RoomIDs this connection is subscribed to
    TSet<FString>* SubscribedRooms = ConnectionToRooms.Find(Connection);
    if (!SubscribedRooms || SubscribedRooms->Num() == 0)
    {
        // Not subscribed to any room: clear list
        ReplicationActorList.Reset();
        Super::GatherActorListsForConnection(Params);
        return;
    }

    // Build a local array of actors to replicate
    TArray<AActor*> ActorsToReplicate;
    ActorsToReplicate.Reserve(SubscribedRooms->Num());
    for (const FString& RoomID : *SubscribedRooms)
    {
        if (AActor** ActorPtr = RoomIDToActor.Find(RoomID))
        {
            if (AActor* RoomActor = *ActorPtr)
            {
                ActorsToReplicate.Add(RoomActor);
            }
        }
    }

    // Reset the internal actor list and fill it with our selected room actors
    ReplicationActorList.Reset(ActorsToReplicate.Num());
    for (AActor* RoomActor : ActorsToReplicate)
    {
        ReplicationActorList.Add(RoomActor);
    }

    // Finally, let the base class push our ReplicationActorList into the graph’s OutGatheredLists
    Super::GatherActorListsForConnection(Params);
}

