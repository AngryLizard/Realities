// The Gateway of Realities: Planes of Existence.


#include "TGOR_NegativeFloatCustomisation.h"
#include "Components/SkeletalMeshComponent.h"
#include "Realities/Utility/Storage/TGOR_Package.h"


UTGOR_NegativeFloatCustomisation::UTGOR_NegativeFloatCustomisation()
	: Super()
{
	PayloadSize = 1;

	NegativeParam = "Negative";
}

void UTGOR_NegativeFloatCustomisation::Apply(UTGOR_ModularSkeletalMeshComponent* Master, const TArray<uint8>& Payload, const FName& Param, UTGOR_Bodypart* Bodypart, USkeletalMeshComponent* MeshComponent, UMaterialInstanceDynamic* Instance)
{
	float Value;
	FromPayload(Payload, Value);
	Value = FMath::Clamp(Value, -1.0f, 1.0f);

	MeshComponent->SetMorphTarget(NegativeParam, Value);
}

void UTGOR_NegativeFloatCustomisation::Query(UTGOR_ModularSkeletalMeshComponent* Master, TArray<uint8>& Payload, const FName& Param, UTGOR_Bodypart* Bodypart, USkeletalMeshComponent* MeshComponent, UMaterialInstanceDynamic* Instance) const
{
	// Get Material parameter
	ToPayload(Payload, +0.0f);
}

void UTGOR_NegativeFloatCustomisation::Write(FTGOR_GroupWritePackage& Package, const TArray<uint8>& Payload)
{
	float Value;
	FromPayload(Payload, Value);
	Package.WriteEntry("Value", Value);
}

bool UTGOR_NegativeFloatCustomisation::Read(FTGOR_GroupReadPackage& Package, TArray<uint8>& Payload) const
{
	float Value;
	Package.ReadEntry("Value", Value);
	ToPayload(Payload, Value);
	return true;
}

void UTGOR_NegativeFloatCustomisation::ToPayload(TArray<uint8>& Payload, float Value) const
{
	INIT_TO_PAYLOAD
	FLOAT_TO_PAYLOAD(Value)
}

void UTGOR_NegativeFloatCustomisation::FromPayload(const TArray<uint8>& Payload, float& Value)
{
	INIT_FROM_PAYLOAD
	FLOAT_FROM_PAYLOAD(Value)
}




