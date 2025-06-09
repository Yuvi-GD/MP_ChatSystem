// MP_ChatSystem/Source/MP_ChatCore/Public/Network/MP_ChatRepGraph.h
#pragma once

#include "CoreMinimal.h"
#include "ReplicationGraph.h"
#include "Network/MP_ChatRepNode.h"
#include "MP_ChatRepGraph.generated.h"

/**
 * UMP_ChatRepGraph
 *
 * Subclass of UReplicationGraph. On server startup it allocates exactly one
 * UMP_ChatRepNode and adds it as a global node. It exposes methods so that
 * AMP_ChatManager can register new room actors and subscribe/unsubscribe
 * connections from specific rooms.
 */

UCLASS(/*config= DefaultMP_ChatSystem*/)
class MP_CHATCORE_API UMP_ChatRepGraph : public UReplicationGraph
{
    GENERATED_BODY()

public:
    /** Called once on server startup to create & register our single chat node */
    virtual void InitGlobalGraphNodes() override;


    /** Called per new connection, but we leave it empty because our node does all filtering */
    virtual void InitConnectionGraphNodes(UNetReplicationGraphConnection* ConnectionManager) override;

    /** Register a new AChatRoom actor under the given RoomID */
    void RegisterRoomActor(const FString& RoomID, AActor* RoomActor);

    /** Subscribe the given connection to a RoomID */
    void SubscribeConnectionToRoom(UNetReplicationGraphConnection* Connection, const FString& RoomID);

    /** Unsubscribe the given connection from a RoomID */
    void UnsubscribeConnectionFromRoom(UNetReplicationGraphConnection* Connection, const FString& RoomID);

    UNetReplicationGraphConnection* FindConnection(APlayerController* PC);

    UPROPERTY()
    TMap<UNetConnection*, UNetReplicationGraphConnection*> NetConnectionMap;

private:
    /** Our single global node, allocated in InitGlobalGraphNodes */
    UPROPERTY()
    class UMP_ChatRepNode* ChatNode = nullptr;
    
};
