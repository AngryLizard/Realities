// The Gateway of Realities: Planes of Existence.


#include "TGOR_GlowCustomisation.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Realities/Utility/Storage/TGOR_Package.h"


UTGOR_GlowCustomisation::UTGOR_GlowCustomisation()
	: Super()
{
	PayloadSize = 2;

	ValueXParam = "XBias";
	ValueYParam = "YBias";
}

void UTGOR_GlowCustomisation::Apply(UTGOR_ModularSkeletalMeshComponent* Master, const TArray<uint8>& Payload, const FName& Param, UTGOR_Bodypart* Bodypart, USkeletalMeshComponent* MeshComponent, UMaterialInstanceDynamic* Instance)
{
	float ValueX, ValueY;
	FromPayload(Payload, ValueX, ValueY);

	// Set X-Bias parameter
	Instance->SetScalarParameterValue(ValueXParam, ValueX);

	// Set Y-Bias parameter
	Instance->SetScalarParameterValue(ValueYParam, ValueY);
}

void UTGOR_GlowCustomisation::Query(UTGOR_ModularSkeletalMeshComponent* Master, TArray<uint8>& Payload, const FName& Param, UTGOR_Bodypart* Bodypart, USkeletalMeshComponent* MeshComponent, UMaterialInstanceDynamic* Instance) const
{
	// Get Material parameter
	float ValueX, ValueY;
	Instance->GetScalarParameterValue(ValueXParam, ValueX);
	Instance->GetScalarParameterValue(ValueYParam, ValueY);

	ToPayload(Payload, ValueX, ValueY);
}

void UTGOR_GlowCustomisation::Write(FTGOR_GroupWritePackage& Package, const TArray<uint8>& Payload)
{
	float ValueX;
	float ValueY;
	FromPayload(Payload, ValueX, ValueY);
	Package.WriteEntry("ValueX", ValueX);
	Package.WriteEntry("ValueY", ValueY);
}

bool UTGOR_GlowCustomisation::Read(FTGOR_GroupReadPackage& Package, TArray<uint8>& Payload) const
{
	float ValueX;
	float ValueY;
	Package.ReadEntry("ValueX", ValueX);
	Package.ReadEntry("ValueY", ValueY);
	ToPayload(Payload, ValueX, ValueY);
	return true;
}

void UTGOR_GlowCustomisation::ToPayload(TArray<uint8>& Payload, float ValueX, float ValueY) const
{
	INIT_TO_PAYLOAD
	NORMAL_TO_PAYLOAD(ValueX)
	NORMAL_TO_PAYLOAD(ValueY)
}

void UTGOR_GlowCustomisation::FromPayload(const TArray<uint8>& Payload, float& ValueX, float& ValueY)
{
	INIT_FROM_PAYLOAD
	NORMAL_FROM_PAYLOAD(ValueX)
	NORMAL_FROM_PAYLOAD(ValueY)
}
