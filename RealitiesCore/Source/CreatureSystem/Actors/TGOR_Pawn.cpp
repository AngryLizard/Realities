///////////////////////////////////////////////////
// The Gateway of Realities: Planes of Existence //
// Copyright 2016-2017                           //
///////////////////////////////////////////////////


#include "TGOR_Pawn.h"
#include "DimensionSystem/Components/TGOR_DimensionReceiverComponent.h"
#include "DimensionSystem/Components/TGOR_IdentityComponent.h"
#include "DimensionSystem/Data/TGOR_DimensionData.h"
#include "AttributeSystem/Components/TGOR_AttributeComponent.h"

#include "Components/SkeletalMeshComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/GameMode.h"

#include "CoreSystem/TGOR_Singleton.h"
#include "RealitiesUGC/Mod/TGOR_ContentManager.h"

#include "Net/UnrealNetwork.h"


ATGOR_Pawn::ATGOR_Pawn(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = true;
	bFindCameraComponentWhenViewTarget = true;
	SetReplicatingMovement(false);
	bReplicates = true;

	IdentityComponent = ObjectInitializer.CreateDefaultSubobject<UTGOR_IdentityComponent>(this, FName(TEXT("IdentityComponent")));
	AttributeComponent = ObjectInitializer.CreateDefaultSubobject<UTGOR_AttributeComponent>(this, FName(TEXT("AttributeComponent")));

	AutoPossessAI = EAutoPossessAI::Disabled;
}

bool ATGOR_Pawn::DespawnsOnLogout_Implementation()
{
	return false;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

UTGOR_DimensionReceiverComponent* ATGOR_Pawn::GetClosestPortal(TSubclassOf<UTGOR_DimensionReceiverComponent> Type) const
{
	DIMENSIONBASE_RETCHK(false);

	// There is at least one portal if state was found
	TArray<UTGOR_DimensionReceiverComponent*> Connections = DimensionData->GetCListOfType<UTGOR_DimensionReceiverComponent>(Type);
	if (Connections.Num() > 0)
	{
		// TODO: Could use faster custom sort to only compute each distance once
		const FVector Ours = GetActorLocation();
		Connections.Sort(
			[Ours](const UTGOR_DimensionReceiverComponent& A, const UTGOR_DimensionReceiverComponent& B) -> bool
		{
			const float DistA = (A.GetComponentLocation() - Ours).SizeSquared();
			const float DistB = (B.GetComponentLocation() - Ours).SizeSquared();
			return(DistA < DistB);
		});
		return(Connections[0]);
	}
	return(nullptr);
}