// The Gateway of Realities: Planes of Existence.


#include "TGOR_SliderCustomisation.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Realities/Utility/Storage/TGOR_Package.h"

UTGOR_SliderCustomisation::UTGOR_SliderCustomisation()
	: Super()
{
	PayloadSize = 1;
	SliderParam = "Value";
}

void UTGOR_SliderCustomisation::Apply(UTGOR_ModularSkeletalMeshComponent* Master, const TArray<uint8>& Payload, const FName& Param, UTGOR_Bodypart* Bodypart, USkeletalMeshComponent* MeshComponent, UMaterialInstanceDynamic* Instance)
{
	float Value;
	FromPayload(Payload, Value);

	// Set Material parameter
	Instance->SetScalarParameterValue(SliderParam, Value);
}

void UTGOR_SliderCustomisation::Query(UTGOR_ModularSkeletalMeshComponent* Master, TArray<uint8>& Payload, const FName& Param, UTGOR_Bodypart* Bodypart, USkeletalMeshComponent* MeshComponent, UMaterialInstanceDynamic* Instance) const
{
	// Get Material parameter
	float Value;
	Instance->GetScalarParameterValue(SliderParam, Value);

	ToPayload(Payload, Value);
}

void UTGOR_SliderCustomisation::Write(FTGOR_GroupWritePackage& Package, const TArray<uint8>& Payload)
{
	float Value;
	FromPayload(Payload, Value);
	Package.WriteEntry("Value", Value);
}

bool UTGOR_SliderCustomisation::Read(FTGOR_GroupReadPackage& Package, TArray<uint8>& Payload) const
{
	float Value;
	Package.ReadEntry("Value", Value);
	ToPayload(Payload, Value);
	return true;
}

void UTGOR_SliderCustomisation::ToPayload(TArray<uint8>& Payload, float Value) const
{
	INIT_TO_PAYLOAD
	NORMAL_TO_PAYLOAD(Value)
}

void UTGOR_SliderCustomisation::FromPayload(const TArray<uint8>& Payload, float& Value)
{
	INIT_FROM_PAYLOAD
	NORMAL_FROM_PAYLOAD(Value)
}
