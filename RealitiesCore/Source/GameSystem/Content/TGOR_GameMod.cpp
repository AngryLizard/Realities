// The Gateway of Realities: Planes of Existence. By Salireths.


#include "TGOR_GameMod.h"

#include "CoreSystem/Setups/TGOR_BeginPlaySetup.h"
#include "CoreSystem/Setups/TGOR_ConfigSetup.h"
#include "CoreSystem/Setups/TGOR_PossessionSetup.h"
#include "DimensionSystem/Setups/TGOR_AwaitRequestsSetup.h"
#include "DimensionSystem/Setups/TGOR_PersistentSetup.h"
#include "PlayerSystem/Setups/TGOR_AccountSetup.h"
#include "PlayerSystem/Setups/TGOR_LoginSetup.h"
#include "PlayerSystem/Setups/TGOR_SpawnSetup.h"



UTGOR_GameMod::UTGOR_GameMod(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	Entry.Name = L"Core_C";
	Entry.Version = 0;

	Setups.Emplace(UTGOR_PossessionSetup::StaticClass());
	Setups.Emplace(UTGOR_ConfigSetup::StaticClass());
	Setups.Emplace(UTGOR_AccountSetup::StaticClass());
	Setups.Emplace(UTGOR_PersistentSetup::StaticClass());
	Setups.Emplace(UTGOR_AwaitRequestsSetup::StaticClass());
	Setups.Emplace(UTGOR_BeginPlaySetup::StaticClass());
	Setups.Emplace(UTGOR_LoginSetup::StaticClass());
}