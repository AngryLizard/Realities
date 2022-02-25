// The Gateway of Realities: Planes of Existence.


#include "TGOR_ColourCustomisation.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Realities/Utility/Storage/TGOR_Package.h"
#include "Realities/Components/Creature/TGOR_ModularSkeletalMeshComponent.h"

UTGOR_ColourCustomisation::UTGOR_ColourCustomisation()
	: Super()
{
	PayloadSize = 3;

	ColourParam = "Colour";
}

void UTGOR_ColourCustomisation::Apply(UTGOR_ModularSkeletalMeshComponent* Master, const TArray<uint8>& Payload, const FName& Param, UTGOR_Bodypart* Bodypart, USkeletalMeshComponent* MeshComponent, UMaterialInstanceDynamic* Instance)
{
	FColor Colour;
	FromPayload(Payload, Colour);

	// Set Material parameter
	Instance->SetVectorParameterValue(ColourParam, Colour.ReinterpretAsLinear());
}

void UTGOR_ColourCustomisation::Query(UTGOR_ModularSkeletalMeshComponent* Master, TArray<uint8>& Payload, const FName& Param, UTGOR_Bodypart* Bodypart, USkeletalMeshComponent* MeshComponent, UMaterialInstanceDynamic* Instance) const
{
	// Get Material parameter
	FLinearColor Colour;
	Instance->GetVectorParameterValue(ColourParam, Colour);

	ToPayload(Payload, Colour.ToFColor(false));
}

void UTGOR_ColourCustomisation::Write(FTGOR_GroupWritePackage& Package, const TArray<uint8>& Payload)
{
	FColor Colour;
	FromPayload(Payload, Colour);
	Package.WriteEntry("Colour", Colour);
}

bool UTGOR_ColourCustomisation::Read(FTGOR_GroupReadPackage& Package, TArray<uint8>& Payload) const
{
	FColor Colour;
	Package.ReadEntry("Colour", Colour);
	ToPayload(Payload, Colour);
	return true;
}

void UTGOR_ColourCustomisation::ToPayload(TArray<uint8>& Payload, const FColor& Colour) const
{
	INIT_TO_PAYLOAD
	NORMAL_TO_PAYLOAD(Colour.R)
	NORMAL_TO_PAYLOAD(Colour.G)
	NORMAL_TO_PAYLOAD(Colour.B)
}

void UTGOR_ColourCustomisation::FromPayload(const TArray<uint8>& Payload, FColor& Colour)
{
	INIT_FROM_PAYLOAD
	NORMAL_FROM_PAYLOAD(Colour.R)
	NORMAL_FROM_PAYLOAD(Colour.G)
	NORMAL_FROM_PAYLOAD(Colour.B)
}


