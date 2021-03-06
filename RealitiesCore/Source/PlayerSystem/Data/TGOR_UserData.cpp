// The Gateway of Realities: Planes of Existence.


#include "TGOR_UserData.h"

#include "CoreSystem/TGOR_Singleton.h"
#include "CoreSystem/Storage/TGOR_Package.h"

#include "KnowledgeSystem/Components/TGOR_TrackerComponent.h"
#include "DimensionSystem/Components/TGOR_IdentityComponent.h"
#include "DimensionSystem/Data/TGOR_WorldData.h"

#include "CreatureSystem/Content/TGOR_PawnTracker.h"
#include "CreatureSystem/Content/TGOR_Creature.h"

#include "PlayerSystem/Gameplay/TGOR_OnlineController.h"

UTGOR_UserData::UTGOR_UserData()
{
	// Initialise random seed
	srand(FDateTime::Now().GetMillisecond());
}

void UTGOR_UserData::Write_Implementation(FTGOR_GroupWritePackage& Package, UTGOR_Singleton* Context) const
{
	Package.WriteEntry("UserBase", UserBase);
}

bool UTGOR_UserData::Read_Implementation(FTGOR_GroupReadPackage& Package, UTGOR_Singleton* Context)
{
	return Package.ReadEntry("UserBase", UserBase);
}


bool UTGOR_UserData::HasUser(int32 Key) const
{
	return UserBase.Contains(Key);
}

FTGOR_UserInstance& UTGOR_UserData::GetUser(int32 Key)
{
	return UserBase[Key];
}

bool UTGOR_UserData::ForEachController(ControllerLambda Func) const
{
	for (const auto& Pair : UserBase)
	{
		if (Pair.Value.Controller.IsValid())
		{
			if (!Func(Pair.Value.Controller.Get()))
			{
				return true;
			}
		}
	}
	return true;
}

bool UTGOR_UserData::ForEachUser(ConstUserLambda Func) const
{
	for (const auto& Pair : UserBase)
	{
		if (!Func(Pair.Value))
		{
			return false;
		}
	}
	return true;
}

bool UTGOR_UserData::ForEachUser(UserLambda Func)
{
	for (auto& Pair : UserBase)
	{
		if (!Func(Pair.Value))
		{
			return false;
		}
	}
	return true;
}

int32 UTGOR_UserData::StoreUser(const FTGOR_UserInstance& User)
{
	int32 Key;
	do Key = FMath::RandRange(1000, 10000);
	while (UserBase.Contains(Key));

	UserBase.Add(Key, User);
	return Key;
}

void UTGOR_UserData::RemoveUser(int32 Key)
{
	UserBase.Remove(Key);
}

void UTGOR_UserData::ClearUserTable()
{
	UserBase.Reset();
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void UTGOR_UserData::UpdateTrackers(int32 UserKey, float DeltaSeconds)
{
	SINGLETON_CHK;
	if (HasUser(UserKey))
	{
		FTGOR_UserInstance& User = GetUser(UserKey);
		for (auto& Body : User.Bodies)
		{
			UTGOR_WorldData* WorldData = Singleton->GetData<UTGOR_WorldData>();
			if (IsValid(WorldData))
			{
				APawn* Pawn = WorldData->GetTrackedActor<APawn>(Body.Key);
				UTGOR_Creature* Creature = Cast<UTGOR_Creature>(Body.Value.Spawner);
				if (IsValid(Pawn) && IsValid(Creature))
				{
					UTGOR_TrackerComponent* Tracker = Pawn->FindComponentByClass<UTGOR_TrackerComponent>();
					if (IsValid(Tracker))
					{
						// Update current avatar context
						const TSet<UTGOR_CoreContent*> ContentContext = Tracker->GetContext();

						// Track automatic trackers
						const TArray<UTGOR_PawnTracker*> PawnTrackers = Creature->Instanced_TrackerInsertions.GetListOfType<UTGOR_PawnTracker>();
						for (UTGOR_PawnTracker* PawnTracker : PawnTrackers)
						{
							const float Delta = PawnTracker->Track(Pawn, DeltaSeconds);
							FTGOR_TrackerInstance& Instance = User.Trackers.Data.FindOrAdd(PawnTracker);
							Instance.Put(ContentContext, Delta);
						}

						// Flush cached trackers into user database
						Tracker->FlushTracker(User.Trackers);
					}
				}
			}
		}
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////

int32 UTGOR_UserData::AddUserBody(int32 UserKey, UTGOR_IdentityComponent* Component)
{
	SINGLETON_RETCHK(INDEX_NONE);
	if (FTGOR_UserInstance* Instance = UserBase.Find(UserKey))
	{
		UTGOR_WorldData* WorldData = Singleton->GetData<UTGOR_WorldData>();
		if (IsValid(WorldData))
		{
			const int32 Identifier = WorldData->RegisterTracked(Component, true);
			if (Identifier != INDEX_NONE)
			{
				FTGOR_UserBody Body;
				Body.Spawner = Component->GetActorSpawner();
				Instance->Bodies.Add(Identifier, Body);
				return Identifier;
			}
		}
	}
	return INDEX_NONE;
}

void UTGOR_UserData::RemoveUserBody(int32 UserKey, int32 Identifier)
{
	SINGLETON_CHK;
	FTGOR_UserInstance* Instance = UserBase.Find(UserKey);
	if (Instance)
	{
		if (Instance->Bodies.Contains(Identifier))
		{
			Instance->Bodies.Remove(Identifier);

			// If no user has this body, remove from tracked
			if (ForEachUser([Identifier](const FTGOR_UserInstance& User) { return !User.Bodies.Contains(Identifier); }))
			{
				UTGOR_WorldData* WorldData = Singleton->GetData<UTGOR_WorldData>();
				if (IsValid(WorldData))
				{
					WorldData->UnregisterTracked(Identifier);
				}
			}
		}
	}
}
