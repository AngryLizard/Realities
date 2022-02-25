#include "TGOR_Module.h"

#include "Realities/Utility/Storage/TGOR_Serialisation.h"
#include "Realities/Utility/Storage/TGOR_Package.h"
#include "Realities/Base/TGOR_Singleton.h"
#include "Realities/Base/TGOR_GameState.h"

UTGOR_Module::UTGOR_Module()
: ModulePriority(ETGOR_NetvarEnumeration::Client)
{
}
////////////////////////////////////////////////////////////////////////////////////////////////////

bool UTGOR_Module::Equals(const UTGOR_Module* Other) const
{
	return Compatible(Other);
}

bool UTGOR_Module::Merge(const UTGOR_Module* Other, ETGOR_NetvarEnumeration Priority)
{
	return Compatible(Other);
}

void UTGOR_Module::BuildModule(const UTGOR_Storage* Parent)
{
}

void UTGOR_Module::SetPriority(ETGOR_NetvarEnumeration Priority)
{
	ModulePriority = Priority;
}

bool UTGOR_Module::IsRelevant(ETGOR_NetvarEnumeration Priority) const
{
	return ModulePriority >= Priority;
}