// The Gateway of Realities: Planes of Existence.


#include "TGOR_SkinCustomisation.h"
#include "CoreSystem/Storage/TGOR_Package.h"
#include "Materials/MaterialInstanceDynamic.h"

#include "RealitiesUGC/Mod/TGOR_ContentManager.h"
#include "CoreSystem/TGOR_Singleton.h"

#include "CustomisationSystem/Components/TGOR_CustomisationComponent.h"
#include "CustomisationSystem/Content/TGOR_Skin.h"
#include "CustomisationSystem/Content/TGOR_Mask.h"
#include "CustomisationSystem/Content/TGOR_Palette.h"
#include "CustomisationSystem/Content/TGOR_Bodypart.h"
#include "CustomisationSystem/Content/TGOR_Canvas.h"

UTGOR_SkinCustomisation::UTGOR_SkinCustomisation()
	: Super()
{
	PayloadSize = 6;
}


bool UTGOR_SkinCustomisation::Check(UTGOR_CustomisationComponent* Master, const FTGOR_CustomisationPayload& Payload, UTGOR_Bodypart* Bodypart)
{
	UTGOR_Skin* Skin;
	UTGOR_Mask* Mask;
	UTGOR_Palette* Palette;
	FromPayload(Payload, Skin, Mask, Palette);

	return true;
}

void UTGOR_SkinCustomisation::Apply(UTGOR_CustomisationComponent* Master, const FTGOR_CustomisationPayload& Payload, UTGOR_Bodypart* Bodypart, int32 NodeIndex, UMaterialInstanceDynamic* Instance, const FTGOR_CustomisationMeshSection& Allocation)
{
	UTGOR_Skin* Skin;
	UTGOR_Mask* Mask;
	UTGOR_Palette* Palette;
	FromPayload(Payload, Skin, Mask, Palette);
	if (IsValid(Bodypart))
	{
		// Make sure skins are defined
		Palette = GetMandatoryPalette(Palette, Bodypart);

		// Make sure skin is allowed by bodypart
		Skin = GetMandatorySkin(Skin, Bodypart);
		if (IsValid(Skin))
		{
			Skin = Skin->GetBodypartSkin(Bodypart);
		}

		// Make sure mask is allowed by bodypart
		Mask = GetMandatoryMask(Mask, Bodypart);
		if (IsValid(Mask))
		{
			Mask = Mask->GetBodypartMask(Bodypart);
		}

		// Set all textures as instructed by skin resourceable
		for (const auto& MaterialPair : DynamicMaterials)
		{
			// Find all canvases that fit this request
			for (const auto& AllocatedPair : Allocation.Textures)
			{
				if (AllocatedPair.Key->IsA(MaterialPair.Key) && AllocatedPair.Key && IsValid(MaterialPair.Value))
				{
					if (IsValid(Skin))
					{
						MaterialPair.Value->SetTextureParameterValue(NormalName, Skin->Normal);
						MaterialPair.Value->SetTextureParameterValue(SurfaceName, Skin->Surface);
					}
					if (IsValid(Mask))
					{
						MaterialPair.Value->SetTextureParameterValue(MaskName, Mask->Texture);
					}
					if (IsValid(Palette))
					{
						MaterialPair.Value->SetTextureParameterValue(ColorName, Palette->Color);
						MaterialPair.Value->SetTextureParameterValue(MaterialName, Palette->Material);
					}

					if (FCanvas* Canvas = Master->CustomisationTextures[AllocatedPair.Key].Canvas)
					{
						FCanvasTileItem TileItem(AllocatedPair.Value.Pos, MaterialPair.Value->GetRenderProxy(), AllocatedPair.Value.Size, FVector2D(0.0f, 0.0f), FVector2D(1.0f, 1.0f));
						Canvas->DrawItem(TileItem);
					}
				}
			}
		}
	}
}

void UTGOR_SkinCustomisation::Query(const UTGOR_CustomisationComponent* Master, FTGOR_CustomisationPayload& Payload, UTGOR_Bodypart* Bodypart, int32 NodeIndex, UMaterialInstanceDynamic* Instance, const FTGOR_CustomisationMeshSection& Allocation) const
{
	ToPayload(Payload, nullptr, nullptr, nullptr);
}

void UTGOR_SkinCustomisation::Write(FTGOR_GroupWritePackage& Package, const FTGOR_CustomisationPayload& Payload)
{
	UTGOR_Skin* Skin;
	UTGOR_Mask* Mask;
	UTGOR_Palette* Palette;
	FromPayload(Payload, Skin, Mask, Palette);
	Package.WriteEntry("Skin", Skin);
	Package.WriteEntry("Mask", Mask);
	Package.WriteEntry("Palette", Palette);
}

bool UTGOR_SkinCustomisation::Read(FTGOR_GroupReadPackage& Package, FTGOR_CustomisationPayload& Payload) const
{
	UTGOR_Skin* Skin;
	UTGOR_Mask* Mask;
	UTGOR_Palette* Palette;
	Package.ReadEntry("Skin", Skin);
	Package.ReadEntry("Mask", Mask);
	Package.ReadEntry("Palette", Palette);
	ToPayload(Payload, Skin, Mask, Palette);
	return true;
}


void UTGOR_SkinCustomisation::ToPayload(FTGOR_CustomisationPayload& Payload, UTGOR_Skin* Skin, UTGOR_Mask* Mask, UTGOR_Palette* Palette) const
{
	INIT_TO_PAYLOAD;
	CONTENT_TO_PAYLOAD(Skin);
	CONTENT_TO_PAYLOAD(Mask);
	CONTENT_TO_PAYLOAD(Palette);
}

void UTGOR_SkinCustomisation::FromPayload(const FTGOR_CustomisationPayload& Payload, UTGOR_Skin*& Skin, UTGOR_Mask*& Mask, UTGOR_Palette*& Palette)
{
	INIT_FROM_PAYLOAD;
	CONTENT_FROM_PAYLOAD(Skin);
	CONTENT_FROM_PAYLOAD(Mask);
	CONTENT_FROM_PAYLOAD(Palette);
}