// MP_ChatSystem/Source/MP_ChatCore/Private/Network/MP_ChatRepGraph.cpp

#include "Network/MP_ChatRepGraph.h"
#include "Network/MP_ChatRepNode.h"
#include "Engine/World.h"
#include "Engine/NetConnection.h"


void UMP_ChatRepGraph::InitGlobalGraphNodes()
{
    // Create our UMP_ChatRepNode (derived from UReplicationGraphNode_ActorList)
    ChatNode = CreateNewNode<UMP_ChatRepNode>();
    // Register it as a global node so that every connection’s GatherActorListsForConnection
    // will consult ChatNode
    AddGlobalGraphNode(ChatNode);
    UE_LOG(LogTemp, Warning, TEXT("MP_ChatRepGraph::InitGlobalGraphNodes: ChatNode added"));

}

void UMP_ChatRepGraph::InitConnectionGraphNodes(UNetReplicationGraphConnection* ConnectionManager)
{
    UE_LOG(LogTemp, Warning, TEXT("MP_ChatRepGraph::InitConnectionGraphNodes: New Connection Added"));

    if (ConnectionManager && ConnectionManager->NetConnection)
    {
        NetConnectionMap.Add(ConnectionManager->NetConnection, ConnectionManager);
    }
}

void UMP_ChatRepGraph::RegisterRoomActor(const FString& RoomID, AActor* RoomActor)
{
    if (ChatNode && RoomID.Len() && RoomActor)
    {
        ChatNode->RegisterRoomActor(RoomID, RoomActor);
    }
}

void UMP_ChatRepGraph::SubscribeConnectionToRoom(UNetReplicationGraphConnection* Connection, const FString& RoomID)
{
    if (ChatNode && Connection && RoomID.Len())
    {
        ChatNode->SubscribeConnectionToRoom(Connection, RoomID);
    }
}

void UMP_ChatRepGraph::UnsubscribeConnectionFromRoom(UNetReplicationGraphConnection* Connection, const FString& RoomID)
{
    if (ChatNode && Connection && RoomID.Len())
    {
        ChatNode->UnsubscribeConnectionFromRoom(Connection, RoomID);
    }
}

UNetReplicationGraphConnection* UMP_ChatRepGraph::FindConnection(APlayerController* PC)
{
    if (!PC || !PC->GetNetConnection())
    {
        return nullptr;
    }

    for (UNetReplicationGraphConnection* Conn : Connections)
    {
        if (Conn && Conn->NetConnection == PC->GetNetConnection())
        {
            UE_LOG(LogTemp, Warning, TEXT("MP_ChatRepGraph:: RepGraph connection: %s (%s)"),
            *GetNameSafe(Conn), *GetNameSafe(Conn ? Conn->NetConnection : nullptr));
            return Conn;
        }
    }
    return nullptr;
}
