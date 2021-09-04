// The Gateway of Realities: Planes of Existence.


#include "TGOR_DimensionOwnerComponent.h"

#include "DimensionSystem/Data/TGOR_DimensionData.h"

#include "DimensionSystem/Components/TGOR_IdentityComponent.h"
#include "DimensionSystem/Data/TGOR_WorldData.h"

#include "CoreSystem/TGOR_Singleton.h"
#include "Net/UnrealNetwork.h"

UTGOR_DimensionOwnerComponent::UTGOR_DimensionOwnerComponent()
:	Super()
{
}

bool UTGOR_DimensionOwnerComponent::Assemble(UTGOR_DimensionData* Dimension)
{
	const bool Success = Super::Assemble(Dimension);
	DialOwner();

	return Success;
}

bool UTGOR_DimensionOwnerComponent::IsReady() const
{
	return !IsMidTeleport;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void UTGOR_DimensionOwnerComponent::DialOwner()
{
	SINGLETON_CHK;
	DIMENSIONBASE_CHK;

	UTGOR_WorldData* WorldData = Singleton->GetData<UTGOR_WorldData>();
	if (IsValid(WorldData))
	{
		FTGOR_DimensionIdentifier Identifier = WorldData->GetOwner(DimensionData->GetIdentifier());
		DimensionIdentifier = Identifier.DimensionIdentifier;
		ActorIdentifier = Identifier.ActorIdentifier;
	}
}
