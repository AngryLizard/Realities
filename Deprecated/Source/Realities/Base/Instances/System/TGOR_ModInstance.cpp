// The Gateway of Realities: Planes of Existence.


#include "TGOR_ModInstance.h"

#include "Realities/Mod/TGOR_Mod.h"


FTGOR_ModInstance::FTGOR_ModInstance()
{
}

bool FTGOR_ModInstance::IsEqual(FTGOR_ModInstance& Setup)
{
	if (BaseMod != Setup.BaseMod) return(false);
	if (ActiveMods.Num() != Setup.ActiveMods.Num()) return(false);

	for (int i = 0; i < ActiveMods.Num(); i++)
	{
		if (ActiveMods[i].Name.Equals(Setup.ActiveMods[i].Name)) return(false);
		if (ActiveMods[i].Version.Equals(Setup.ActiveMods[i].Version)) return(false);
	}

	return(true);
}