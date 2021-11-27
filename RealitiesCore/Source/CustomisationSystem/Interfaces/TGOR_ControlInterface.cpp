// The Gateway of Realities: Planes of Existence.

#include "TGOR_ControlInterface.h"

ITGOR_ControlInterface::ITGOR_ControlInterface()
{
}

bool ITGOR_ControlInterface::IsInitialOnly() const
{
	return false;
}

FName ITGOR_ControlInterface::GetControlName() const
{
	return "Control";
}

FTransform ITGOR_ControlInterface::GetControlTransform(USkinnedMeshComponent* Component) const
{
	return FTransform::Identity;
}
