// The Gateway of Realities: Planes of Existence.


#include "TGOR_DoubleFloatCustomisation.h"
#include "Components/SkeletalMeshComponent.h"
#include "Realities/Utility/Storage/TGOR_Package.h"


UTGOR_DoubleFloatCustomisation::UTGOR_DoubleFloatCustomisation()
	: Super()
{
	PayloadSize = 1;

	NegativeParam = "Negative";
	PositiveParam = "Positive";

}

void UTGOR_DoubleFloatCustomisation::Apply(UTGOR_ModularSkeletalMeshComponent* Master, const TArray<uint8>& Payload, const FName& Param, UTGOR_Bodypart* Bodypart, USkeletalMeshComponent* MeshComponent, UMaterialInstanceDynamic* Instance)
{
	float Value;
	FromPayload(Payload, Value);
	Value = FMath::Clamp(Value, -1.0f, 1.0f);

	if (Value < 0.0f)
	{
		MeshComponent->SetMorphTarget(NegativeParam, -Value);
		MeshComponent->SetMorphTarget(PositiveParam, 0.0f);
	}
	else
	{
		MeshComponent->SetMorphTarget(NegativeParam, 0.0f);
		MeshComponent->SetMorphTarget(PositiveParam, Value);
	}
}

void UTGOR_DoubleFloatCustomisation::Query(UTGOR_ModularSkeletalMeshComponent* Master, TArray<uint8>& Payload, const FName& Param, UTGOR_Bodypart* Bodypart, USkeletalMeshComponent* MeshComponent, UMaterialInstanceDynamic* Instance) const
{
	// Get Material parameter
	ToPayload(Payload, +0.0f);
}

void UTGOR_DoubleFloatCustomisation::Write(FTGOR_GroupWritePackage& Package, const TArray<uint8>& Payload)
{
	float Value;
	FromPayload(Payload, Value);
	Package.WriteEntry("Value", Value);
}

bool UTGOR_DoubleFloatCustomisation::Read(FTGOR_GroupReadPackage& Package, TArray<uint8>& Payload) const
{
	float Value;
	Package.ReadEntry("Value", Value);
	ToPayload(Payload, Value);
	return true;
}

void UTGOR_DoubleFloatCustomisation::ToPayload(TArray<uint8>& Payload, float Value) const
{
	INIT_TO_PAYLOAD
	NORMAL_TO_PAYLOAD(Value)
}

void UTGOR_DoubleFloatCustomisation::FromPayload(const TArray<uint8>& Payload, float& Value)
{
	INIT_FROM_PAYLOAD
	NORMAL_FROM_PAYLOAD(Value)
}


