// The Gateway of Realities: Planes of Existence.

#include "TGOR_Setup.h"
#include "CoreSystem/TGOR_Singleton.h"
#include "CoreSystem/Data/TGOR_Data.h"

#define LOCTEXT_NAMESPACE "TGOR_Setup"

UTGOR_Setup::UTGOR_Setup()
	: Super()
{
	SetupName = LOCTEXT("CoreSetupName", "Core Mod Setup.");
}

bool UTGOR_Setup::Attempt_Implementation(bool IsServer)
{
	State.State = State.State = ETGOR_SetupStateEnumeration::Loading;
	return Super::Attempt_Implementation(IsServer);
}


////////////////////////////////////////////////////////////////////////////////////////////////////

UTGOR_Data* UTGOR_Setup::AddData(TSubclassOf<UTGOR_Data> Type)
{
	SINGLETON_RETCHK(nullptr);

	// Don't create multiuple datas of the same type
	UTGOR_Data* Data = Singleton->GetData(Type);
	if (IsValid(Data))
	{
		return Data;
	}

	Data = NewObject<UTGOR_Data>(Singleton.Get(), Type);
	Singleton->Datas.Emplace(Data);
	return Data;
}
#undef LOCTEXT_NAMESPACE
