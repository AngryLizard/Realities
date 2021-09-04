#include "TGOR_Module.h"

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