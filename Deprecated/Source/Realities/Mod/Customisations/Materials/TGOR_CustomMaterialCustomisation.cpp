// The Gateway of Realities: Planes of Existence.


#include "TGOR_CustomMaterialCustomisation.h"


UTGOR_CustomMaterialCustomisation::UTGOR_CustomMaterialCustomisation()
	: Super()
{

}

void UTGOR_CustomMaterialCustomisation::Apply(UTGOR_ModularSkeletalMeshComponent* Master, const TArray<uint8>& Payload, const FName& Param, UTGOR_Bodypart* Bodypart, USkeletalMeshComponent* MeshComponent, UMaterialInstanceDynamic* Instance)
{
	ApplyMaterial(Master, Payload, Bodypart, Instance);
}

void UTGOR_CustomMaterialCustomisation::Query(UTGOR_ModularSkeletalMeshComponent* Master, TArray<uint8>& Payload, const FName& Param, UTGOR_Bodypart* Bodypart, USkeletalMeshComponent* MeshComponent, UMaterialInstanceDynamic* Instance) const
{
	QueryMaterial(Master, Payload, Bodypart, Instance);
}

void UTGOR_CustomMaterialCustomisation::Write(FTGOR_GroupWritePackage& Package, const TArray<uint8>& Payload)
{
	WriteMaterial(Package, Payload);
}

bool UTGOR_CustomMaterialCustomisation::Read(FTGOR_GroupReadPackage& Package, TArray<uint8>& Payload) const
{
	return ReadMaterial(Package, Payload);
}
