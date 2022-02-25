// The Gateway of Realities: Planes of Existence.

#include "TGOR_MetamorphInstance.h"
#include "Realities.h"

#include "Base/Content/TGOR_ContentManager.h"

FTGOR_MetamorphEntryInstance::FTGOR_MetamorphEntryInstance()
{
	AllowChildren = false;
	Instance.Quantity = 1.0f;
}

FTGOR_MetamorphInstance::FTGOR_MetamorphInstance()
{
	Min = FTGOR_ElementInstance(-100.0f);
	Max = FTGOR_ElementInstance(+100.0f);
	Time = 0.0f;
}


bool FTGOR_MetamorphInstance::Satisfies(const FTGOR_ElementInstance& Instance) const
{
	return(Instance.IsInBetween(Min, Max));
}

FTGOR_CombinationInstance::FTGOR_CombinationInstance()
{

}