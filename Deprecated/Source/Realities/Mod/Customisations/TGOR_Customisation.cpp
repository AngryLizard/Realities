// The Gateway of Realities: Planes of Existence.


#include "TGOR_Customisation.h"


UTGOR_Customisation::UTGOR_Customisation()
	: Super()
{

}

bool UTGOR_Customisation::Validate(UTGOR_ModularSkeletalMeshComponent* Master, const TArray<uint8>& Payload, const FName& Param, UTGOR_Bodypart* Bodypart)
{
	return true;
}

void UTGOR_Customisation::Apply(UTGOR_ModularSkeletalMeshComponent* Master, const TArray<uint8>& Payload, const FName& Param, UTGOR_Bodypart* Bodypart, USkeletalMeshComponent* MeshComponent, UMaterialInstanceDynamic* Instance)
{

}

void UTGOR_Customisation::Query(UTGOR_ModularSkeletalMeshComponent* Master, TArray<uint8>& Payload, const FName& Param, UTGOR_Bodypart* Bodypart, USkeletalMeshComponent* MeshComponent, UMaterialInstanceDynamic* Instance) const
{

}


void UTGOR_Customisation::Write(FTGOR_GroupWritePackage& Package, const TArray<uint8>& Payload)
{

}

bool UTGOR_Customisation::Read(FTGOR_GroupReadPackage& Package, TArray<uint8>& Payload) const
{
	return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

TArray<uint8> UTGOR_Customisation::CreatePayload() const
{
	TArray<uint8> Payload;
	Payload.SetNumZeroed(PayloadSize);
	return(Payload);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void UTGOR_Customisation::ByteToPayload(TArray<uint8>& Payload, int32 Offset, uint8 Value) const
{
	DataToPayload(Payload, Offset, &Value, 1);
}

void UTGOR_Customisation::WordToPayload(TArray<uint8>& Payload, int32 Offset, int32 Value) const
{
	DataToPayload(Payload, Offset, &Value, 2);
}

void UTGOR_Customisation::FloatToPayload(TArray<uint8>& Payload, int32 Offset, float Value) const
{
	DataToPayload(Payload, Offset, &Value, 4);
}

void UTGOR_Customisation::NormalToPayload(TArray<uint8>& Payload, int32 Offset, float Value) const
{
	uint8 Normal = (uint8)(FMath::Clamp((Value + 1.0f)/2, 0.0f, 1.0f) * 0xFF);
	ByteToPayload(Payload, Offset, Normal);
}


uint8 UTGOR_Customisation::ByteFromPayload(const TArray<uint8>& Payload, int32 Offset) const
{
	uint8 Value;
	DataFromPayload(Payload, Offset, &Value, 1);
	return(Value);
}

int32 UTGOR_Customisation::WordFromPayload(const TArray<uint8>& Payload, int32 Offset) const
{
	int16 Value;
	DataFromPayload(Payload, Offset, &Value, 2);
	return(Value);
}

float UTGOR_Customisation::FloatFromPayload(const TArray<uint8>& Payload, int32 Offset) const
{
	float Value;
	DataFromPayload(Payload, Offset, &Value, 4);
	return(Value);
}

float UTGOR_Customisation::NormalFromPayload(const TArray<uint8>& Payload, int32 Offset) const
{
	uint8 Value = ByteFromPayload(Payload, Offset);
	return(FMath::Clamp(((float)Value) / 0xFF, 0.0f, 1.0f) * 2 - 1.0f);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void UTGOR_Customisation::DataToPayload(TArray<uint8>& Payload, int32 Offset, const void* Src, int32 Length) const
{
	if (Offset + Length > Payload.Num())
	{
		ERROR("Payload out of bounds", Error)
	}
	memcpy(Payload.GetData() + Offset, Src, Length);
}

void UTGOR_Customisation::DataFromPayload(const TArray<uint8>& Payload, int32 Offset, void* Dst, int32 Length) const
{
	if (Offset + Length > Payload.Num())
	{
		ERROR("Payload overflow", Error)
	}
	memcpy(Dst, Payload.GetData() + Offset, Length);
}