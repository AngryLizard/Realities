// The Gateway of Realities: Planes of Existence.


#include "TGOR_GradientCustomisation.h"
#include "CoreSystem/Storage/TGOR_Package.h"
#include "Materials/MaterialInstanceDynamic.h"

#include "RealitiesUGC/Mod/TGOR_ContentManager.h"
#include "CoreSystem/TGOR_Singleton.h"

#include "CustomisationSystem/Components/TGOR_CustomisationComponent.h"
#include "CustomisationSystem/Content/TGOR_Palette.h"
#include "CustomisationSystem/Content/TGOR_Bodypart.h"


UTGOR_GradientCustomisation::UTGOR_GradientCustomisation()
	: Super()
{
	PayloadSize = 4;

	PaletteParam = "Palette";
	ValueXParam = "XBias";
	ValueYParam = "YBias";
}

bool UTGOR_GradientCustomisation::Check(UTGOR_CustomisationComponent* Master, const FTGOR_CustomisationPayload& Payload, UTGOR_Bodypart* Bodypart)
{
	float ValueX, ValueY;
	UTGOR_Palette* Palette;
	FromPayload(Payload, Palette, ValueX, ValueY);

	return true;
}

void UTGOR_GradientCustomisation::Apply(UTGOR_CustomisationComponent* Master, const FTGOR_CustomisationPayload& Payload, UTGOR_Bodypart* Bodypart, int32 NodeIndex, UMaterialInstanceDynamic* Instance, const FTGOR_CustomisationMeshSection& Allocation)
{
	float ValueX, ValueY;
	UTGOR_Palette* Palette;
	FromPayload(Payload, Palette, ValueX, ValueY);

	// Only go ahead if valid input
	if (IsValid(Palette))
	{
		// Set material textures
		Instance->SetTextureParameterValue(PaletteParam, Palette->Color);
		Instance->SetTextureParameterValue(MaterialParam, Palette->Material);

		// Set X-Bias parameter
		Instance->SetScalarParameterValue(ValueXParam, ValueX);

		// Set Y-Bias parameter
		Instance->SetScalarParameterValue(ValueYParam, ValueY);
	}
	else
	{
		UTexture* Texture = nullptr;
		if (Instance->GetTextureParameterDefaultValue(PaletteParam, Texture) && Texture)
		{
			Instance->SetTextureParameterValue(PaletteParam, Texture);
		}
		if (Instance->GetTextureParameterDefaultValue(MaterialParam, Texture) && Texture)
		{
			Instance->SetTextureParameterValue(MaterialParam, Texture);
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

void UTGOR_GradientCustomisation::Query(const UTGOR_CustomisationComponent* Master, FTGOR_CustomisationPayload& Payload, UTGOR_Bodypart* Bodypart, int32 NodeIndex, UMaterialInstanceDynamic* Instance, const FTGOR_CustomisationMeshSection& Allocation) const
{
	// Get Material parameter
	float ValueX, ValueY;
	Instance->GetScalarParameterValue(ValueXParam, ValueX);
	Instance->GetScalarParameterValue(ValueYParam, ValueY);

	ToPayload(Payload, nullptr, ValueX, ValueY);
}

void UTGOR_GradientCustomisation::Write(FTGOR_GroupWritePackage& Package, const FTGOR_CustomisationPayload& Payload)
{
	UTGOR_Palette* Palette;
	float ValueX;
	float ValueY;
	FromPayload(Payload, Palette, ValueX, ValueY);
	Package.WriteEntry("Palette", Palette);
	Package.WriteEntry("ValueX", ValueX);
	Package.WriteEntry("ValueY", ValueY);
}

bool UTGOR_GradientCustomisation::Read(FTGOR_GroupReadPackage& Package, FTGOR_CustomisationPayload& Payload) const
{
	UTGOR_Palette* Palette;
	float ValueX;
	float ValueY;
	Package.ReadEntry("Palette", Palette);
	Package.ReadEntry("ValueX", ValueX);
	Package.ReadEntry("ValueY", ValueY);
	ToPayload(Payload, Palette, ValueX, ValueY);
	return true;
}

void UTGOR_GradientCustomisation::ToPayload(FTGOR_CustomisationPayload& Payload, UTGOR_Palette* Palette, float ValueX, float ValueY) const
{
	INIT_TO_PAYLOAD
	CONTENT_TO_PAYLOAD(Palette)
	NORMAL_TO_PAYLOAD(ValueX);
	NORMAL_TO_PAYLOAD(ValueY);
}

void UTGOR_GradientCustomisation::FromPayload(const FTGOR_CustomisationPayload& Payload, UTGOR_Palette*& Palette, float& ValueX, float& ValueY)
{
	INIT_FROM_PAYLOAD
	CONTENT_FROM_PAYLOAD(Palette)
	NORMAL_FROM_PAYLOAD(ValueX);
	NORMAL_FROM_PAYLOAD(ValueY);
}