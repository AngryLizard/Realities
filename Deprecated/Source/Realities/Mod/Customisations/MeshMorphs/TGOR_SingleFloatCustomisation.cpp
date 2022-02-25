// The Gateway of Realities: Planes of Existence.


#include "TGOR_SingleFloatCustomisation.h"
#include "Components/SkeletalMeshComponent.h"
#include "Realities/Utility/Storage/TGOR_Package.h"


UTGOR_SingleFloatCustomisation::UTGOR_SingleFloatCustomisation()
	: Super()
{
	PayloadSize = 1;

	PositiveParam = "Positive";
}

void UTGOR_SingleFloatCustomisation::Apply(UTGOR_ModularSkeletalMeshComponent* Master, const TArray<uint8>& Payload, const FName& Param, UTGOR_Bodypart* Bodypart, USkeletalMeshComponent* MeshComponent, UMaterialInstanceDynamic* Instance)
{
	float Value;
	FromPayload(Payload, Value);
	Value = FMath::Clamp(Value, 0.0f, 1.0f);

	MeshComponent->SetMorphTarget(PositiveParam, Value);
}

void UTGOR_SingleFloatCustomisation::Query(UTGOR_ModularSkeletalMeshComponent* Master, TArray<uint8>& Payload, const FName& Param, UTGOR_Bodypart* Bodypart, USkeletalMeshComponent* MeshComponent, UMaterialInstanceDynamic* Instance) const
{
	// Get Material parameter
	ToPayload(Payload, +0.0f);
}

void UTGOR_SingleFloatCustomisation::Write(FTGOR_GroupWritePackage& Package, const TArray<uint8>& Payload)
{
	float Value;
	FromPayload(Payload, Value);
	Package.WriteEntry("Value", Value);
}

bool UTGOR_SingleFloatCustomisation::Read(FTGOR_GroupReadPackage& Package, TArray<uint8>& Payload) const
{
	float Value;
	Package.ReadEntry("Value", Value);
	ToPayload(Payload, Value);
	return true;
}

void UTGOR_SingleFloatCustomisation::ToPayload(TArray<uint8>& Payload, float Value) const
{
	INIT_TO_PAYLOAD
	NORMAL_TO_PAYLOAD(Value)
}

void UTGOR_SingleFloatCustomisation::FromPayload(const TArray<uint8>& Payload, float& Value)
{
	INIT_FROM_PAYLOAD
	NORMAL_FROM_PAYLOAD(Value)
}


