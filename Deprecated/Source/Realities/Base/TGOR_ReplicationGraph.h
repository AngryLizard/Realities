// The Gateway of Realities: Planes of Existence.

#pragma once

#include "CoreMinimal.h"
#include "ReplicationGraph.h"

#include "Realities/Actors/Dimension/Volumes/TGOR_LevelVolume.h"
#include "Realities/Interfaces/TGOR_SingletonInterface.h"
#include "TGOR_ReplicationGraph.generated.h"

// Copied from 2D GridCell
UCLASS()
class REALITIES_API UTGOR_ReplicationGraphNode_VolumeCell : public UReplicationGraphNode_ActorList
{
	GENERATED_BODY()

public:

	virtual void NotifyAddNetworkActor(const FNewReplicatedActorInfo& ActorInfo) override { ensureMsgf(false, TEXT("UTGOR_ReplicationGraphNode_Simple2DSpatializationLeaf::NotifyAddNetworkActor not functional.")); }
	virtual bool NotifyRemoveNetworkActor(const FNewReplicatedActorInfo& ActorInfo, bool bWarnIfNotFound = true) override { ensureMsgf(false, TEXT("UTGOR_ReplicationGraphNode_Simple2DSpatializationLeaf::NotifyRemoveNetworkActor not functional.")); return false; }
	virtual void GetAllActorsInNode_Debugging(TArray<FActorRepListType>& OutArray) const override;

	void AddStaticActor(const FNewReplicatedActorInfo& ActorInfo, FGlobalActorReplicationInfo& GlobalRepInfo, bool bParentNodeHandlesDormancyChange);
	void AddDynamicActor(const FNewReplicatedActorInfo& ActorInfo);

	void RemoveStaticActor(const FNewReplicatedActorInfo& ActorInfo, FGlobalActorReplicationInfo& ActorRepInfo, bool bWasAddedAsDormantActor);
	void RemoveDynamicActor(const FNewReplicatedActorInfo& ActorInfo);

private:

	UPROPERTY()
		UReplicationGraphNode* DynamicNode = nullptr;
	UReplicationGraphNode* EnsureDynamicNode();

	UPROPERTY()
		UReplicationGraphNode_DormancyNode* DormancyNode = nullptr;
	UReplicationGraphNode_DormancyNode* EnsureDormancyNode();

	void OnActorDormancyFlush(FActorRepListType Actor, FGlobalActorReplicationInfo& GlobalInfo, UReplicationGraphNode_DormancyNode* DormancyNode);
	void ConditionalCopyDormantActors(FActorRepListRefView& FromList, UReplicationGraphNode_DormancyNode* ToNode);
	void OnStaticActorNetDormancyChange(FActorRepListType Actor, FGlobalActorReplicationInfo& GlobalInfo, ENetDormancy NewVlue, ENetDormancy OldValue);
};


// So far only makes sure not logged in clients will get any messages
UCLASS()
class REALITIES_API UTGOR_ReplicationGraphNode_VolumeSpatialization : public UReplicationGraphNode, public ITGOR_SingletonInterface
{
	GENERATED_BODY()

public:
	UTGOR_ReplicationGraphNode_VolumeSpatialization();

	virtual void NotifyAddNetworkActor(const FNewReplicatedActorInfo& Actor) override;
	virtual bool NotifyRemoveNetworkActor(const FNewReplicatedActorInfo& ActorInfo, bool bWarnIfNotFound = true) override;
	virtual void NotifyResetAllNetworkActors() override;
	virtual void PrepareForReplication() override;
	virtual void GatherActorListsForConnection(const FConnectionGatherActorListParameters& Params) override;

	void AddActor_Static(const FNewReplicatedActorInfo& ActorInfo, FGlobalActorReplicationInfo& ActorRepInfo) { AddActorInternal_Static(ActorInfo, ActorRepInfo, false); }
	void AddActor_Dynamic(const FNewReplicatedActorInfo& ActorInfo, FGlobalActorReplicationInfo& ActorRepInfo) { AddActorInternal_Dynamic(ActorInfo); }
	void AddActor_Dormancy(const FNewReplicatedActorInfo& ActorInfo, FGlobalActorReplicationInfo& ActorRepInfo);

	void RemoveActor_Static(const FNewReplicatedActorInfo& ActorInfo);
	void RemoveActor_Dynamic(const FNewReplicatedActorInfo& ActorInfo) { RemoveActorInternal_Dynamic(ActorInfo); }
	void RemoveActor_Dormancy(const FNewReplicatedActorInfo& ActorInfo);
	
	// Allow graph to override function for creating cell nodes in this grid.
	void AddSpatialRebuildBlacklistClass(UClass* Class) { RebuildSpatialBlacklistMap.Set(Class, true); }


protected:

	/** For adding new actor to the graph */
	virtual void AddActorInternal_Dynamic(const FNewReplicatedActorInfo& ActorInfo);
	virtual void AddActorInternal_Static(const FNewReplicatedActorInfo& ActorInfo, FGlobalActorReplicationInfo& ActorRepInfo, bool IsDormancyDriven);

	virtual void RemoveActorInternal_Dynamic(const FNewReplicatedActorInfo& Actor);
	virtual void RemoveActorInternal_Static(const FNewReplicatedActorInfo& Actor, FGlobalActorReplicationInfo& ActorRepInfo, bool WasAddedAsDormantActor);

private:

	// Classmap of actor classes which CANNOT force a rebuild of the spatialization tree. They will be clamped instead. E.g, projectiles.
	TClassMap<bool> RebuildSpatialBlacklistMap;
	
	struct FCachedDynamicActorInfo
	{
		FCachedDynamicActorInfo(const FNewReplicatedActorInfo& InInfo) : ActorInfo(InInfo) { }
		FNewReplicatedActorInfo ActorInfo;
		TWeakObjectPtr<ATGOR_PhysicsVolume> Volume;
	};
	TMap<FActorRepListType, FCachedDynamicActorInfo> DynamicSpatializedActors;

	struct FCachedStaticActorInfo
	{
		FCachedStaticActorInfo(const FNewReplicatedActorInfo& InInfo, bool bInDormDriven) : ActorInfo(InInfo), bDormancyDriven(bInDormDriven) { }
		FNewReplicatedActorInfo ActorInfo;
		bool bDormancyDriven = false; // This actor will be removed from static actor list if it becomes non dormant.
	};
	TMap<FActorRepListType, FCachedStaticActorInfo> StaticSpatializedActors;

	void OnNetDormancyChange(FActorRepListType Actor, FGlobalActorReplicationInfo& GlobalInfo, ENetDormancy NewVlue, ENetDormancy OldValue);

	void PutStaticActorIntoCell(const FNewReplicatedActorInfo& ActorInfo, FGlobalActorReplicationInfo& ActorRepInfo, bool bDormancyDriven);

	UTGOR_ReplicationGraphNode_VolumeCell* GetCellNode(const TWeakObjectPtr<ATGOR_PhysicsVolume>& Volume)
	{
		if (!IsValid(Volume->NetworkCell))
		{
			// Not using CreateChild because things are stored in the volumes. Dunno if that's bad
			Volume->NetworkCell = NewObject<UTGOR_ReplicationGraphNode_VolumeCell>(this);
			Volume->NetworkCell->Initialize(GraphGlobals);
		}

		return Volume->NetworkCell;
	}

	UTGOR_ReplicationGraphNode_VolumeCell* GetGridNodesForActor(FActorRepListType Actor, const FGlobalActorReplicationInfo& ActorRepInfo);
	UTGOR_ReplicationGraphNode_VolumeCell* GetGridNodesForActor(FActorRepListType Actor, const TWeakObjectPtr<ATGOR_PhysicsVolume>& Volume);
	TWeakObjectPtr<ATGOR_PhysicsVolume> GetCellInfoForActor(const FVector& Location);

	// This is a reused TArray for gathering actor nodes. Just to prevent using a stack based TArray everywhere or static/reset patten.
	TArray<UTGOR_ReplicationGraphNode_VolumeCell*> GatheredNodes;

	friend class AReplicationGraphDebugActor;
};




/*
 *
 */
USTRUCT()
struct FTGOR_ConnectionAlwaysRelevantNodePair
{
	GENERATED_BODY()
		FTGOR_ConnectionAlwaysRelevantNodePair() { }
	FTGOR_ConnectionAlwaysRelevantNodePair(UNetConnection* InConnection, UReplicationGraphNode_AlwaysRelevant_ForConnection* InNode) : NetConnection(InConnection), Node(InNode) { }
	bool operator==(UNetConnection* InConnection) const;

	UPROPERTY()
		UNetConnection* NetConnection = nullptr;

	UPROPERTY()
		UReplicationGraphNode_AlwaysRelevant_ForConnection* Node = nullptr;
};


/*
 *
 */
UCLASS(transient, config = Engine)
class UTGOR_ReplicationGraph :public UReplicationGraph
{
	GENERATED_BODY()

public:

	UTGOR_ReplicationGraph();

	virtual void InitGlobalActorClassSettings() override;
	virtual void InitGlobalGraphNodes() override;
	virtual void InitConnectionGraphNodes(UNetReplicationGraphConnection* RepGraphConnection) override;
	virtual void RouteAddNetworkActorToNodes(const FNewReplicatedActorInfo& ActorInfo, FGlobalActorReplicationInfo& GlobalInfo) override;
	virtual void RouteRemoveNetworkActorToNodes(const FNewReplicatedActorInfo& ActorInfo) override;

	virtual int32 ServerReplicateActors(float DeltaSeconds) override;

	UPROPERTY()
		UTGOR_ReplicationGraphNode_VolumeSpatialization* GridNode;

	UPROPERTY()
		UReplicationGraphNode_ActorList* AlwaysRelevantNode;

	UPROPERTY()
		TArray<FTGOR_ConnectionAlwaysRelevantNodePair> AlwaysRelevantForConnectionList;

	/** Actors that are only supposed to replicate to their owning connection, but that did not have a connection on spawn */
	UPROPERTY()
		TArray<AActor*> ActorsWithoutNetConnection;


	UReplicationGraphNode_AlwaysRelevant_ForConnection* GetAlwaysRelevantNodeForConnection(UNetConnection* Connection);
};
