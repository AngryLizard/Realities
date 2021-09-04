// The Gateway of Realities: Planes of Existence.


#include "TGOR_GlowCustomisation.h"
#include "CoreSystem/Storage/TGOR_Package.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "CustomisationSystem/Components/TGOR_CustomisationComponent.h"


UTGOR_GlowCustomisation::UTGOR_GlowCustomisation()
	: Super()
{
	PayloadSize = 2;

	ValueXParam = "XBias";
	ValueYParam = "YBias";
}

void UTGOR_GlowCustomisation::Apply(UTGOR_CustomisationComponent* Master, const FTGOR_CustomisationPayload& Payload, UTGOR_Bodypart* Bodypart, int32 NodeIndex, UMaterialInstanceDynamic* Instance, const FTGOR_CustomisationMeshSection& Allocation)
{
	float ValueX, ValueY;
	FromPayload(Payload, ValueX, ValueY);

	// Set X-Bias parameter
	Instance->SetScalarParameterValue(ValueXParam, ValueX);

	// Set Y-Bias parameter
	Instance->SetScalarParameterValue(ValueYParam, ValueY);
}

void UTGOR_GlowCustomisation::Query(const UTGOR_CustomisationComponent* Master, FTGOR_CustomisationPayload& Payload, UTGOR_Bodypart* Bodypart, int32 NodeIndex, UMaterialInstanceDynamic* Instance, const FTGOR_CustomisationMeshSection& Allocation) const
{
	// Get Material parameter
	float ValueX, ValueY;
	Instance->GetScalarParameterValue(ValueXParam, ValueX);
	Instance->GetScalarParameterValue(ValueYParam, ValueY);

	ToPayload(Payload, ValueX, ValueY);
}

void UTGOR_GlowCustomisation::Write(FTGOR_GroupWritePackage& Package, const FTGOR_CustomisationPayload& Payload)
{
	float ValueX;
	float ValueY;
	FromPayload(Payload, ValueX, ValueY);
	Package.WriteEntry("ValueX", ValueX);
	Package.WriteEntry("ValueY", ValueY);
}

bool UTGOR_GlowCustomisation::Read(FTGOR_GroupReadPackage& Package, FTGOR_CustomisationPayload& Payload) const
{
	float ValueX;
	float ValueY;
	Package.ReadEntry("ValueX", ValueX);
	Package.ReadEntry("ValueY", ValueY);
	ToPayload(Payload, ValueX, ValueY);
	return true;
}

void UTGOR_GlowCustomisation::ToPayload(FTGOR_CustomisationPayload& Payload, float ValueX, float ValueY) const
{
	INIT_TO_PAYLOAD
	NORMAL_TO_PAYLOAD(ValueX)
	NORMAL_TO_PAYLOAD(ValueY)
}

void UTGOR_GlowCustomisation::FromPayload(const FTGOR_CustomisationPayload& Payload, float& ValueX, float& ValueY)
{
	INIT_FROM_PAYLOAD
	NORMAL_FROM_PAYLOAD(ValueX)
	NORMAL_FROM_PAYLOAD(ValueY)
}
