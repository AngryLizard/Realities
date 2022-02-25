// The Gateway of Realities: Planes of Existence.


#include "TGOR_UserData.h"

#include "Realities/Actors/Pawns/TGOR_Avatar.h"
#include "Realities/Components/Dimension/TGOR_IdentityComponent.h"
#include "Realities/Base/Controller/TGOR_OnlineController.h"
#include "Realities/Base/TGOR_Singleton.h"
#include "Realities/Utility/Storage/TGOR_Package.h"
#include "Realities/Mod/Trackers/TGOR_AvatarTracker.h"
#include "Realities/Mod/Spawner/Creatures/TGOR_Creature.h"

UTGOR_UserData::UTGOR_UserData()
{
	// Initialise random seed
	srand(time(NULL));
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
	if (HasUser(UserKey))
	{
		FTGOR_UserInstance& User = GetUser(UserKey);
		for (auto& Body : User.Bodies)
		{
			if (Body.Value.Avatar.IsValid())
			{
				// Update current avatar context
				const TSet<UTGOR_Content*> ContentContext = Body.Value.Avatar->GetContext();
				
				// Track automatic trackers
				UTGOR_Creature* Creature = Body.Value.Avatar->GetCreature();
				TArray<UTGOR_AvatarTracker*> AvatarTrackers = Creature->GetIListFromType<UTGOR_AvatarTracker>();
				for (UTGOR_AvatarTracker* Tracker : AvatarTrackers)
				{
					const float Delta = Tracker->Track(Body.Value.Avatar.Get(), DeltaSeconds);
					FTGOR_TrackerInstance& Instance = User.Trackers.Data.FindOrAdd(Tracker);
					Instance.Put(ContentContext, Delta);
				}

				// Flush cached trackers into user database
				Body.Value.Avatar->FlushTracker(User.Trackers);
			}
		}
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void UTGOR_UserData::UpdateAllUserBodies()
{
	SINGLETON_CHK;
	UTGOR_WorldData* WorldData = Singleton->GetWorldData();
	for (auto& User : UserBase)
	{
		UpdateUserBodies(User.Key);
	}
}

void UTGOR_UserData::UpdateUserBodies(int32 UserKey)
{
	SINGLETON_CHK;
	ETGOR_FetchEnumeration State;
	UTGOR_WorldData* WorldData = Singleton->GetWorldData();

	if (HasUser(UserKey))
	{
		FTGOR_UserInstance& User = GetUser(UserKey);
		for (auto& Body : User.Bodies)
		{
			if (Body.Value.Avatar.IsValid())
			{
				// Update identifier if loaded
				Body.Value.Creature = Body.Value.Avatar->GetCreature();
				UTGOR_IdentityComponent* Identity = Body.Value.Avatar->GetIdentity();
				Body.Value.Identifier.ActorIdentifier = Identity->GetActorIdentifier();
				Body.Value.Identifier.DimensionIdentifier = Identity->GetDimensionIdentifier(State);
			}
			else
			{
				// Try updating avatar if unloaded
				UTGOR_IdentityComponent* Identity = WorldData->GetIdentity(Body.Value.Identifier, State);
				if (State == ETGOR_FetchEnumeration::Found)
				{
					Body.Value.Avatar = Cast<ATGOR_Avatar>(Identity->GetOwner());
				}
			}
		}

		if (User.Controller.IsValid())
		{
			User.Controller->UpdateBodyDisplay();
		}
	}
}

int32 UTGOR_UserData::AddUserBody(int32 UserKey, ATGOR_Avatar* Avatar)
{
	FTGOR_UserInstance* Instance = UserBase.Find(UserKey);
	if (Instance)
	{
		int32 Identifier;
		do Identifier = FMath::RandRange(1000, 10000);
		while (Instance->Bodies.Contains(Identifier));


		FTGOR_UserBody Body;
		Body.Avatar = Avatar;
		Instance->Bodies.Add(Identifier, Body);

		UpdateUserBodies(UserKey);
		return Identifier;
	}
	return -1;
}

void UTGOR_UserData::RemoveUserBody(int32 UserKey, int32 Identifier)
{
	FTGOR_UserInstance* Instance = UserBase.Find(UserKey);
	if (Instance)
	{
		Instance->Bodies.Remove(Identifier);
	}
}


////////////////////////////////////////////////////////////////////////////////////////////////////

FTGOR_Buffer UTGOR_UserData::Encrypt(const FTGOR_Buffer& Message, const FTGOR_Buffer& PublicKey)
{
	return(Message);
}

FTGOR_Buffer UTGOR_UserData::Decrypt(const FTGOR_Buffer& Message, const FTGOR_Buffer& PrivateKey)
{
	return(Message);
}

FTGOR_Buffer UTGOR_UserData::CreatePrivateKey()
{
	FTGOR_Buffer PrivateKey;
	PrivateKey.Data.SetNum(8);
	return(PrivateKey);
}

FTGOR_Buffer UTGOR_UserData::CreatePublicKey(const FTGOR_Buffer& PrivateKey)
{
	FTGOR_Buffer PublicKey;
	PublicKey.Data.SetNum(8);
	return(PublicKey);
}

FTGOR_Buffer UTGOR_UserData::CreatePassword()
{
	const int32 n = 8;

	FTGOR_Buffer Password;
	Password.Data.SetNum(n);
	for (int i = 0; i < n; i++)
	{
		Password.Data[i] = (uint8)(FMath::Rand() % 0xFF);
	}
	return(Password);
}