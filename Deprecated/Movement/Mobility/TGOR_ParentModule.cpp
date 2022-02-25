
#include "TGOR_ParentModule.h"

#include "Realities/Components/Movement/TGOR_MobilityComponent.h"
#include "Realities/Base/Inventory/Modules/TGOR_Module.h"
#include "Realities/Base/Inventory/TGOR_Storage.h"
#include "Realities/Mod/Inputs/TGOR_Input.h"

#include "Realities/Utility/Storage/TGOR_Serialisation.h"
#include "Realities/Utility/Storage/TGOR_Package.h"
#include "Realities/Base/TGOR_Singleton.h"
#include "Realities/Base/TGOR_GameState.h"

#include "Realities/Base/System/Data/TGOR_WorldData.h"

////////////////////////////////////////////////////////////////////////////////////////////////////

UTGOR_ParentModule::UTGOR_ParentModule()
	: Super()
{
}

bool UTGOR_ParentModule::Equals(const UTGOR_Module* Other) const
{
	if (!Super::Equals(Other))
	{
		return false;
	}

	const UTGOR_ParentModule* OtherModule = static_cast<const UTGOR_ParentModule*>(Other);
	return Component == OtherModule->Component && ParentIndex == OtherModule->ParentIndex && (Local.Compare(OtherModule->Local, FTGOR_PositionThreshold(SMALL_NUMBER, SMALL_NUMBER)) > SMALL_NUMBER);
}

bool UTGOR_ParentModule::Merge(const UTGOR_Module* Other, ETGOR_NetvarEnumeration Priority)
{
	if (!Super::Merge(Other, Priority))
	{
		return false;
	}

	const UTGOR_ParentModule* OtherModule = static_cast<const UTGOR_ParentModule*>(Other);
	Component = OtherModule->Component;
	ParentIndex = OtherModule->ParentIndex;
	Local = OtherModule->Local;
	return true;
}

void UTGOR_ParentModule::Write_Implementation(FTGOR_GroupWritePackage& Package, UTGOR_Singleton* Context) const
{
	Package.WriteEntry("Component", Component);
	Package.WriteEntry("ParentIndex", ParentIndex);
	Package.WriteEntry("Local", Local);
	Super::Write_Implementation(Package, Context);
}

bool UTGOR_ParentModule::Read_Implementation(FTGOR_GroupReadPackage& Package, UTGOR_Singleton* Context)
{
	Package.ReadEntry("Component", Component);
	Package.ReadEntry("ParentIndex", ParentIndex);
	Package.ReadEntry("Local", Local);
	return Super::Read_Implementation(Package, Context);
}

void UTGOR_ParentModule::Send_Implementation(FTGOR_NetworkWritePackage& Package, UTGOR_Singleton* Context) const
{
	Package.WriteEntry(Component);
	Package.WriteEntry(ParentIndex);
	Package.WriteEntry(Local);
	Super::Send_Implementation(Package, Context);
}

bool UTGOR_ParentModule::Recv_Implementation(FTGOR_NetworkReadPackage& Package, UTGOR_Singleton* Context)
{
	Package.ReadEntry(Component);
	Package.ReadEntry(ParentIndex);
	Package.ReadEntry(Local);
	return Super::Recv_Implementation(Package, Context);
}


void UTGOR_ParentModule::Clear()
{
	Super::Clear();
	Component.Reset();
}

FString UTGOR_ParentModule::Print() const
{
	return FString::Printf(TEXT("Linear: %s, Parent: %d, Component: %s, %s"), *Local.Linear.ToString(), ParentIndex.Index, *Component->GetName(), *Super::Print());
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void UTGOR_ParentModule::ResetToComponent(UTGOR_MobilityComponent* Owner)
{
	const FTGOR_MovementPosition ParentPosition = ComputeParentPosition();
	Local.PositionToBase(ParentPosition, Owner->GetComponentPosition());
	MarkDirty();
}

////////////////////////////////////////////////////////////////////////////////////////////////////

FTGOR_MovementPosition UTGOR_ParentModule::ComputePosition() const
{
	return Local.BaseToPosition(ComputeParentPosition());
}

FTGOR_MovementPosition UTGOR_ParentModule::ComputeParentPosition() const
{
	if (Component.IsValid())
	{
		const FTGOR_MovementPosition Position = Component->ComputePosition();
		if (ParentIndex)
		{
			const FTGOR_MovementPosition Part = Component->GetIndexTransform(ParentIndex.Index);
			return Part.BaseToPosition(Position);
		}
		return Position;
	}
	return FTGOR_MovementPosition();
}

////////////////////////////////////////////////////////////////////////////////////////////////////

FTGOR_MovementSpace UTGOR_ParentModule::ComputeSpace() const
{
	return Local.BaseToSpace(ComputeParentSpace());
}

FTGOR_MovementSpace UTGOR_ParentModule::ComputeParentSpace() const
{
	if (Component.IsValid())
	{
		FTGOR_MovementSpace Space = Component->ComputeSpace();
		if (ParentIndex)
		{
			const FTGOR_MovementDynamic Part = Component->GetIndexTransform(ParentIndex.Index);
			return Part.BaseToSpace(Space);
		}

		return Space;
	}
	return FTGOR_MovementSpace();
}

////////////////////////////////////////////////////////////////////////////////////////////////////


void UTGOR_ParentModule::SimulateDynamic(const FTGOR_MovementDynamic& Dynamic)
{
	const FTGOR_MovementDynamic ParentDynamic = ComputeParentSpace();

	Local.DynamicToBase(ParentDynamic, Dynamic);
	MarkDirty();
}

void UTGOR_ParentModule::SimulateDynamicWith(UTGOR_MobilityComponent* Owner, int32 Index, const FTGOR_MovementDynamic& Dynamic)
{
	const FTGOR_MovementDynamic ParentDynamic = ComputeParentSpace();
	const FTGOR_MovementDynamic Part = Owner->GetIndexTransform(Index);

	FTGOR_MovementDynamic Relative;
	Relative.DynamicToBase(Part, Dynamic);
	Local.DynamicToBase(ParentDynamic, Relative);
	MarkDirty();
}

////////////////////////////////////////////////////////////////////////////////////////////////////

FName UTGOR_ParentModule::GetSocketName() const
{
	if (Component.IsValid())
	{
		return Component->GetNameFromIndex(ParentIndex);
	}
	return FName();
}

UTGOR_MobilityComponent* UTGOR_ParentModule::GetParent() const
{
	return Component.Get();
}

int32 UTGOR_ParentModule::GetParentIndex() const
{
	return ParentIndex.Index;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

float UTGOR_ParentModule::Compare(UTGOR_ParentModule* Other, const FTGOR_MovementThreshold& Threshold) const
{
	if (Component != Other->Component)
	{
		return 1.0f;
	}

	if (ParentIndex != Other->ParentIndex)
	{
		return 1.0f;
	}

	const float LinearSpeed = Other->Local.LinearVelocity.Size();
	const float AngularSpeed = Other->Local.AngularVelocity.Size();
	const float Cost = Local.Compare(Local, Threshold(LinearSpeed, AngularSpeed));
	if (Cost >= 1.0f)
	{
		return 1.0f;
	}
	
	const float Distance = (Local.LinearVelocity - Other->Local.LinearVelocity).SizeSquared() / (Threshold.LinearVelocity * Threshold.LinearVelocity);
	const float Angle = (Local.AngularVelocity - Other->Local.AngularVelocity).SizeSquared() / (Threshold.AngularVelocity * Threshold.AngularVelocity);
	return Cost + Distance + Angle;
}


void UTGOR_ParentModule::SetLerp(UTGOR_ParentModule* Other, float Alpha)
{
	if (Component != Other->Component || ParentIndex != Other->ParentIndex)
	{
		if (Alpha > 0.5f)
		{
			Component = Other->Component;
			ParentIndex = Other->ParentIndex;
			Local = Other->Local;
		}
	}
	else
	{
		Local.SetLerp(Other->Local, Alpha);
	}
	MarkDirty();
}

bool UTGOR_ParentModule::SetParent(UTGOR_MobilityComponent* Owner, UTGOR_MobilityComponent* Parent, int32 Index)
{
	if (Component != Parent || ParentIndex.Index != Index)
	{
		if (Owner->CanParent(Parent))
		{
			Component = Parent;
			ParentIndex = Index;
			MarkDirty();
		}
		else
		{
			Component = nullptr;
			ParentIndex = FTGOR_Index();
			MarkDirty();
			return false;
		}
	}
	return true;
}
