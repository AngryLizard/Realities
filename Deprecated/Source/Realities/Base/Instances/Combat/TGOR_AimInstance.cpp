// The Gateway of Realities: Planes of Existence.

#include "TGOR_AimInstance.h"

#include "Realities/Base/TGOR_Singleton.h"
#include "Realities/Base/TGOR_GameInstance.h"
#include "Realities/Base/Content/TGOR_ContentManager.h"
#include "Realities/Components/Dimension/Interactable/TGOR_InteractableComponent.h"
#include "Realities/Components/System/Actions/TGOR_ActionComponent.h"
#include "Realities/Components/Combat/TGOR_AimComponent.h"
#include "Realities/Mod/Targets/TGOR_Target.h"
#include "Realities/Mod/Forces/TGOR_Force.h"
#include "Realities/Utility/Storage/TGOR_Package.h"

#include "Components/SceneComponent.h"


FTGOR_ForceInstance::FTGOR_ForceInstance()
{
}

FTGOR_ForceInstance FTGOR_ForceInstance::operator*(float Intensity) const
{
	FTGOR_ForceInstance Instance;
	for (const auto& Pair : Forces)
	{
		Instance.Forces.Emplace(Pair.Key, Pair.Value * Intensity);
	}
	return Instance;
}

void FTGOR_ForceInstance::Send(FTGOR_NetworkWritePackage& Package, UTGOR_Context* Context) const
{
	Package.WriteEntry(Forces);
}

void FTGOR_ForceInstance::Recv(FTGOR_NetworkReadPackage& Package, UTGOR_Context* Context)
{
	Package.ReadEntry(Forces);
}

SERIALISE_INIT_SOURCE_IMPLEMENT(FTGOR_ForceInstance);

////////////////////////////////////////////////////////////////////////////////////////////////////

FTGOR_InfluenceInstance::FTGOR_InfluenceInstance()
: Target(nullptr),
	Instigator(nullptr),
	Location(FVector::ZeroVector)
{
}

void FTGOR_InfluenceInstance::Send(FTGOR_NetworkWritePackage& Package, UTGOR_Context* Context) const
{
	Package.WriteEntry(Target);
	Package.WriteEntry(Forces);
	Package.WriteEntry(Instigator);
	Package.WriteEntry(Location);
}

void FTGOR_InfluenceInstance::Recv(FTGOR_NetworkReadPackage& Package, UTGOR_Context* Context)
{
	Package.ReadEntry(Target);
	Package.ReadEntry(Forces);
	Package.ReadEntry(Instigator);
	Package.ReadEntry(Location);
}

SERIALISE_INIT_SOURCE_IMPLEMENT(FTGOR_InfluenceInstance);

////////////////////////////////////////////////////////////////////////////////////////////////////

FTGOR_AimInstance::FTGOR_AimInstance()
:	Target(nullptr),
	Component(TWeakObjectPtr<USceneComponent>()),
	Index(-1),
	Offset(FVector::ZeroVector)
{
}

void FTGOR_AimInstance::Send(FTGOR_NetworkWritePackage& Package, UTGOR_Context* Context) const
{
	Package.WriteEntry(Component);
	Package.WriteEntry(Target);
	Package.WriteEntry(Offset);
	Package.WriteEntry(Index);
}

void FTGOR_AimInstance::Recv(FTGOR_NetworkReadPackage& Package, UTGOR_Context* Context)
{
	Package.ReadEntry(Component);
	Package.ReadEntry(Target);
	Package.ReadEntry(Offset);
	Package.ReadEntry(Index);
}

SERIALISE_INIT_SOURCE_IMPLEMENT(FTGOR_AimInstance);

////////////////////////////////////////////////////////////////////////////////////////////////////

FTGOR_AimPoint::FTGOR_AimPoint()
:	Target(nullptr),
	Component(TWeakObjectPtr<USceneComponent>()),
	Distance(0.0f),
	Center(FVector::ZeroVector)
{

}
