// The Gateway of Realities: Planes of Existence.

#include "TGOR_Input.h"

UTGOR_Input::UTGOR_Input()
: Super(),
Replication(ETGOR_InputRepEnumeration::Gradual), 
Threshold(0.5f)
{
}

bool UTGOR_Input::IsReplicated() const
{
	return Replication >= ETGOR_InputRepEnumeration::Binary;
}

bool UTGOR_Input::IsBinary() const
{
	return Replication == ETGOR_InputRepEnumeration::Binary;
}

bool UTGOR_Input::IsActive(const FTGOR_Normal& Value) const
{
	return Value.Value > Threshold;
}
