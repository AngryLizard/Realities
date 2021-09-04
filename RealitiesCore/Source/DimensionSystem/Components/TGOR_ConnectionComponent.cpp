// The Gateway of Realities: Planes of Existence.


#include "TGOR_ConnectionComponent.h"
#include "DimensionSystem/Data/TGOR_DimensionData.h"


UTGOR_ConnectionComponent::UTGOR_ConnectionComponent()
:	Super(),
ConnectionName(),
IsPublic(true)
{
}

bool UTGOR_ConnectionComponent::PreAssemble(UTGOR_DimensionData* Dimension)
{
	if (!ConnectionName.IsNone())
	{
		Dimension->AddConnection(ConnectionName, this);
	}

	return(ITGOR_DimensionInterface::PreAssemble(Dimension));
}

FName UTGOR_ConnectionComponent::GetConnectionName() const
{
	return ConnectionName;
}
