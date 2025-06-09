// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
//#include "Data/MP_ChatData.h"
#include "MP_ChatFastArray.h"
#include "Engine/NetConnection.h" // Ensure this is included for UNetReplicationGraphConnection
#include "ReplicationGraph.h" // Ensure the module containing this header is properly added to the build dependencies
#include "ReplicationGraphTypes.h" // Ensure this is included for FReplicationActorInfoIterator
#include "MP_ChatRepNode.generated.h" // Ensure the module containing this header is properly added to the build dependencies

//class UMP_ChatManager;
//class AActor;

/**
 * UMP_ChatRepNode
 *
 * Subclass of UReplicationGraphNode_ActorList that “fans out” each chat room’s
 * AChatRoom actor only to the connections subscribed to that room.
 *
 * - RoomIDToActor      maps RoomID → AChatRoom* (the actor that owns the FastArray)
 * - ConnectionToRooms  maps UNetReplicationGraphConnection* → set of RoomID strings
 */

UCLASS(Transient, Config = DefaultMP_ChatSystem)
class MP_CHATCORE_API UMP_ChatRepNode : public UReplicationGraphNode_ActorList
{
	GENERATED_BODY()

public:
    /**
     * Called each frame for each client connection. Unreal asks, “Which actors
     * should I replicate to this connection?”
     *
     * We override this to add exactly those AChatRoom actors whose RoomID is in
     * ConnectionToRooms[Connection].
     *
     * @param Params  - Contains ConnectionManager (UNetReplicationGraphConnection*).
     */
    virtual void GatherActorListsForConnection(const FConnectionGatherActorListParameters& Params) override;

    /** Map RoomID → pointer to that room’s AChatRoom actor (server side). */
    TMap<FString, AActor*> RoomIDToActor;

    /** Map each connection → set of RoomIDs that connection has joined. */
    TMap<UNetReplicationGraphConnection*, TSet<FString>> ConnectionToRooms;

    /** Add a new room actor under the given RoomID. Called by the ChatManager when spawning. */
    void RegisterRoomActor(const FString& RoomID, AActor* RoomActor);

    /** Subscribe a connection to a given RoomID. Called by ChatManager on join. */
    void SubscribeConnectionToRoom(UNetReplicationGraphConnection* Connection, const FString& RoomID);

    /** Unsubscribe a connection from a given RoomID. Called by ChatManager on leave. */
    void UnsubscribeConnectionFromRoom(UNetReplicationGraphConnection* Connection, const FString& RoomID);
};
