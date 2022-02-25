// The Gateway of Realities: Planes of Existence.


#include "TGOR_CustomMeshMorphCustomisation.h"


UTGOR_CustomMeshMorphCustomisation::UTGOR_CustomMeshMorphCustomisation()
	: Super()
{

}

void UTGOR_CustomMeshMorphCustomisation::Apply(UTGOR_ModularSkeletalMeshComponent* Master, const TArray<uint8>& Payload, const FName& Param, UTGOR_Bodypart* Bodypart, USkeletalMeshComponent* MeshComponent, UMaterialInstanceDynamic* Instance)
{
	ApplyMeshMorph(Master, Payload, Param, Bodypart, MeshComponent);
}

void UTGOR_CustomMeshMorphCustomisation::Query(UTGOR_ModularSkeletalMeshComponent* Master, TArray<uint8>& Payload, const FName& Param, UTGOR_Bodypart* Bodypart, USkeletalMeshComponent* MeshComponent, UMaterialInstanceDynamic* Instance) const
{
	QueryMeshMorph(Master, Payload, Bodypart, Instance);
}

void UTGOR_CustomMeshMorphCustomisation::Write(FTGOR_GroupWritePackage& Package, const TArray<uint8>& Payload)
{
	WriteMeshMorph(Package, Payload);
}

bool UTGOR_CustomMeshMorphCustomisation::Read(FTGOR_GroupReadPackage& Package, TArray<uint8>& Payload) const
{
	return ReadMeshMorph(Package, Payload);
}
