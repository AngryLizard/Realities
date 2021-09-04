// The Gateway of Realities: Planes of Existence.


#include "TGOR_RegisterInterface.h"

bool ITGOR_RegisterInterface::IsContentActive_Implementation(UTGOR_CoreContent* Content) const
{
	const TSet<UTGOR_CoreContent*> Active = GetActiveContent_Implementation();
	return Active.Contains(Content);
}

TSet<UTGOR_CoreContent*> ITGOR_RegisterInterface::GetActiveContent_Implementation() const
{
	return TSet<UTGOR_CoreContent*>();
}