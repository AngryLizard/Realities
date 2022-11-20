// The Gateway of Realities: Planes of Existence.

#include "TGOR_AimReceiverInterface.h"

bool ITGOR_AimReceiverInterface::TestAimCandidate_Implementation(UTGOR_AimTargetComponent* Candidate)
{
	return true;
}

bool ITGOR_AimReceiverInterface::TestAimTarget_Implementation(const FTGOR_AimInstance& Aim)
{
	return true;
}

void ITGOR_AimReceiverInterface::ApplyAimTarget_Implementation(const FTGOR_AimInstance& Aim)
{
}