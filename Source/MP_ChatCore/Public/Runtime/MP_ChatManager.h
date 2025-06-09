// Path: Plugins/MP_ChatSystem/Source/MP_ChatCore/Public/Runtime/MP_ChatManager.h
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Runtime/MP_ChatRoom.h"
#include "Network/MP_ChatRepGraph.h" 
#include "MP_ChatManager.generated.h"

/**
 * AMP_ChatManager
 *
 * A replicated actor that serves as the central chat hub. It:
 *  - Spawns or finds AMP_ChatRoom actors per RoomID.
 *  - Calls into UMP_ChatRepGraph to register room actors and manage subscriptions.
 *  - Handles three Server RPCs:
 *      * Server_JoinRoom
 *      * Server_LeaveRoom
 *      * Server_SendChatMessage
 */

UCLASS()
class MP_CHATCORE_API AMP_ChatManager : public AActor
{
    GENERATED_BODY()

public:
    AMP_ChatManager();

    /** Called when the actor is first placed or spawned */
    virtual void BeginPlay() override;


    // === Room Management === //

    /** Create a new chat room. If RoomID exists, returns existing. SERVER ONLY. */
    UFUNCTION(BlueprintCallable, Category = "MP_ChatCore|Manager|API")
    AMP_ChatRoom* CreateRoom(const FMP_ChatRoomDesc& RoomDesc, APlayerController* PlayerController);

    /** Lookup chat room by RoomID. Safe for Blueprint/clients. */
    UFUNCTION(BlueprintPure, Category = "MP_ChatCore|Manager|API")
    AMP_ChatRoom* GetRoom(const FString& RoomID) const;

    // === Player Join/Leave/Message ===

    /** Join a player to a room by RoomID. SERVER ONLY. */
    UFUNCTION(BlueprintCallable, Category = "MP_ChatCore|Manager|API")
    AMP_ChatRoom* JoinRoom(const FString& RoomID, APlayerController* PlayerController);

    /** Remove a player from a room by RoomID. SERVER ONLY. */
    UFUNCTION(BlueprintCallable, Category = "MP_ChatCore|Manager|API")
    bool LeaveRoom(const FString& RoomID, APlayerController* PlayerController);

    /** Send a chat message. SERVER ONLY. */
    UFUNCTION(BlueprintCallable, Category = "MP_ChatCore|Manager|API")
    bool SendMessage(const FString& RoomID, const FString& MessageText, APlayerController* PlayerController);

    /** Edit a chat message. SERVER ONLY. */
    UFUNCTION(BlueprintCallable, Category = "MP_ChatCore|Manager|API")
    bool EditMessage(const FString& RoomID, int32 MsgIndex, const FString& NewText, APlayerController* PlayerController);

    /** Delete a chat message. SERVER ONLY. */
    UFUNCTION(BlueprintCallable, Category = "MP_ChatCore|Manager|API")
    bool DeleteMessage(const FString& RoomID, int32 MsgIndex, APlayerController* PlayerController);


    // === Utility ===

    /** Returns all rooms a player is currently in (for UI/tools) */
    UFUNCTION(BlueprintCallable, Category = "MP_ChatCore|ChatManager|API")
    TArray<AMP_ChatRoom*> FindRoomsByPlayer(APlayerController* PlayerController) const;

    /** Get ChatManager singleton (world context) */
    UFUNCTION(BlueprintPure, Category = "MP_ChatCore|MP_ChatManager", meta = (WorldContext = "WorldContext"))
    static AMP_ChatManager* GetChatManager(UObject* WorldContext);


protected:

    /** Server‐only map: RoomID → the spawned AMP_ChatRoom actor */
    UPROPERTY()
    TMap<FString, AMP_ChatRoom*> ChatRooms;

    /** Destroy/dormant a room if empty (internal) */
    void CleanupRoomIfEmpty(AMP_ChatRoom* Room);

    /** Helper to get a player's ID via interface (null safe) */
    FString GetPlayerID(APlayerController* PC) const;

    //-----------------------------------------------------------------------------------//

	// // Get or create a room actor by RoomID
 //   AMP_ChatRoom* GetOrCreateRoomActor(const FString& RoomID);


	//// For Future Purpose of ReplicationGraph implementation
 //   /** Cached pointer to the ReplicationGraph instance (server‐only) */
 //   UPROPERTY()
 //   UMP_ChatRepGraph* CachedRepGraph = nullptr;

	// // Helper function to get the UNetReplicationGraphConnection for a player
 //   UNetReplicationGraphConnection* GetRGConnectionForPlayer(APlayerController* Player) const;
};
