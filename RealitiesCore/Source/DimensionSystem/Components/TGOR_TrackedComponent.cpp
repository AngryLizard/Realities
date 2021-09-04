// The Gateway of Realities: Planes of Existence.

#include "TGOR_TrackedComponent.h"

#include "CoreSystem/TGOR_Singleton.h"
#include "DimensionSystem/Data/TGOR_WorldData.h"

#include "CoreSystem/Storage/TGOR_SaveInterface.h"
#include "CoreSystem/Storage/TGOR_Package.h"

#include "Net/UnrealNetwork.h"

UTGOR_TrackedComponent::UTGOR_TrackedComponent()
	: Super(),
	TrackedIdentity(INDEX_NONE)
{
	SetIsReplicatedByDefault(true);
}

void UTGOR_TrackedComponent::GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION(UTGOR_TrackedComponent, TrackedIdentity, COND_None);
}

bool UTGOR_TrackedComponent::PreAssemble(UTGOR_DimensionData* Dimension)
{
	Super::PreAssemble(Dimension);
	
	// Update tracked identifier in case we switched dimension
	SINGLETON_RETCHK(false);
	if (UTGOR_WorldData* WorldData = Singleton->GetData<UTGOR_WorldData>())
	{
		WorldData->RegisterTracked(this, false);
	}
	return true;
}

void UTGOR_TrackedComponent::Write_Implementation(FTGOR_GroupWritePackage& Package, UTGOR_Singleton* Context) const
{
	Super::Write_Implementation(Package, Context);
	Package.WriteEntry("TrackedIdentity", TrackedIdentity);
}

bool UTGOR_TrackedComponent::Read_Implementation(FTGOR_GroupReadPackage& Package, UTGOR_Singleton* Context)
{
	Super::Read_Implementation(Package, Context);
	Package.ReadEntry("TrackedIdentity", TrackedIdentity);
	return true;
}

int32 UTGOR_TrackedComponent::GetTrackedIdentifier() const
{
	return TrackedIdentity;
}
