// The Gateway of Realities: Planes of Existence.


#include "TGOR_SingleFloatCustomisation.h"
#include "CoreSystem/Storage/TGOR_Package.h"
#include "Components/SkeletalMeshComponent.h"
#include "CustomisationSystem/Components/TGOR_CustomisationComponent.h"


UTGOR_SingleFloatCustomisation::UTGOR_SingleFloatCustomisation()
	: Super()
{
	PayloadSize = 1;

	PositiveParam = "Positive";
}

void UTGOR_SingleFloatCustomisation::Apply(UTGOR_CustomisationComponent* Master, const FTGOR_CustomisationPayload& Payload, UTGOR_Bodypart* Bodypart, int32 NodeIndex, UMaterialInstanceDynamic* Instance, const FTGOR_CustomisationMeshSection& Allocation)
{
	float Value;
	FromPayload(Payload, Value);
	Value = FMath::Clamp(Value, 0.0f, 1.0f);
	
	// TODO: Define targets correctly for the mesh that matters (define a repo of morph-targets or sth)
	//Master->SetMorphTarget(PositiveParam, Value);
}

void UTGOR_SingleFloatCustomisation::Query(const UTGOR_CustomisationComponent* Master, FTGOR_CustomisationPayload& Payload, UTGOR_Bodypart* Bodypart, int32 NodeIndex, UMaterialInstanceDynamic* Instance, const FTGOR_CustomisationMeshSection& Allocation) const
{
	// Get Material parameter
	ToPayload(Payload, +0.0f);
}

void UTGOR_SingleFloatCustomisation::Write(FTGOR_GroupWritePackage& Package, const FTGOR_CustomisationPayload& Payload)
{
	float Value;
	FromPayload(Payload, Value);
	Package.WriteEntry("Value", Value);
}

bool UTGOR_SingleFloatCustomisation::Read(FTGOR_GroupReadPackage& Package, FTGOR_CustomisationPayload& Payload) const
{
	float Value;
	Package.ReadEntry("Value", Value);
	ToPayload(Payload, Value);
	return true;
}

bool UTGOR_SingleFloatCustomisation::MergePayloads(const TArray<FTGOR_CustomisationPayload>& Payloads, FTGOR_CustomisationPayload& Merge)
{
	Super::MergePayloads(Payloads, Merge);

	float Sum = 0.0f;
	for (const FTGOR_CustomisationPayload& Payload : Payloads)
	{
		float Value = 0.0f;
		FromPayload(Payload, Value);
		Sum += Value;
	}

	ToPayload(Merge, Sum / Payloads.Num());
	return true;
}

void UTGOR_SingleFloatCustomisation::ToPayload(FTGOR_CustomisationPayload& Payload, float Value) const
{
	INIT_TO_PAYLOAD
	NORMAL_TO_PAYLOAD(Value)
}

void UTGOR_SingleFloatCustomisation::FromPayload(const FTGOR_CustomisationPayload& Payload, float& Value)
{
	INIT_FROM_PAYLOAD
	NORMAL_FROM_PAYLOAD(Value)
}


