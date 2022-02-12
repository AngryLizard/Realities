// The Gateway of Realities: Planes of Existence.


#include "TGOR_Spectator.h"

#include "Components/CapsuleComponent.h"
#include "RealitiesUGC/Mod/TGOR_ContentManager.h"

#include "CoreSystem/TGOR_Singleton.h"
#include "DimensionSystem/Components/TGOR_IdentityComponent.h"
#include "PlayerSystem/Components/TGOR_BodySpawnComponent.h"
#include "PlayerSystem/Gameplay/TGOR_OnlineController.h"

#include "Net/UnrealNetwork.h"


// Sets default values
ATGOR_Spectator::ATGOR_Spectator(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = true;
	bFindCameraComponentWhenViewTarget = true;
	SetReplicatingMovement(false);
	bReplicates = true;

	IdentityComponent = ObjectInitializer.CreateDefaultSubobject<UTGOR_IdentityComponent>(this, FName(TEXT("IdentityComponent")));
	IdentityComponent->SetIsReplicated(true);
	IdentityComponent->IgnoreStorage = true;

	SpawnComponent = ObjectInitializer.CreateDefaultSubobject<UTGOR_BodySpawnComponent>(this, FName(TEXT("SpawnComponent")));
	SpawnComponent->SetIsReplicated(true);
}

////////////////////////////////////////////////////////////////////////////////////////////////////
