// The Gateway of Realities: Planes of Existence.


#include "TGOR_ColorCustomisation.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "CoreSystem/Storage/TGOR_Package.h"
#include "CustomisationSystem/Components/TGOR_CustomisationComponent.h"

UTGOR_ColorCustomisation::UTGOR_ColorCustomisation()
	: Super()
{
	PayloadSize = 3;

	ColorParam = "Color";
}

void UTGOR_ColorCustomisation::Apply(UTGOR_CustomisationComponent* Master, const FTGOR_CustomisationPayload& Payload, UTGOR_Bodypart* Bodypart, int32 NodeIndex, UMaterialInstanceDynamic* Instance, const FTGOR_CustomisationMeshSection& Allocation)
{
	FColor Color;
	FromPayload(Payload, Color);

	// Set Material parameter
	Instance->SetVectorParameterValue(ColorParam, Color.ReinterpretAsLinear());
}

void UTGOR_ColorCustomisation::Query(const UTGOR_CustomisationComponent* Master, FTGOR_CustomisationPayload& Payload, UTGOR_Bodypart* Bodypart, int32 NodeIndex, UMaterialInstanceDynamic* Instance, const FTGOR_CustomisationMeshSection& Allocation) const
{
	// Get Material parameter
	FLinearColor Color;
	Instance->GetVectorParameterValue(ColorParam, Color);

	ToPayload(Payload, Color.ToFColor(false));
}

void UTGOR_ColorCustomisation::Write(FTGOR_GroupWritePackage& Package, const FTGOR_CustomisationPayload& Payload)
{
	FColor Color;
	FromPayload(Payload, Color);
	Package.WriteEntry("Color", Color);
}

bool UTGOR_ColorCustomisation::Read(FTGOR_GroupReadPackage& Package, FTGOR_CustomisationPayload& Payload) const
{
	FColor Color;
	Package.ReadEntry("Color", Color);
	ToPayload(Payload, Color);
	return true;
}

void UTGOR_ColorCustomisation::ToPayload(FTGOR_CustomisationPayload& Payload, const FColor& Color) const
{
	INIT_TO_PAYLOAD
	NORMAL_TO_PAYLOAD(Color.R)
	NORMAL_TO_PAYLOAD(Color.G)
	NORMAL_TO_PAYLOAD(Color.B)
}

void UTGOR_ColorCustomisation::FromPayload(const FTGOR_CustomisationPayload& Payload, FColor& Color)
{
	INIT_FROM_PAYLOAD
	NORMAL_FROM_PAYLOAD(Color.R)
	NORMAL_FROM_PAYLOAD(Color.G)
	NORMAL_FROM_PAYLOAD(Color.B)
}


