
#include "TGOR_SocketModule.h"

#include "Realities/Components/Movement/TGOR_SocketComponent.h"
#include "Realities/Base/Inventory/Modules/TGOR_Module.h"
#include "Realities/Base/Inventory/TGOR_Storage.h"
#include "Realities/Mod/Inputs/TGOR_Input.h"

#include "Realities/Utility/Storage/TGOR_Serialisation.h"
#include "Realities/Utility/Storage/TGOR_Package.h"
#include "Realities/Base/TGOR_Singleton.h"
#include "Realities/Base/TGOR_GameState.h"

#include "Realities/Base/System/Data/TGOR_WorldData.h"

////////////////////////////////////////////////////////////////////////////////////////////////////

UTGOR_SocketModule::UTGOR_SocketModule()
	: Super()
{
}

bool UTGOR_SocketModule::Equals(const UTGOR_Module* Other) const
{
	if (!Super::Equals(Other))
	{
		return false;
	}

	const UTGOR_SocketModule* OtherModule = static_cast<const UTGOR_SocketModule*>(Other);
	return Component == OtherModule->Component && Radius == OtherModule->Radius && Blend == OtherModule->Blend;
}

bool UTGOR_SocketModule::Merge(const UTGOR_Module* Other, ETGOR_NetvarEnumeration Priority)
{
	if (!Super::Merge(Other, Priority))
	{
		return false;
	}

	const UTGOR_SocketModule* OtherModule = static_cast<const UTGOR_SocketModule*>(Other);
	Component = OtherModule->Component;
	Radius = OtherModule->Radius;
	Blend = OtherModule->Blend;

	return true;
}

void UTGOR_SocketModule::Write_Implementation(FTGOR_GroupWritePackage& Package, UTGOR_Singleton* Context) const
{
	Package.WriteEntry("Component", Component);
	Package.WriteEntry("Radius", Radius);
	Package.WriteEntry("Blend", Blend);
	Super::Write_Implementation(Package, Context);
}

bool UTGOR_SocketModule::Read_Implementation(FTGOR_GroupReadPackage& Package, UTGOR_Singleton* Context)
{
	Package.ReadEntry("Component", Component);
	SetParent(Component.Get());

	Package.ReadEntry("Radius", Radius);
	Package.ReadEntry("Blend", Blend);
	return Super::Read_Implementation(Package, Context);
}

void UTGOR_SocketModule::Send_Implementation(FTGOR_NetworkWritePackage& Package, UTGOR_Singleton* Context) const
{
	Package.WriteEntry(Component);
	Package.WriteEntry(Radius);
	Package.WriteEntry(Blend);
	Super::Send_Implementation(Package, Context);
}

bool UTGOR_SocketModule::Recv_Implementation(FTGOR_NetworkReadPackage& Package, UTGOR_Singleton* Context)
{
	Package.ReadEntry(Component);
	SetParent(Component.Get());

	Package.ReadEntry(Radius);
	Package.ReadEntry(Blend);
	return Super::Recv_Implementation(Package, Context);
}


void UTGOR_SocketModule::Clear()
{
	Super::Clear();
	Cache.Reset();
	Component.Reset();
	Radius = 0.0f;
	Blend = 0.0f;
}

FString UTGOR_SocketModule::Print() const
{
	return FString::Printf(TEXT("Socket = %s, %s"), *Socket.ToString(), *Super::Print());
}

////////////////////////////////////////////////////////////////////////////////////////////////////

FTGOR_MovementPosition UTGOR_SocketModule::ComputePosition() const
{
	if (Component.IsValid())
	{
		const FTGOR_MovementPosition Position = Component->ComputePosition();
		const FTGOR_MovementPosition Part = Component->GetTransformOfSocket(Socket);

		USceneComponent* BaseComponent = Component->GetMovementBasis();
		const FTransform ParentTransform = BaseComponent->GetComponentTransform();

		FTGOR_MovementPosition Local;
		Local.Linear = ParentTransform.InverseTransformPositionNoScale(Part.Linear);
		Local.Angular = ParentTransform.InverseTransformRotation(Part.Angular);
		return Local.BaseToPosition(Position);
	}
	return FTGOR_MovementPosition();
}

FTGOR_MovementSpace UTGOR_SocketModule::ComputeSpace() const
{
	if (Component.IsValid())
	{
		FTGOR_MovementSpace Space = Component->ComputeSpace();
		const FTGOR_MovementPosition Part = Component->GetTransformOfSocket(Socket);

		USceneComponent* BaseComponent = Component->GetMovementBasis();
		const FTransform ParentTransform = BaseComponent->GetComponentTransform();

		FTGOR_MovementDynamic Local;
		Local.Linear = ParentTransform.InverseTransformPositionNoScale(Part.Linear);
		Local.Angular = ParentTransform.InverseTransformRotation(Part.Angular);
		return Local.BaseToSpace(Space);
	}
	return FTGOR_MovementSpace();
}

////////////////////////////////////////////////////////////////////////////////////////////////////

float UTGOR_SocketModule::Compare(UTGOR_SocketModule* Other, const FTGOR_MovementThreshold& Threshold) const
{
	if (Component != Other->Component)
	{
		return 1.0f;
	}
	return FMath::Square(Blend.Value - Other->Blend.Value);
}


void UTGOR_SocketModule::SetLerp(UTGOR_SocketModule* Other, float Alpha)
{
	if (Component != Other->Component)
	{
		if (Alpha > 0.5f)
		{
			SetParent(Other->Component.Get());
			Radius = Other->Radius;
			Blend = Other->Blend;
		}
	}
	else
	{
		Radius = FMath::Lerp(Radius, Other->Radius, Alpha);
		Blend = FMath::Lerp(Blend.Value, Other->Blend.Value, Alpha);
	}
	MarkDirty();
}

////////////////////////////////////////////////////////////////////////////////////////////////////

float UTGOR_SocketModule::GetRadius() const
{
	return Radius;
}

float UTGOR_SocketModule::GetBlend() const
{
	return Blend.Value;
}

UTGOR_SocketComponent* UTGOR_SocketModule::GetParent() const
{
	return Component.Get();
}

////////////////////////////////////////////////////////////////////////////////////////////////////


void UTGOR_SocketModule::RecachePrimitive()
{
	Cache.Reset();
	if (!Socket.IsNone() && Component.IsValid())
	{
		TArray<UPrimitiveComponent*> Primitives = Component->GetOwnerComponents<UPrimitiveComponent>();
		for (UPrimitiveComponent* Primitive : Primitives)
		{
			if (Primitive->DoesSocketExist(Socket))
			{
				Cache = Primitive;
			}
		}
	}
}

void UTGOR_SocketModule::SetRadius(float SocketRadius)
{
	Radius = SocketRadius;
	MarkDirty();
}
void UTGOR_SocketModule::SetBlend(float SocketBlend)
{
	Blend = FMath::Clamp(SocketBlend, 0.0f, 1.0f);
	MarkDirty();
}

void UTGOR_SocketModule::SetParent(UTGOR_SocketComponent* Parent)
{
	Component = Parent;
	RecachePrimitive();
	MarkDirty();
}

void UTGOR_SocketModule::SetSocketName(const FName& Name)
{
	Socket = Name;
	RecachePrimitive();
}