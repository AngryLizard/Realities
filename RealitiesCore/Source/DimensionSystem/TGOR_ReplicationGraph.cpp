// The Gateway of Realities: Planes of Existence.

#include "TGOR_ReplicationGraph.h"

// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.
// 
#include "Net/UnrealNetwork.h"
#include "EngineUtils.h"
#include "UObject/UObjectIterator.h"
#include "Engine/NetConnection.h"
#include "DimensionSystem/Gameplay/TGOR_DimensionController.h"
#include "DimensionSystem/Data/TGOR_WorldData.h"
#include "CoreSystem/TGOR_Singleton.h"

#include "Engine/ChildConnection.h"

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


void UTGOR_ReplicationGraphNode_VolumeCell::AddStaticActor(const FNewReplicatedActorInfo& ActorInfo, FGlobalActorReplicationInfo& GlobalInfo, bool bParentNodeHandlesDormancyChange)
{
	if (GlobalInfo.bWantsToBeDormant)
	{
		// Pass to dormancy node
		EnsureDormancyNode()->AddDormantActor(ActorInfo, GlobalInfo);
	}
	else
	{
		// Put him in our non dormancy list
		Super::NotifyAddNetworkActor(ActorInfo);
	}

	// We need to be told if this actor changes dormancy so we can move him between nodes. Unless our parent is going to do it.
	if (!bParentNodeHandlesDormancyChange)
	{
		GlobalInfo.Events.DormancyChange.AddUObject(this, &UTGOR_ReplicationGraphNode_VolumeCell::OnStaticActorNetDormancyChange);
	}
}

void UTGOR_ReplicationGraphNode_VolumeCell::AddDynamicActor(const FNewReplicatedActorInfo& ActorInfo)
{
	UReplicationGraphNode* Node = EnsureDynamicNode();
	Node->NotifyAddNetworkActor(ActorInfo);
}

void UTGOR_ReplicationGraphNode_VolumeCell::RemoveStaticActor(const FNewReplicatedActorInfo& ActorInfo, FGlobalActorReplicationInfo& ActorRepInfo, bool bWasAddedAsDormantActor)
{
	if (bWasAddedAsDormantActor)
	{
		EnsureDormancyNode()->RemoveDormantActor(ActorInfo, ActorRepInfo);
	}
	else
	{
		Super::NotifyRemoveNetworkActor(ActorInfo);
	}

	ActorRepInfo.Events.DormancyChange.RemoveAll(this);
}

void UTGOR_ReplicationGraphNode_VolumeCell::RemoveDynamicActor(const FNewReplicatedActorInfo& ActorInfo)
{
	UReplicationGraphNode* Node = EnsureDynamicNode();
	Node->NotifyRemoveNetworkActor(ActorInfo);
}

void UTGOR_ReplicationGraphNode_VolumeCell::ConditionalCopyDormantActors(FActorRepListRefView& FromList, UReplicationGraphNode_DormancyNode* ToNode)
{
	if (GraphGlobals.IsValid())
	{
		for (int32 idx = FromList.Num() - 1; idx >= 0; --idx)
		{
			FActorRepListType Actor = FromList[idx];
			FGlobalActorReplicationInfo& GlobalInfo = GraphGlobals->GlobalActorReplicationInfoMap->Get(Actor);
			if (GlobalInfo.bWantsToBeDormant)
			{
				ToNode->NotifyAddNetworkActor(FNewReplicatedActorInfo(Actor));
				FromList.RemoveAtSwap(idx);
			}
		}
	}
}

void UTGOR_ReplicationGraphNode_VolumeCell::OnStaticActorNetDormancyChange(FActorRepListType Actor, FGlobalActorReplicationInfo& GlobalInfo, ENetDormancy NewValue, ENetDormancy OldValue)
{
	const bool bCurrentDormant = NewValue > DORM_Awake;
	const bool bPreviousDormant = OldValue > DORM_Awake;

	if (!bCurrentDormant && bPreviousDormant)
	{
		// Actor is now awake, remove from dormany node and add to non dormany list
		FNewReplicatedActorInfo ActorInfo(Actor);
		EnsureDormancyNode()->RemoveDormantActor(ActorInfo, GlobalInfo);
		Super::NotifyAddNetworkActor(ActorInfo);
	}
	else if (bCurrentDormant && !bPreviousDormant)
	{
		// Actor is now dormant, remove from non dormant list, add to dormant node
		FNewReplicatedActorInfo ActorInfo(Actor);
		Super::NotifyRemoveNetworkActor(ActorInfo);
		EnsureDormancyNode()->AddDormantActor(ActorInfo, GlobalInfo);
	}
}


UReplicationGraphNode* UTGOR_ReplicationGraphNode_VolumeCell::EnsureDynamicNode()
{
	if (DynamicNode == nullptr)
	{
		DynamicNode = CreateChildNode<UReplicationGraphNode_ActorListFrequencyBuckets>();
	}

	return DynamicNode;
}

UReplicationGraphNode_DormancyNode* UTGOR_ReplicationGraphNode_VolumeCell::EnsureDormancyNode()
{
	if (DormancyNode == nullptr)
	{
		DormancyNode = CreateChildNode<UReplicationGraphNode_DormancyNode>();
	}

	return DormancyNode;
}

void UTGOR_ReplicationGraphNode_VolumeCell::GetAllActorsInNode_Debugging(TArray<FActorRepListType>& OutArray) const
{
	Super::GetAllActorsInNode_Debugging(OutArray);
	if (DynamicNode)
	{
		DynamicNode->GetAllActorsInNode_Debugging(OutArray);
	}
	if (DormancyNode)
	{
		DormancyNode->GetAllActorsInNode_Debugging(OutArray);
	}
}



UTGOR_ReplicationGraphNode_VolumeSpatialization::UTGOR_ReplicationGraphNode_VolumeSpatialization()
{
	bRequiresPrepareForReplicationCall = true;
}

void UTGOR_ReplicationGraphNode_VolumeSpatialization::NotifyAddNetworkActor(const FNewReplicatedActorInfo& ActorInfo)
{
	ensureAlwaysMsgf(false, TEXT("UTGOR_ReplicationGraphNode_VolumeSpatialization::NotifyAddNetworkActor should not be called directly"));
}

bool UTGOR_ReplicationGraphNode_VolumeSpatialization::NotifyRemoveNetworkActor(const FNewReplicatedActorInfo& ActorInfo, bool bWarnIfNotFound)
{
	ensureAlwaysMsgf(false, TEXT("UTGOR_ReplicationGraphNode_VolumeSpatialization::NotifyRemoveNetworkActor should not be called directly"));
	return false;
}

void UTGOR_ReplicationGraphNode_VolumeSpatialization::AddActor_Dormancy(const FNewReplicatedActorInfo& ActorInfo, FGlobalActorReplicationInfo& ActorRepInfo)
{
	if (ActorRepInfo.bWantsToBeDormant)
	{
		AddActorInternal_Static(ActorInfo, ActorRepInfo, true);
	}
	else
	{
		AddActorInternal_Dynamic(ActorInfo);
	}

	// Tell us if dormancy changes for this actor because then we need to move it. Note we don't care about Flushing.
	ActorRepInfo.Events.DormancyChange.AddUObject(this, &UTGOR_ReplicationGraphNode_VolumeSpatialization::OnNetDormancyChange);
}

void UTGOR_ReplicationGraphNode_VolumeSpatialization::RemoveActor_Static(const FNewReplicatedActorInfo& ActorInfo)
{
	if (GraphGlobals.IsValid())
	{
		FGlobalActorReplicationInfo& GlobalInfo = GraphGlobals->GlobalActorReplicationInfoMap->Get(ActorInfo.Actor);
		RemoveActorInternal_Static(ActorInfo, GlobalInfo, GlobalInfo.bWantsToBeDormant);
	}
}

void UTGOR_ReplicationGraphNode_VolumeSpatialization::RemoveActor_Dormancy(const FNewReplicatedActorInfo& ActorInfo)
{
	if (GraphGlobals.IsValid())
	{
		FGlobalActorReplicationInfo& ActorRepInfo = GraphGlobals->GlobalActorReplicationInfoMap->Get(ActorInfo.Actor);
		if (ActorRepInfo.bWantsToBeDormant)
		{
			RemoveActorInternal_Static(ActorInfo, ActorRepInfo, true);
		}
		else
		{
			RemoveActorInternal_Dynamic(ActorInfo);
		}
	}
}

void UTGOR_ReplicationGraphNode_VolumeSpatialization::AddActorInternal_Dynamic(const FNewReplicatedActorInfo& ActorInfo)
{
	DynamicSpatializedActors.Emplace(ActorInfo.Actor, ActorInfo);
}

void UTGOR_ReplicationGraphNode_VolumeSpatialization::AddActorInternal_Static(const FNewReplicatedActorInfo& ActorInfo, FGlobalActorReplicationInfo& ActorRepInfo, bool bDormancyDriven)
{
	AActor* Actor = ActorInfo.Actor;
	const FVector Location = Actor->GetActorLocation();
	ActorRepInfo.WorldLocation = Location;
	
	StaticSpatializedActors.Emplace(Actor, FCachedStaticActorInfo(ActorInfo, bDormancyDriven));

	// Only put in cell right now if we aren't needing to rebuild the whole grid
	PutStaticActorIntoCell(ActorInfo, ActorRepInfo, bDormancyDriven);
}

void UTGOR_ReplicationGraphNode_VolumeSpatialization::RemoveActorInternal_Dynamic(const FNewReplicatedActorInfo& ActorInfo)
{
	if (FCachedDynamicActorInfo* DynamicActorInfo = DynamicSpatializedActors.Find(ActorInfo.Actor))
	{
		if (DynamicActorInfo->Volume.IsValid())
		{
			UTGOR_ReplicationGraphNode_VolumeCell* Node = GetGridNodesForActor(ActorInfo.Actor, DynamicActorInfo->Volume);
			if (IsValid(Node))
			{
				Node->RemoveDynamicActor(ActorInfo);
			}
		}
		DynamicSpatializedActors.Remove(ActorInfo.Actor);
	}
	else
	{
		if (StaticSpatializedActors.Remove(ActorInfo.Actor) > 0)
		{
		}
	}
}

void UTGOR_ReplicationGraphNode_VolumeSpatialization::RemoveActorInternal_Static(const FNewReplicatedActorInfo& ActorInfo, FGlobalActorReplicationInfo& ActorRepInfo, bool bWasAddedAsDormantActor)
{
	if (StaticSpatializedActors.Remove(ActorInfo.Actor) <= 0)
	{
		if (DynamicSpatializedActors.Remove(ActorInfo.Actor) > 0)
		{
			// ...
		}
	}

	// Remove it from the actual node it should still be in. Note that even if the actor did move in between this and the last replication frame, the FGlobalActorReplicationInfo would not have been updated
	UTGOR_ReplicationGraphNode_VolumeCell* Node = GetGridNodesForActor(ActorInfo.Actor, ActorRepInfo);
	if (IsValid(Node))
	{
		Node->RemoveStaticActor(ActorInfo, ActorRepInfo, bWasAddedAsDormantActor);
	}
}

void UTGOR_ReplicationGraphNode_VolumeSpatialization::OnNetDormancyChange(FActorRepListType Actor, FGlobalActorReplicationInfo& GlobalInfo, ENetDormancy NewValue, ENetDormancy OldValue)
{
	const bool bCurrentShouldBeStatic = (NewValue > DORM_Awake);
	const bool bPreviousShouldBeStatic = (OldValue > DORM_Awake);

	if (bCurrentShouldBeStatic && !bPreviousShouldBeStatic)
	{
		// Actor was dynamic and is now static. Remove from dynamic list and add to static.
		FNewReplicatedActorInfo ActorInfo(Actor);
		RemoveActorInternal_Dynamic(ActorInfo);
		AddActorInternal_Static(ActorInfo, GlobalInfo, true);
	}
	else if (!bCurrentShouldBeStatic && bPreviousShouldBeStatic)
	{
		FNewReplicatedActorInfo ActorInfo(Actor);
		RemoveActorInternal_Static(ActorInfo, GlobalInfo, true); // This is why we need the 3rd bool parameter: this actor was placed as dormant (and it no longer is at the moment of this callback)
		AddActorInternal_Dynamic(ActorInfo);
	}
}

void UTGOR_ReplicationGraphNode_VolumeSpatialization::NotifyResetAllNetworkActors()
{
	StaticSpatializedActors.Reset();
	DynamicSpatializedActors.Reset();
	Super::NotifyResetAllNetworkActors();
}

void UTGOR_ReplicationGraphNode_VolumeSpatialization::PutStaticActorIntoCell(const FNewReplicatedActorInfo& ActorInfo, FGlobalActorReplicationInfo& ActorRepInfo, bool bDormancyDriven)
{
	UTGOR_ReplicationGraphNode_VolumeCell* Node = GetGridNodesForActor(ActorInfo.Actor, ActorRepInfo);
	if (IsValid(Node))
	{
		Node->AddStaticActor(ActorInfo, ActorRepInfo, bDormancyDriven);
	}
}

TWeakObjectPtr<ATGOR_PhysicsVolume> UTGOR_ReplicationGraphNode_VolumeSpatialization::GetCellInfoForActor(const FVector& Location)
{
	SINGLETON_RETCHK(TWeakObjectPtr<ATGOR_PhysicsVolume>(nullptr));

	UTGOR_WorldData* WorldData = Singleton->GetData<UTGOR_WorldData>();
	if (IsValid(WorldData))
	{
		ATGOR_PhysicsVolume* Volume = WorldData->GetMainVolume();
		if (IsValid(Volume))
		{
			Volume = Volume->QueryNetwork(Location);
		}
		return Volume;
	}
	return nullptr;
}

UTGOR_ReplicationGraphNode_VolumeCell* UTGOR_ReplicationGraphNode_VolumeSpatialization::GetGridNodesForActor(FActorRepListType Actor, const FGlobalActorReplicationInfo& ActorRepInfo)
{
	return GetGridNodesForActor(Actor, GetCellInfoForActor(ActorRepInfo.WorldLocation));
}

UTGOR_ReplicationGraphNode_VolumeCell* UTGOR_ReplicationGraphNode_VolumeSpatialization::GetGridNodesForActor(FActorRepListType Actor, const TWeakObjectPtr<ATGOR_PhysicsVolume>& Volume)
{
	if (Volume.IsValid())
	{
		return GetCellNode(Volume);
	}
	return nullptr;
}

void UTGOR_ReplicationGraphNode_VolumeSpatialization::PrepareForReplication()
{
	FGlobalActorReplicationInfoMap* GlobalRepMap = nullptr;
	if (GraphGlobals.IsValid())
	{
		GlobalRepMap = GraphGlobals->GlobalActorReplicationInfoMap;
	}

	for (auto& MapIt : DynamicSpatializedActors)
	{
		FActorRepListType& DynamicActor = MapIt.Key;
		FCachedDynamicActorInfo& DynamicActorInfo = MapIt.Value;
		TWeakObjectPtr<ATGOR_PhysicsVolume> OldVolume = DynamicActorInfo.Volume;
		FNewReplicatedActorInfo& ActorInfo = DynamicActorInfo.ActorInfo;

		// Update location
		FGlobalActorReplicationInfo& ActorRepInfo = GlobalRepMap->Get(DynamicActor);

		// Check if volume changed
		TWeakObjectPtr<ATGOR_PhysicsVolume> NewVolume = OldVolume;
		ActorRepInfo.WorldLocation = DynamicActor->GetActorLocation();
		if (OldVolume.IsValid() && OldVolume->IsInside(ActorRepInfo.WorldLocation))
		{
			NewVolume = OldVolume->QueryNetwork(ActorRepInfo.WorldLocation);
		}
		else
		{
			NewVolume = GetCellInfoForActor(ActorRepInfo.WorldLocation);
		}


		if (OldVolume != NewVolume)
		{
			DynamicActorInfo.Volume = NewVolume;
			UTGOR_ReplicationGraphNode_VolumeCell* OldNode = GetGridNodesForActor(DynamicActor, OldVolume);
			if (IsValid(OldNode))
			{
				OldNode->RemoveDynamicActor(ActorInfo);
			}

			UTGOR_ReplicationGraphNode_VolumeCell* NewNode = GetGridNodesForActor(DynamicActor, NewVolume);
			if (IsValid(NewNode))
			{
				NewNode->AddDynamicActor(ActorInfo);
			}
		}
	}
}

void UTGOR_ReplicationGraphNode_VolumeSpatialization::GatherActorListsForConnection(const FConnectionGatherActorListParameters& Params)
{
	for (const FNetViewer& Viewer : Params.Viewers)
	{
		// Only add actors if the user is actually logged in
		ATGOR_DimensionController* Controller = Cast<ATGOR_DimensionController>(Viewer.InViewer);
		if (!IsValid(Controller) || Controller->IsOnline())
		{
			//RPORT("---------------------");
			const TWeakObjectPtr<ATGOR_PhysicsVolume> Volume = GetCellInfoForActor(Viewer.ViewLocation);
			if (Volume.IsValid() && IsValid(Volume->NetworkCell))
			{
				Volume->NetworkCell->GatherActorListsForConnection(Params);
				/*
				TArray<FActorRepListRawView>& lists = Params.OutGatheredReplicationLists.GetLists(EActorRepListTypeFlags::Default);
				for (FActorRepListRawView& list : lists)
				{
					RPORT(list.BuildDebugString());
				}
				*/
			}
		}
	}
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


bool FTGOR_ConnectionAlwaysRelevantNodePair::operator==(UNetConnection* InConnection) const
{
	// Any children should be looking at their parent connections instead.
	if (InConnection->GetUChildConnection() != nullptr)
	{
		InConnection = ((UChildConnection*)InConnection)->Parent;
	}

	return InConnection == NetConnection;
}


UTGOR_ReplicationGraph::UTGOR_ReplicationGraph()
{

}

void UTGOR_ReplicationGraph::InitGlobalActorClassSettings()
{
	Super::InitGlobalActorClassSettings();

	// ReplicationGraph stores internal associative data for actor classes. 
	// We build this data here based on actor CDO values.
	for (TObjectIterator<UClass> It; It; ++It)
	{
		UClass* Class = *It;
		AActor* ActorCDO = Cast<AActor>(Class->GetDefaultObject());
		if (!ActorCDO || !ActorCDO->GetIsReplicated())
		{
			continue;
		}

		// Skip SKEL and REINST classes.
		if (Class->GetName().StartsWith(TEXT("SKEL_")) || Class->GetName().StartsWith(TEXT("REINST_")))
		{
			continue;
		}

		FClassReplicationInfo ClassInfo;

		// Replication Graph is frame based. Convert NetUpdateFrequency to ReplicationPeriodFrame based on Server MaxTickRate.
		ClassInfo.ReplicationPeriodFrame = FMath::Max<uint32>((uint32)FMath::RoundToFloat(NetDriver->NetServerMaxTickRate / ActorCDO->NetUpdateFrequency), 1);

		if (ActorCDO->bAlwaysRelevant || ActorCDO->bOnlyRelevantToOwner)
		{
			ClassInfo.SetCullDistanceSquared(0.f);
		}
		else
		{
			ClassInfo.SetCullDistanceSquared(ActorCDO->NetCullDistanceSquared);
		}

		GlobalActorReplicationInfoMap.SetClassInfo(Class, ClassInfo);
	}
}

void UTGOR_ReplicationGraph::InitGlobalGraphNodes()
{
	// -----------------------------------------------
	//	Spatial Actors
	// -----------------------------------------------

	GridNode = CreateNewNode<UTGOR_ReplicationGraphNode_VolumeSpatialization>();
	AddGlobalGraphNode(GridNode);

	// -----------------------------------------------
	//	Always Relevant (to everyone) Actors
	// -----------------------------------------------
	AlwaysRelevantNode = CreateNewNode<UReplicationGraphNode_ActorList>();
	AddGlobalGraphNode(AlwaysRelevantNode);
}

void UTGOR_ReplicationGraph::InitConnectionGraphNodes(UNetReplicationGraphConnection* RepGraphConnection)
{
	Super::InitConnectionGraphNodes(RepGraphConnection);

	UReplicationGraphNode_AlwaysRelevant_ForConnection* AlwaysRelevantNodeForConnection = CreateNewNode<UReplicationGraphNode_AlwaysRelevant_ForConnection>();
	AddConnectionGraphNode(AlwaysRelevantNodeForConnection, RepGraphConnection);

	AlwaysRelevantForConnectionList.Emplace(RepGraphConnection->NetConnection, AlwaysRelevantNodeForConnection);
}

void UTGOR_ReplicationGraph::RouteAddNetworkActorToNodes(const FNewReplicatedActorInfo& ActorInfo, FGlobalActorReplicationInfo& GlobalInfo)
{
	if (ActorInfo.Actor->bAlwaysRelevant)
	{
		AlwaysRelevantNode->NotifyAddNetworkActor(ActorInfo);
	}
	else if (ActorInfo.Actor->bOnlyRelevantToOwner)
	{
		ActorsWithoutNetConnection.Add(ActorInfo.Actor);
	}
	else
	{
		// Note that UTGOR_ReplicationGraphNode_VolumeSpatialization has 3 methods for adding actor based on the mobility of the actor. Since AActor lacks this information, we will
		// add all spatialized actors as dormant actors: meaning they will be treated as possibly dynamic (moving) when not dormant, and as static (not moving) when dormant.
		GridNode->AddActor_Dormancy(ActorInfo, GlobalInfo);
	}
}

void UTGOR_ReplicationGraph::RouteRemoveNetworkActorToNodes(const FNewReplicatedActorInfo& ActorInfo)
{
	if (ActorInfo.Actor->bAlwaysRelevant)
	{
		AlwaysRelevantNode->NotifyRemoveNetworkActor(ActorInfo);
	}
	else if (ActorInfo.Actor->bOnlyRelevantToOwner && IsValid(ActorInfo.Actor))
	{
		if (UReplicationGraphNode* Node = ActorInfo.Actor->GetNetConnection() ? GetAlwaysRelevantNodeForConnection(ActorInfo.Actor->GetNetConnection()) : nullptr)
		{
			Node->NotifyRemoveNetworkActor(ActorInfo);
		}
	}
	else
	{
		GridNode->RemoveActor_Dormancy(ActorInfo);
	}
}

UReplicationGraphNode_AlwaysRelevant_ForConnection* UTGOR_ReplicationGraph::GetAlwaysRelevantNodeForConnection(UNetConnection* Connection)
{
	UReplicationGraphNode_AlwaysRelevant_ForConnection* Node = nullptr;
	if (Connection)
	{
		if (FTGOR_ConnectionAlwaysRelevantNodePair* Pair = AlwaysRelevantForConnectionList.FindByKey(Connection))
		{
			if (Pair->Node)
			{
				Node = Pair->Node;
			}
			else
			{
				UE_LOG(LogNet, Warning, TEXT("AlwaysRelevantNode for connection %s is null."), *GetNameSafe(Connection));
			}
		}
		else
		{
			UE_LOG(LogNet, Warning, TEXT("Could not find AlwaysRelevantNode for connection %s. This should have been created in UTGOR_ReplicationGraph::InitConnectionGraphNodes."), *GetNameSafe(Connection));
		}
	}
	else
	{
		// Basic implementation requires owner is set on spawn that never changes. A more robust graph would have methods or ways of listening for owner to change
		UE_LOG(LogNet, Warning, TEXT("Actor: is bOnlyRelevantToOwner but does not have an owning Netconnection. It will not be replicated"));
	}

	return Node;
}

int32 UTGOR_ReplicationGraph::ServerReplicateActors(float DeltaSeconds)
{
	// Route Actors needing owning net connections to appropriate nodes
	for (int32 idx = ActorsWithoutNetConnection.Num() - 1; idx >= 0; --idx)
	{
		bool bRemove = false;
		if (AActor* Actor = ActorsWithoutNetConnection[idx])
		{
			if (UNetConnection* Connection = Actor->GetNetConnection())
			{
				bRemove = true;
				if (UReplicationGraphNode_AlwaysRelevant_ForConnection* Node = GetAlwaysRelevantNodeForConnection(Actor->GetNetConnection()))
				{
					Node->NotifyAddNetworkActor(FNewReplicatedActorInfo(Actor));
				}
			}
		}
		else
		{
			bRemove = true;
		}

		if (bRemove)
		{
			ActorsWithoutNetConnection.RemoveAtSwap(idx, 1, false);
		}
	}

	return Super::ServerReplicateActors(DeltaSeconds);
}
