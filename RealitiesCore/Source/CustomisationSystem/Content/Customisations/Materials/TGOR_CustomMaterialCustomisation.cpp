// The Gateway of Realities: Planes of Existence.


#include "TGOR_CustomMaterialCustomisation.h"
#include "CoreSystem/Storage/TGOR_Package.h"

#include "CustomisationSystem/Components/TGOR_CustomisationComponent.h"

UTGOR_CustomMaterialCustomisation::UTGOR_CustomMaterialCustomisation()
	: Super()
{

}

void UTGOR_CustomMaterialCustomisation::Apply(UTGOR_CustomisationComponent* Master, const FTGOR_CustomisationPayload& Payload, UTGOR_Bodypart* Bodypart, int32 NodeIndex, UMaterialInstanceDynamic* Instance, const FTGOR_CustomisationMeshSection& Allocation)
{
	ApplyMaterial(Master, Payload, NodeIndex, Instance);
}

void UTGOR_CustomMaterialCustomisation::Query(const UTGOR_CustomisationComponent* Master, FTGOR_CustomisationPayload& Payload, UTGOR_Bodypart* Bodypart, int32 NodeIndex, UMaterialInstanceDynamic* Instance, const FTGOR_CustomisationMeshSection& Allocation) const
{
	QueryMaterial(Master, Payload, NodeIndex, Instance);
}

void UTGOR_CustomMaterialCustomisation::Write(FTGOR_GroupWritePackage& Package, const FTGOR_CustomisationPayload& Payload)
{
	WriteMaterial(Package, Payload);
}

bool UTGOR_CustomMaterialCustomisation::Read(FTGOR_GroupReadPackage& Package, FTGOR_CustomisationPayload& Payload) const
{
	return ReadMaterial(Package, Payload);
}
