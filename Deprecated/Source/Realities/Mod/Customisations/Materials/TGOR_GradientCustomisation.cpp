// The Gateway of Realities: Planes of Existence.


#include "TGOR_GradientCustomisation.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Realities/Utility/Storage/TGOR_Package.h"
#include "Realities/Components/Creature/TGOR_ModularSkeletalMeshComponent.h"

#include "Realities/Mod/Colours/TGOR_Colour.h"
#include "Realities/Base/Content/TGOR_ContentManager.h"
#include "Realities/Base/TGOR_Singleton.h"

UTGOR_GradientCustomisation::UTGOR_GradientCustomisation()
	: Super()
{
	PayloadSize = 4;

	ColourParam = "Colour";
	ValueXParam = "XBias";
	ValueYParam = "YBias";
}

bool UTGOR_GradientCustomisation::Validate(UTGOR_ModularSkeletalMeshComponent* Master, const TArray<uint8>& Payload, const FName& Param, UTGOR_Bodypart* Bodypart)
{
	float ValueX, ValueY;
	UTGOR_Colour* Content;
	FromPayload(Payload, Content, ValueX, ValueY);

	// Only go ahead if valid input
	if (IsValid(Content) && !Master->Execute_SupportsContent(Master, Content))
	{
		return false;
	}
	return true;
}

void UTGOR_GradientCustomisation::Apply(UTGOR_ModularSkeletalMeshComponent* Master, const TArray<uint8>& Payload, const FName& Param, UTGOR_Bodypart* Bodypart, USkeletalMeshComponent* MeshComponent, UMaterialInstanceDynamic* Instance)
{
	float ValueX, ValueY;
	UTGOR_Colour* Content;
	FromPayload(Payload, Content, ValueX, ValueY);

	// Only go ahead if valid input
	if (IsValid(Content) && IsValid(Content->Texture))
	{
		// Set material somehow
		Instance->SetTextureParameterValue(ColourParam, Content->Texture);

		// Set X-Bias parameter
		Instance->SetScalarParameterValue(ValueXParam, ValueX);

		// Set Y-Bias parameter
		Instance->SetScalarParameterValue(ValueYParam, ValueY);
}
	else
	{
		UTexture* Texture = nullptr;
		if (Instance->GetTextureParameterDefaultValue(ColourParam, Texture) && Texture)
		{
			Instance->SetTextureParameterValue(ColourParam, Texture);
		}
		if (Instance->GetScalarParameterDefaultValue(ValueXParam, ValueX))
		{ 
			Instance->SetScalarParameterValue(ValueXParam, ValueX);
		}
		if (Instance->GetScalarParameterDefaultValue(ValueYParam, ValueY))
		{
			Instance->SetScalarParameterValue(ValueYParam, ValueY);
		}
	}
}

void UTGOR_GradientCustomisation::Query(UTGOR_ModularSkeletalMeshComponent* Master, TArray<uint8>& Payload, const FName& Param, UTGOR_Bodypart* Bodypart, USkeletalMeshComponent* MeshComponent, UMaterialInstanceDynamic* Instance) const
{
	// Get Material parameter
	float ValueX, ValueY;
	Instance->GetScalarParameterValue(ValueXParam, ValueX);
	Instance->GetScalarParameterValue(ValueYParam, ValueY);

	ToPayload(Payload, nullptr, ValueX, ValueY);
}

void UTGOR_GradientCustomisation::Write(FTGOR_GroupWritePackage& Package, const TArray<uint8>& Payload)
{
	UTGOR_Colour* Content;
	float ValueX;
	float ValueY;
	FromPayload(Payload, Content, ValueX, ValueY);
	Package.WriteEntry("Content", Content);
	Package.WriteEntry("ValueX", ValueX);
	Package.WriteEntry("ValueY", ValueY);
}

bool UTGOR_GradientCustomisation::Read(FTGOR_GroupReadPackage& Package, TArray<uint8>& Payload) const
{
	UTGOR_Colour* Content;
	float ValueX;
	float ValueY;
	Package.ReadEntry("Content", Content);
	Package.ReadEntry("ValueX", ValueX);
	Package.ReadEntry("ValueY", ValueY);
	ToPayload(Payload, Content, ValueX, ValueY);
	return true;
}

void UTGOR_GradientCustomisation::ToPayload(TArray<uint8>& Payload, UTGOR_Colour* Content, float ValueX, float ValueY) const
{
	INIT_TO_PAYLOAD
	CONTENT_TO_PAYLOAD(Colour)
	NORMAL_TO_PAYLOAD(ValueX);
	NORMAL_TO_PAYLOAD(ValueY);
}

void UTGOR_GradientCustomisation::FromPayload(const TArray<uint8>& Payload, UTGOR_Colour*& Content, float& ValueX, float& ValueY)
{
	INIT_FROM_PAYLOAD
	CONTENT_FROM_PAYLOAD(Colour)
	NORMAL_FROM_PAYLOAD(ValueX);
	NORMAL_FROM_PAYLOAD(ValueY);
}