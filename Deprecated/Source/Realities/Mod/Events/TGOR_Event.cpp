// The Gateway of Realities: Planes of Existence.

#include "TGOR_Event.h"

UTGOR_Event::UTGOR_Event()
	: Super(),
	EventPriority(ETGOR_NetvarEnumeration::Client)
{
}

bool UTGOR_Event::IsRelevant(ETGOR_NetvarEnumeration Priority) const
{
	return EventPriority >= Priority;
}
