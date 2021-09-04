// The Gateway of Realities: Planes of Existence.

#include "TGOR_Customisation.h"
#include "CoreSystem/Storage/TGOR_Package.h"

#include "CustomisationSystem/Content/TGOR_Section.h"

UTGOR_Customisation::UTGOR_Customisation()
	: Super(),
	LayerType(ETGOR_LayerTypeEnumeration::Optional),
	TrickleDown(true),
	PayloadSize(0)
{
}

bool UTGOR_Customisation::Check(UTGOR_CustomisationComponent* Master, const FTGOR_CustomisationPayload& Payload, UTGOR_Bodypart* Bodypart)
{
	return true;
}

void UTGOR_Customisation::Apply(UTGOR_CustomisationComponent* Master, const FTGOR_CustomisationPayload& Payload, UTGOR_Bodypart* Bodypart, int32 NodeIndex, UMaterialInstanceDynamic* Instance, const FTGOR_CustomisationMeshSection& Allocation)
{

}

void UTGOR_Customisation::Query(const UTGOR_CustomisationComponent* Master, FTGOR_CustomisationPayload& Payload, UTGOR_Bodypart* Bodypart, int32 NodeIndex, UMaterialInstanceDynamic* Instance, const FTGOR_CustomisationMeshSection& Allocation) const
{

}

void UTGOR_Customisation::Write(FTGOR_GroupWritePackage& Package, const FTGOR_CustomisationPayload& Payload)
{

}

bool UTGOR_Customisation::Read(FTGOR_GroupReadPackage& Package, FTGOR_CustomisationPayload& Payload) const
{
	return true;
}

bool UTGOR_Customisation::MergePayloads(const TArray<FTGOR_CustomisationPayload>& Payloads, FTGOR_CustomisationPayload& Payload)
{
	Payload = CreatePayload();
	return false;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

FTGOR_CustomisationPayload UTGOR_Customisation::CreatePayload() const
{
	FTGOR_CustomisationPayload Payload;
	Payload.Bytes.SetNumZeroed(PayloadSize);
	return Payload;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void UTGOR_Customisation::ByteToPayload(FTGOR_CustomisationPayload& Payload, int32 Offset, uint8 Value) const
{
	DataToPayload(Payload, Offset, &Value, 1);
}

void UTGOR_Customisation::WordToPayload(FTGOR_CustomisationPayload& Payload, int32 Offset, int32 Value) const
{
	DataToPayload(Payload, Offset, &Value, 2);
}

void UTGOR_Customisation::FloatToPayload(FTGOR_CustomisationPayload& Payload, int32 Offset, float Value) const
{
	DataToPayload(Payload, Offset, &Value, 4);
}

void UTGOR_Customisation::NormalToPayload(FTGOR_CustomisationPayload& Payload, int32 Offset, float Value) const
{
	uint8 Normal = (uint8)(FMath::Clamp((Value + 1.0f)/2, 0.0f, 1.0f) * 0xFF);
	ByteToPayload(Payload, Offset, Normal);
}


uint8 UTGOR_Customisation::ByteFromPayload(const FTGOR_CustomisationPayload& Payload, int32 Offset) const
{
	uint8 Value;
	DataFromPayload(Payload, Offset, &Value, 1);
	return(Value);
}

int32 UTGOR_Customisation::WordFromPayload(const FTGOR_CustomisationPayload& Payload, int32 Offset) const
{
	int16 Value;
	DataFromPayload(Payload, Offset, &Value, 2);
	return(Value);
}

float UTGOR_Customisation::FloatFromPayload(const FTGOR_CustomisationPayload& Payload, int32 Offset) const
{
	float Value;
	DataFromPayload(Payload, Offset, &Value, 4);
	return(Value);
}

float UTGOR_Customisation::NormalFromPayload(const FTGOR_CustomisationPayload& Payload, int32 Offset) const
{
	uint8 Value = ByteFromPayload(Payload, Offset);
	return(FMath::Clamp(((float)Value) / 0xFF, 0.0f, 1.0f) * 2 - 1.0f);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

ETextureRenderTargetFormat UTGOR_Customisation::GetRendertargetFormat(EPixelFormat PixelFormant) const
{
	return ETextureRenderTargetFormat::RTF_RGBA8;
	//if(PixelFormant == EPixelFormat::)
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void UTGOR_Customisation::DataToPayload(FTGOR_CustomisationPayload& Payload, int32 Offset, const void* Src, int32 Length) const
{
	if (Offset + Length > Payload.Bytes.Num())
	{
		ERROR("Payload out of bounds", Error)
	}
	memcpy(Payload.Bytes.GetData() + Offset, Src, Length);
}

void UTGOR_Customisation::DataFromPayload(const FTGOR_CustomisationPayload& Payload, int32 Offset, void* Dst, int32 Length) const
{
	if (Offset + Length > Payload.Bytes.Num())
	{
		ERROR("Payload overflow", Error)
	}
	memcpy(Dst, Payload.Bytes.GetData() + Offset, Length);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void UTGOR_Customisation::MoveInsertion(UTGOR_Content* Insertion, ETGOR_InsertionActionEnumeration Action, bool& Success)
{
	Super::MoveInsertion(Insertion, Action, Success);

	MOV_INSERTION(SectionInsertion);
}
