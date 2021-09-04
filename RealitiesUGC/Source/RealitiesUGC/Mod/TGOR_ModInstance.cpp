// The Gateway of Realities: Planes of Existence.


#include "TGOR_ModInstance.h"


bool FTGOR_ModInstance::IsEqual(FTGOR_ModInstance& Setup)
{
	if (ActiveMods.Num() != Setup.ActiveMods.Num()) return false;

	for (int i = 0; i < ActiveMods.Num(); i++)
	{
		if (!ActiveMods[i].Name.Equals(Setup.ActiveMods[i].Name)) return false;
		if (!ActiveMods[i].Version == Setup.ActiveMods[i].Version) return false;
	}

	return true;
}

FTGOR_ModSetupState::FTGOR_ModSetupState()
	: Status(), State(ETGOR_SetupStateEnumeration::Pending)
{
}