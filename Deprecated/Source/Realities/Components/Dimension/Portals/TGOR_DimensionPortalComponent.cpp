// The Gateway of Realities: Planes of Existence.


#include "TGOR_DimensionPortalComponent.h"

#include "Realities/Base/TGOR_Singleton.h"
#include "Realities/Base/Content/TGOR_ContentManager.h"
#include "Realities/Base/System/Data/TGOR_WorldData.h"
#include "Realities/Mod/Dimensions/TGOR_Dimension.h"
#include "Realities/Actors/Pawns/TGOR_Avatar.h"
#include "Realities/Base/System/Data/TGOR_DimensionData.h"
#include "Realities/Components/Dimension/TGOR_RegionComponent.h"
#include "Realities/Components/Movement/TGOR_MovementComponent.h"
#include "Realities/Base/Controller/TGOR_OnlineController.h"
#include "Realities/Utility/Storage/TGOR_Package.h"
#include "DrawDebugHelpers.h"

UTGOR_DimensionPortalComponent::UTGOR_DimensionPortalComponent()
	: Super(),
	PortalTransform(FTransform::Identity),
	IsMidTeleport(false),
	IsPublic(true), 
	Verbose(false)
{
}

void UTGOR_DimensionPortalComponent::BeginPlay()
{
	Super::BeginPlay();
}

bool UTGOR_DimensionPortalComponent::PreAssemble(UTGOR_DimensionData* Dimension)
{
	Dimension->AddPortal(PortalName, this);

	return(ITGOR_DimensionInterface::PreAssemble(Dimension));
}

void UTGOR_DimensionPortalComponent::Write_Implementation(FTGOR_GroupWritePackage& Package, UTGOR_Singleton* Context) const
{
	Super::Write_Implementation(Package, Context);

	Package.WriteEntry("PortalName", PortalName);
	Package.WriteEntry("OtherName", OtherPortalName);
}

bool UTGOR_DimensionPortalComponent::Read_Implementation(FTGOR_GroupReadPackage& Package, UTGOR_Singleton* Context)
{
	Super::Read_Implementation(Package, Context);

	Package.ReadEntry("PortalName", PortalName);
	Package.ReadEntry("OtherName", OtherPortalName);
	return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void UTGOR_DimensionPortalComponent::DialPortal(const FTGOR_PortalSelection& Selection)
{
	OtherPortalName = Selection.Portal;
	DialDimension(Selection.Dimension, Selection.Suffix);
}


void UTGOR_DimensionPortalComponent::DialPortalName(const FName& Portal)
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

FName UTGOR_DimensionPortalComponent::GetPortalName() const
{
	return PortalName;
}


void UTGOR_DimensionPortalComponent::SetPortalTransform(const FTransform& Transform)
{
	const FTransform Own = GetComponentTransform();
	PortalTransform = Transform * Own.Inverse();
}

void UTGOR_DimensionPortalComponent::Connect(ETGOR_ComputeEnumeration& Branches)
{
	Branches = ETGOR_ComputeEnumeration::Failed;

	UTGOR_DimensionPortalComponent* Portal = GetPortalComponent();
	if (IsValid(Portal))
	{
		ETGOR_FetchEnumeration State;
		Portal->DimensionIdentifier = GetDimensionIdentifier(State);
		Portal->OtherPortalName = GetPortalName();
		if (State == ETGOR_FetchEnumeration::Found)
		{
			Branches = ETGOR_ComputeEnumeration::Success;
		}
	}
}

UTGOR_DimensionPortalComponent* UTGOR_DimensionPortalComponent::GetPortalComponent() const
{
	if (IsValid(Singleton))
	{
		UTGOR_WorldData* WorldData = Singleton->GetWorldData();

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
		UTGOR_DimensionPortalComponent* Other = Dimension->GetPortal(OtherPortalName);
		if (IsValid(Other))
		{
			return Other;
		}
	}
	return nullptr;
}


void UTGOR_DimensionPortalComponent::EnterOther(UTGOR_PilotComponent* Component, bool KeepRelative, const FTransform& Offset, ETGOR_ComputeEnumeration& Branches)
{
	Branches = ETGOR_ComputeEnumeration::Failed;
	if (!IsValid(Component))
	{
		ERROR("Invalid component to teleport", Error);
	}

	SINGLETON_CHK;
	UTGOR_DimensionPortalComponent* Portal = GetPortalComponent();
	if (IsValid(Portal) && CanTeleport(Component))
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
			RPORT(FString("Teleporting to ") +  Portal->GetPortalName().ToString() + " in " + DimensionIdentifier.ToString());
		}

		Portal->EnterSelf(Component, Relative, Branches);

		IsMidTeleport = false;
	}
}

void UTGOR_DimensionPortalComponent::EnterSelf(UTGOR_PilotComponent* Component, const FTransform& Offset, ETGOR_ComputeEnumeration& Branches)
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

	OnTeleportedComponent.Broadcast(Component);
	Branches = ETGOR_ComputeEnumeration::Success;

	IsMidTeleport = false;
}


bool UTGOR_DimensionPortalComponent::IsReady() const
{
	UTGOR_DimensionPortalComponent* Portal = GetPortalComponent();
	if (!IsValid(Singleton) || !IsValid(Portal)) return false;

	// Don't allow teleportation during an ongoing teleportation
	if (Portal->IsPublic && !IsMidTeleport)
	{
		ETGOR_FetchEnumeration State;
		FName DimensionName = Portal->GetDimensionIdentifier(State);
		if (State == ETGOR_FetchEnumeration::Found)
		{
			UTGOR_WorldData* WorldData = Singleton->GetWorldData();
			if (WorldData->IsOpen(DimensionName))
			{
				return true;
			}
		}
	}
	return false;
}

bool UTGOR_DimensionPortalComponent::IsOccupied(UTGOR_PilotComponent* Component) const
{
	return false;
}

bool UTGOR_DimensionPortalComponent::CanTeleport(UTGOR_PilotComponent* Component) const
{
	if (!IsReady()) return false;
	UTGOR_DimensionPortalComponent* Portal = GetPortalComponent();
	return !Portal->IsOccupied(Component);
}