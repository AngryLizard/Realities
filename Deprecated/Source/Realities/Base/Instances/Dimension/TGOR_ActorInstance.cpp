// The Gateway of Realities: Planes of Existence.


#include "TGOR_ActorInstance.h"

#include "Realities/Mod/Dimensions/TGOR_Dimension.h"
#include "Realities/Components/Dimension/TGOR_RegionComponent.h"
#include "Realities/Components/Dimension/TGOR_IdentityComponent.h"
#include "Realities/Base/System/Data/TGOR_DimensionData.h"
#include "Realities/Mod/Spawner/TGOR_Spawner.h"
#include "Realities/Base/TGOR_Singleton.h"
#include "Realities/Base/TGOR_GameInstance.h"
#include "Realities/Base/Content/TGOR_ContentManager.h"

#include "Realities/Utility/Storage/TGOR_Package.h"
#include "Realities/Utility/Storage/TGOR_File.h"

SERIALISE_INIT_SOURCE_IMPLEMENT(FTGOR_SpawnIdentity)

FTGOR_ActorState::FTGOR_ActorState()
:	Component(nullptr), 
	Cache(FVector2D::ZeroVector)
{
}

FTGOR_ActorState::FTGOR_ActorState(UTGOR_RegionComponent* Source)
	: Component(Source), Cache(FVector2D::ZeroVector)
{
	if (!IsValid(Component)) return;
	if (!IsValid(Component->GetOwner())) return;
	Cache = FVector2D(Component->GetOwner()->GetActorLocation());
}



FTGOR_SpawnInstance::FTGOR_SpawnInstance()
:	SpawnActor(nullptr),
	SpawnLocation(FVector::ZeroVector),
	SpawnRotation(FRotator::ZeroRotator),
	Identity(nullptr)
{
}

void FTGOR_SpawnInstance::Write(FTGOR_GroupWritePackage& Package, UTGOR_Singleton* Context) const
{
	Package.WriteEntry("SpawnActor", SpawnActor);
	Package.WriteEntry("SpawnLocation", SpawnLocation);
	Package.WriteEntry("SpawnRotation", SpawnRotation);
}

void FTGOR_SpawnInstance::Read(FTGOR_GroupReadPackage& Package, UTGOR_Singleton* Context)
{
	Package.ReadEntry("SpawnActor", SpawnActor);
	Package.ReadEntry("SpawnLocation", SpawnLocation);
	Package.ReadEntry("SpawnRotation", SpawnRotation);
}


FTGOR_SpawnIdentity::FTGOR_SpawnIdentity()
	: Spawner(nullptr),
	Dimension(nullptr),
	Identifier(-1)
{
}

void FTGOR_SpawnIdentity::Send(FTGOR_NetworkWritePackage& Package, UTGOR_Singleton* Context) const
{
	Package.WriteEntry(Spawner);
	Package.WriteEntry(Dimension);
	Package.WriteEntry(Identifier);
}

void FTGOR_SpawnIdentity::Recv(FTGOR_NetworkReadPackage& Package, UTGOR_Singleton* Context)
{
	Package.ReadEntry(Spawner);
	Package.ReadEntry(Dimension);
	Package.ReadEntry(Identifier);
}