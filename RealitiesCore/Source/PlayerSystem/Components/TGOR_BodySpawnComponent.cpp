// The Gateway of Realities: Planes of Existence.
#include "TGOR_BodySpawnComponent.h"

#include "PlayerSystem/Data/TGOR_UserData.h"
#include "DimensionSystem/Data/TGOR_DimensionData.h"
#include "DimensionSystem/Components/TGOR_IdentityComponent.h"
#include "PlayerSystem/Gameplay/TGOR_OnlineController.h"
#include "CreatureSystem/Content/TGOR_Creature.h"
#include "CoreSystem/Gameplay/TGOR_GameInstance.h"
#include "CoreSystem/Storage/TGOR_Package.h"

#include "RealitiesUGC/Mod/TGOR_ContentManager.h"
#include "CoreSystem/TGOR_Singleton.h"

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

////////////////////////////////////////////////////////////////////////////////////////////////////

UTGOR_BodySpawnComponent::UTGOR_BodySpawnComponent()
	: Super()
{
}

void UTGOR_BodySpawnComponent::UpdateContent_Implementation(FTGOR_SpawnerDependencies& Dependencies)
{
	ITGOR_SpawnerInterface::UpdateContent_Implementation(Dependencies);
}

TMap<int32, UTGOR_SpawnModule*> UTGOR_BodySpawnComponent::GetModuleType_Implementation() const
{
	return TMap<int32, UTGOR_SpawnModule*>();
}


int32 UTGOR_BodySpawnComponent::RequestNewBody(ATGOR_OnlineController* OwnerController, FTGOR_CreateBodySetup Setup, bool AutoSwitch)
{
	SINGLETON_RETCHK(INDEX_NONE);
	UTGOR_UserData* UserData = Singleton->GetData<UTGOR_UserData>();
	if (IsValid(UserData) && IsValid(OwnerController))
	{
		// Spawn in persistent dimension
		ETGOR_FetchEnumeration State;
		UTGOR_DimensionData* Dimension = GetDimension(State);
		if (State == ETGOR_FetchEnumeration::Found)
		{
			// Get spawn location as defined
			const FTransform Transform = GetComponentTransform();

			// Spawn new body
			const int32 ActorIdewntifier = Dimension->GetUniqueActorIdentifier();
			UTGOR_IdentityComponent* Identity = Dimension->AddDimensionObject(ActorIdewntifier, Setup.Creature, Transform.GetLocation(), Transform.GetRotation().Rotator(), State);
			if (State == ETGOR_FetchEnumeration::Found)
			{
				APawn* Pawn = Cast<APawn>(Identity->GetOwner());
				if (IsValid(Pawn))
				{
					// Assign body to user
					const int32 UserKey = OwnerController->GetActiveUserKey();
					const int32 BodyIdentifier = UserData->AddUserBody(UserKey, Identity);
					OwnerController->RequestAppearance_Implementation(BodyIdentifier, Setup.Setup.Appearance);
					OwnerController->RequestActions_Implementation(BodyIdentifier, Setup.Setup.Actions);

					if (AutoSwitch)
					{
						OwnerController->RequestBodySwitch(BodyIdentifier);
					}

					NotifyBodySpawned_Implementation(Pawn);
					return BodyIdentifier;
				}
			}
		}
	}

	return INDEX_NONE;
}


void UTGOR_BodySpawnComponent::NotifyBodySpawned_Implementation(APawn* Pawn)
{
	OnBodySpawned.Broadcast(Pawn);
}