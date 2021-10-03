// The Gateway of Realities: Planes of Existence.


#include "TGOR_Spectator.h"

#include "Components/CapsuleComponent.h"
#include "RealitiesUGC/Mod/TGOR_ContentManager.h"

#include "CoreSystem/TGOR_Singleton.h"
#include "PlayerSystem/Data/TGOR_UserData.h"
#include "DimensionSystem/Data/TGOR_WorldData.h"
#include "DimensionSystem/Data/TGOR_DimensionData.h"
#include "DimensionSystem/Components/TGOR_IdentityComponent.h"
#include "MovementSystem/Components/TGOR_MovementComponent.h"
#include "AnimationSystem/Components/TGOR_ArmatureComponent.h"
#include "CreatureSystem/Content/TGOR_Creature.h"
#include "PlayerSystem/Gameplay/TGOR_PlayerController.h"

#include "CoreSystem/Gameplay/TGOR_GameInstance.h"
#include "PhysicsSystem/Components/TGOR_PhysicsComponent.h"
#include "CoreSystem/Storage/TGOR_Package.h"

#include "PlayerSystem/Gameplay/TGOR_OnlineController.h"
#include "DimensionSystem/Actors/TGOR_GateActor.h"
#include "Net/UnrealNetwork.h"

SERIALISE_INIT_SOURCE_IMPLEMENT(FTGOR_CreateBodySetup);


FTGOR_CreateBodySetup::FTGOR_CreateBodySetup()
	: Creature(nullptr)
{
}

void FTGOR_CreateBodySetup::Send(FTGOR_NetworkWritePackage& Package, UTGOR_Singleton* Context) const
{
	Package.WriteEntry(Creature);
	Package.WriteEntry(Setup);
}

void FTGOR_CreateBodySetup::Recv(FTGOR_NetworkReadPackage& Package, UTGOR_Singleton* Context)
{
	Package.ReadEntry(Creature);
	Package.ReadEntry(Setup);
}


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
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void ATGOR_Spectator::RequestNewBody_Implementation(FTGOR_CreateBodySetup Setup)
{
	SINGLETON_CHK;
	UTGOR_UserData* UserData = Singleton->GetData<UTGOR_UserData>();
	if (IsValid(UserData))
	{
		// Spawn in persistent dimension
		ETGOR_FetchEnumeration State;
		UTGOR_DimensionData* Dimension = GetDimension(State);
		if (State == ETGOR_FetchEnumeration::Found)
		{
			// Get spawn location as defined
			FTransform Transform = GetActorTransform();
			GetSpawnTransform(Transform);

			// Spawn new body
			const int32 ActorIdewntifier = Dimension->GetUniqueActorIdentifier();
			UTGOR_IdentityComponent* Identity = Dimension->AddDimensionObject(ActorIdewntifier, Setup.Creature, Transform.GetLocation(), Transform.GetRotation().Rotator(), State);
			if (State == ETGOR_FetchEnumeration::Found)
			{
				APawn* Pawn = Cast<APawn>(Identity->GetOwner());
				if (IsValid(Pawn))
				{
					// Assign body to user
					AController* OwnController = GetController();
					if (IsValid(OwnController))
					{
						ATGOR_OnlineController* OnlineController = Cast<ATGOR_OnlineController>(OwnController);
						if (IsValid(OnlineController))
						{
							const int32 UserKey = OnlineController->GetActiveUserKey();
							const int32 BodyIdentifier = UserData->AddUserBody(UserKey, Identity);

							OnlineController->RequestAppearance_Implementation(BodyIdentifier, Setup.Setup.Appearance);
							OnlineController->RequestActions_Implementation(BodyIdentifier, Setup.Setup.Actions);
							OnlineController->RequestBodySwitch(BodyIdentifier);
							OnBodyRequest(Pawn);
						}
						else
						{
							// Technically an error case since only NPCs have non-online controllers and those don't call the server
							OwnController->Possess(Pawn);
							OnBodyRequest(Pawn);
						}
					}
				}
			}
		}
	}
}

bool ATGOR_Spectator::RequestNewBody_Validate(FTGOR_CreateBodySetup Setup)
{
	return true;
}