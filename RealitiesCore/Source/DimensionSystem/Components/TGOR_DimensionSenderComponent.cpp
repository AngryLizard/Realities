// The Gateway of Realities: Planes of Existence.


#include "TGOR_DimensionSenderComponent.h"

#include "CoreSystem/TGOR_Singleton.h"
#include "DimensionSystem/Data/TGOR_WorldData.h"
#include "DimensionSystem/Content/TGOR_Dimension.h"
#include "DimensionSystem/Data/TGOR_DimensionData.h"
#include "DimensionSystem/Components/TGOR_DimensionReceiverComponent.h"
#include "DimensionSystem/Components/TGOR_PilotComponent.h"
#include "CoreSystem/Storage/TGOR_Package.h"

UTGOR_DimensionSenderComponent::UTGOR_DimensionSenderComponent()
	: Super(),
	IsMidTeleport(false),
	Verbose(false)
{
}


void UTGOR_DimensionSenderComponent::Write_Implementation(FTGOR_GroupWritePackage& Package, UTGOR_Singleton* Context) const
{
	Super::Write_Implementation(Package, Context);

	Package.WriteEntry("OtherName", OtherPortalName);
}

bool UTGOR_DimensionSenderComponent::Read_Implementation(FTGOR_GroupReadPackage& Package, UTGOR_Singleton* Context)
{
	Super::Read_Implementation(Package, Context);

	Package.ReadEntry("OtherName", OtherPortalName);
	return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void UTGOR_DimensionSenderComponent::Connect(UTGOR_DimensionReceiverComponent* Receiver, ETGOR_ComputeEnumeration& Branches)
{
	Branches = ETGOR_ComputeEnumeration::Failed;

	if (IsValid(Receiver))
	{
		ETGOR_FetchEnumeration State;
		DimensionIdentifier = Receiver->GetDimensionIdentifier(State);
		OtherPortalName = Receiver->GetConnectionName();
		if (State == ETGOR_FetchEnumeration::Found)
		{
			Branches = ETGOR_ComputeEnumeration::Success;
		}
	}
}

void UTGOR_DimensionSenderComponent::DialPortal(const FTGOR_ConnectionSelection& Selection)
{
	OtherPortalName = Selection.Connection;
	DialDimension(Selection.Dimension, Selection.Suffix);
}


void UTGOR_DimensionSenderComponent::DialPortalName(const FName& Portal)
{
	if (!Portal.IsEqual(OtherPortalName))
	{
		if (Verbose)
		{
			RPORT(FString("Dialling portal to ") + Portal.ToString());
		}
		OtherPortalName = Portal;

		// Broadcast as loader level change even though the level technically didn't change
		ChangeLoaderLevel(LoaderLevel);
	}
}


////////////////////////////////////////////////////////////////////////////////////////////////////

UTGOR_DimensionReceiverComponent* UTGOR_DimensionSenderComponent::GetReceiverComponent() const
{
	SINGLETON_RETCHK(nullptr);

	UTGOR_WorldData* WorldData = Singleton->GetData<UTGOR_WorldData>();
	if (!IsValid(WorldData))
	{
		return nullptr;
	}

	// Make sure dimension is loaded
	if (!Super::IsReady())
	{
		return nullptr;
	}

	// Get dimension base
	UTGOR_DimensionData* Dimension = GetLinkedDimension();
	if (!IsValid(Dimension))
	{
		return nullptr;
	}

	// Link to portal
	return Cast<UTGOR_DimensionReceiverComponent>(Dimension->GetConnection(OtherPortalName));
}


void UTGOR_DimensionSenderComponent::EnterOther(UTGOR_PilotComponent* Component, bool KeepRelative, const FTransform& Offset, ETGOR_ComputeEnumeration& Branches)
{
	Branches = ETGOR_ComputeEnumeration::Failed;
	if (!IsValid(Component))
	{
		ERROR("Invalid component to teleport", Error);
	}

	SINGLETON_CHK;
	UTGOR_DimensionReceiverComponent* Receiver = GetReceiverComponent();
	if (IsValid(Receiver) && CanTeleport(Component))
	{
		IsMidTeleport = true;

		// Compute offset to portal
		FTransform Relative = Offset;
		const FTransform Transform = GetComponentTransform();
		if (KeepRelative)
		{
			const FTGOR_MovementPosition Position = Component->ComputePosition();
			Relative = FTransform(Position.Angular, Position.Linear) * Relative * Transform.Inverse();
		}

		if (Verbose)
		{
			RPORT(FString("Teleporting to ") + Receiver->GetConnectionName().ToString() + " in " + DimensionIdentifier.ToString());
		}

		Receiver->EnterSelf(Component, Relative, Branches);

		OnSentComponent.Broadcast(Component);
		IsMidTeleport = false;
	}
}


bool UTGOR_DimensionSenderComponent::IsReady() const
{
	SINGLETON_RETCHK(false);
	UTGOR_DimensionReceiverComponent* Receiver = GetReceiverComponent();
	if (!IsValid(Receiver)) return false;

	// Don't allow teleportation during an ongoing teleportation
	if (Receiver->IsPublic && !IsMidTeleport)
	{
		ETGOR_FetchEnumeration State;
		FName DimensionName = Receiver->GetDimensionIdentifier(State);
		if (State == ETGOR_FetchEnumeration::Found)
		{
			UTGOR_WorldData* WorldData = Singleton->GetData<UTGOR_WorldData>();
			if (IsValid(WorldData) && WorldData->IsOpen(DimensionName))
			{
				return true;
			}
		}
	}
	return false;
}

bool UTGOR_DimensionSenderComponent::CanTeleport(UTGOR_PilotComponent* Component) const
{
	if (!IsReady()) return false;
	UTGOR_DimensionReceiverComponent* Receiver = GetReceiverComponent();
	return !Receiver->IsOccupied(Component);
}