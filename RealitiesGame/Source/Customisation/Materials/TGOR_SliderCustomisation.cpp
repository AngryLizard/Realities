// The Gateway of Realities: Planes of Existence.


#include "TGOR_SliderCustomisation.h"
#include "CoreSystem/Storage/TGOR_Package.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "CustomisationSystem/Components/TGOR_CustomisationComponent.h"

UTGOR_SliderCustomisation::UTGOR_SliderCustomisation()
	: Super()
{
	PayloadSize = 1;
	SliderParam = "Value";
}

void UTGOR_SliderCustomisation::Apply(UTGOR_CustomisationComponent* Master, const FTGOR_CustomisationPayload& Payload, UTGOR_Bodypart* Bodypart, int32 NodeIndex, UMaterialInstanceDynamic* Instance, const FTGOR_CustomisationMeshSection& Allocation)
{
	float Value;
	FromPayload(Payload, Value);

	// Set Material parameter
	Instance->SetScalarParameterValue(SliderParam, Value);
}

void UTGOR_SliderCustomisation::Query(const UTGOR_CustomisationComponent* Master, FTGOR_CustomisationPayload& Payload, UTGOR_Bodypart* Bodypart, int32 NodeIndex, UMaterialInstanceDynamic* Instance, const FTGOR_CustomisationMeshSection& Allocation) const
{
	// Get Material parameter
	float Value;
	Instance->GetScalarParameterValue(SliderParam, Value);

	ToPayload(Payload, Value);
}

void UTGOR_SliderCustomisation::Write(FTGOR_GroupWritePackage& Package, const FTGOR_CustomisationPayload& Payload)
{
	float Value;
	FromPayload(Payload, Value);
	Package.WriteEntry("Value", Value);
}

bool UTGOR_SliderCustomisation::Read(FTGOR_GroupReadPackage& Package, FTGOR_CustomisationPayload& Payload) const
{
	float Value;
	Package.ReadEntry("Value", Value);
	ToPayload(Payload, Value);
	return true;
}

void UTGOR_SliderCustomisation::ToPayload(FTGOR_CustomisationPayload& Payload, float Value) const
{
	INIT_TO_PAYLOAD
	NORMAL_TO_PAYLOAD(Value)
}

void UTGOR_SliderCustomisation::FromPayload(const FTGOR_CustomisationPayload& Payload, float& Value)
{
	INIT_FROM_PAYLOAD
	NORMAL_FROM_PAYLOAD(Value)
}
