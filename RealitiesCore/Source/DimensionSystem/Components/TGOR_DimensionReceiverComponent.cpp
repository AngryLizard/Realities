// The Gateway of Realities: Planes of Existence.


#include "TGOR_DimensionReceiverComponent.h"

#include "CoreSystem/TGOR_Singleton.h"
#include "DimensionSystem/Data/TGOR_WorldData.h"
#include "DimensionSystem/Content/TGOR_Dimension.h"
#include "DimensionSystem/Components/TGOR_DimensionSenderComponent.h"
#include "DimensionSystem/Components/TGOR_PilotComponent.h"

UTGOR_DimensionReceiverComponent::UTGOR_DimensionReceiverComponent()
	: Super(),
	PortalTransform(FTransform::Identity),
	IsMidTeleport(false),
	Verbose(false)
{
}

////////////////////////////////////////////////////////////////////////////////////////////////////


void UTGOR_DimensionReceiverComponent::Connect(UTGOR_DimensionSenderComponent* Sender, ETGOR_ComputeEnumeration& Branches)
{
	Branches = ETGOR_ComputeEnumeration::Failed;

	if (IsValid(Sender))
	{
		Sender->Connect(this, Branches);
	}
}

void UTGOR_DimensionReceiverComponent::SetPortalTransform(const FTransform& Transform)
{
	const FTransform Own = GetComponentTransform();
	PortalTransform = Transform * Own.Inverse();
}


void UTGOR_DimensionReceiverComponent::EnterSelf(UTGOR_PilotComponent* Component, const FTransform& Offset, ETGOR_ComputeEnumeration& Branches)
{
	Branches = ETGOR_ComputeEnumeration::Failed;
	if (!IsValid(Component))
	{
		ERROR("Invalid component to teleport", Error);
	}

	IsMidTeleport = true;

	// Add offset if enabled
	const FTransform Transform = GetComponentTransform();
	const FTransform Final = Offset * PortalTransform * Transform;

	FTGOR_MovementDynamic Dynamic;
	Dynamic.Linear = Final.GetLocation();
	Dynamic.Angular = Final.GetRotation();

	// Remember camera rotation for after teleport
	//const FQuat OldCamera = Avatar->ToLocalCameraRotation();

	if (Component->Teleport(Dynamic))
	{
		//Avatar->FromLocalCameraRotation(OldCamera);
	}
	else
	{
		//DrawDebugCoordinateSystem(GetWorld(), Target, Rotation.Rotator(), 10.0f, false, 10.0f, 0, 5.0f);
		ERROR("Actor couldn't teleport: Didn't fit.", Error);
	}

	OnReceivedComponent.Broadcast(Component);
	Branches = ETGOR_ComputeEnumeration::Success;

	IsMidTeleport = false;
}


bool UTGOR_DimensionReceiverComponent::IsOccupied(UTGOR_PilotComponent* Component) const
{
	return false;
}
