
#include "TGOR_PipelineModule.h"

#include "Realities/Components/Movement/TGOR_PipelineComponent.h"
#include "Realities/Base/Inventory/Modules/TGOR_Module.h"
#include "Realities/Base/Inventory/TGOR_Storage.h"
#include "Realities/Mod/Inputs/TGOR_Input.h"

#include "Realities/Utility/Storage/TGOR_Serialisation.h"
#include "Realities/Utility/Storage/TGOR_Package.h"
#include "Realities/Base/TGOR_Singleton.h"
#include "Realities/Base/TGOR_GameState.h"

#include "Realities/Base/System/Data/TGOR_WorldData.h"

////////////////////////////////////////////////////////////////////////////////////////////////////

UTGOR_PipelineModule::UTGOR_PipelineModule()
	: Super()
{
}

bool UTGOR_PipelineModule::Equals(const UTGOR_Module* Other) const
{
	if (!Super::Equals(Other))
	{
		return false;
	}

	const UTGOR_PipelineModule* OtherModule = static_cast<const UTGOR_PipelineModule*>(Other);
	return Component == OtherModule->Component && SocketDistance == OtherModule->SocketDistance && SocketSpeed == OtherModule->SocketSpeed;
}

bool UTGOR_PipelineModule::Merge(const UTGOR_Module* Other, ETGOR_NetvarEnumeration Priority)
{
	if (!Super::Merge(Other, Priority))
	{
		return false;
	}

	const UTGOR_PipelineModule* OtherModule = static_cast<const UTGOR_PipelineModule*>(Other);
	Component = OtherModule->Component;
	SocketDistance = OtherModule->SocketDistance;
	SocketSpeed = OtherModule->SocketSpeed;

	return true;
}

void UTGOR_PipelineModule::Write_Implementation(FTGOR_GroupWritePackage& Package, UTGOR_Singleton* Context) const
{
	Package.WriteEntry("Component", Component);
	Package.WriteEntry("Radius", SocketDistance);
	Package.WriteEntry("Speed", SocketSpeed);
	Super::Write_Implementation(Package, Context);
}

bool UTGOR_PipelineModule::Read_Implementation(FTGOR_GroupReadPackage& Package, UTGOR_Singleton* Context)
{
	Package.ReadEntry("Component", Component);
	Package.ReadEntry("Radius", SocketDistance);
	Package.ReadEntry("Speed", SocketSpeed);
	return Super::Read_Implementation(Package, Context);
}

void UTGOR_PipelineModule::Send_Implementation(FTGOR_NetworkWritePackage& Package, UTGOR_Singleton* Context) const
{
	Package.WriteEntry(Component);
	Package.WriteEntry(SocketDistance);
	Package.WriteEntry(SocketSpeed);
	Super::Send_Implementation(Package, Context);
}

bool UTGOR_PipelineModule::Recv_Implementation(FTGOR_NetworkReadPackage& Package, UTGOR_Singleton* Context)
{
	Package.ReadEntry(Component);
	Package.ReadEntry(SocketDistance);
	Package.ReadEntry(SocketSpeed);
	return Super::Recv_Implementation(Package, Context);
}


void UTGOR_PipelineModule::Clear()
{
	Super::Clear();
	Component.Reset();
	SocketDirection = 0.0f;
	SocketSpeed = 0.0f;
}

FString UTGOR_PipelineModule::Print() const
{
	return FString::Printf(TEXT("Direction = %s, %s"), (SocketDirection ? "->" : "<-"), *Super::Print());
}


float UTGOR_PipelineModule::GetDistance() const
{
	return SocketDistance;
}

float UTGOR_PipelineModule::GetSpeed() const
{
	return SocketSpeed;
}

UTGOR_PipelineComponent* UTGOR_PipelineModule::GetParent() const
{
	return Component.Get();
}

FTGOR_MovementDynamic UTGOR_PipelineModule::GetDynamic() const
{
	if (Component.IsValid())
	{
		USplineComponent* SplineComponent = Component->GetSpline();
		if (IsValid(SplineComponent))
		{
			const FQuat AngleRight(FVector::RightVector, (PI / 2));
			const FQuat AngleFlip(FVector::ForwardVector, (SocketDirection ? 0.0f : PI));

			FTGOR_MovementDynamic Dynamic;
			Dynamic.Linear = SplineComponent->GetLocationAtDistanceAlongSpline(SocketDistance, ESplineCoordinateSpace::Local);
			Dynamic.Angular = SplineComponent->GetQuaternionAtDistanceAlongSpline(SocketDistance, ESplineCoordinateSpace::Local) * AngleRight * AngleFlip;
			Dynamic.Angular.Normalize();

			const FVector Direction = SplineComponent->GetDirectionAtDistanceAlongSpline(SocketDistance, ESplineCoordinateSpace::Local);
			Dynamic.LinearVelocity = Direction * SocketSpeed;
			return Dynamic;
		}
	}
	return FTGOR_MovementDynamic();
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void UTGOR_PipelineModule::SetState(float Distance, float Speed)
{
	MarkDirty();
	SocketDistance = Distance;
	SocketSpeed = Speed;
}

void UTGOR_PipelineModule::SetParent(UTGOR_PipelineComponent* Parent)
{
	MarkDirty();
	Component = Parent;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void UTGOR_PipelineModule::SetSocketDirection(bool Direction)
{
	MarkDirty();
	SocketDirection = Direction;
}