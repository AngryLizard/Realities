// The Gateway of Realities: Planes of Existence.


#include "TGOR_SurfaceCustomisation.h"
#include "CoreSystem/Storage/TGOR_Package.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "CustomisationSystem/Components/TGOR_CustomisationComponent.h"

#include "RealitiesUGC/Mod/TGOR_ContentManager.h"
#include "CoreSystem/TGOR_Singleton.h"

#include "CustomisationSystem/Content/TGOR_Canvas.h"
#include "CustomisationSystem/Content/TGOR_Skin.h"
#include "CustomisationSystem/Content/TGOR_Mask.h"
#include "CustomisationSystem/Content/TGOR_Palette.h"
#include "CustomisationSystem/Content/TGOR_Bodypart.h"


UTGOR_SurfaceCustomisation::UTGOR_SurfaceCustomisation()
	: Super(),
	NormalName("Normal"),
	SurfaceName("Surface"),
	MaskName("Mask"),
	ColorName("Color"),
	MaterialName("Material")
{
	PayloadSize = 6;

	SkinFilter = UTGOR_Skin::StaticClass();
	MaskFilter = UTGOR_Mask::StaticClass();
}

void UTGOR_SurfaceCustomisation::BuildResource()
{
	Super::BuildResource();

	DynamicMaterials.Empty();
	for (const auto& Pair : Materials)
	{
		if (IsValid(Pair.Value))
		{
			UMaterialInstanceDynamic*& Instance = DynamicMaterials.FindOrAdd(Pair.Key);
			Instance = UMaterialInstanceDynamic::Create(Pair.Value, this);
		}
	}
	
}

bool UTGOR_SurfaceCustomisation::ValidateMask(UTGOR_Mask* Mask, UTGOR_Bodypart* Bodypart) const
{
	if (IsValid(Mask))
	{
		if (!Mask->IsA(MaskFilter))
		{
			return false;
		}

		if (IsValid(Bodypart))
		{
			return Bodypart->Instanced_MaskInsertions.Contains(Mask);
		}
	}
	return true;
}

UTGOR_Mask* UTGOR_SurfaceCustomisation::GetMandatoryMask(UTGOR_Mask* Mask, UTGOR_Bodypart* Bodypart) const
{
	SINGLETON_RETCHK(nullptr);
	if (IsValid(Mask) && ValidateMask(Mask, Bodypart))
	{
		return Mask;
	}

	if (IsValid(Bodypart))
	{
		return Bodypart->Instanced_MaskInsertions.GetOfType<UTGOR_Mask>(MaskFilter);
	}

	UTGOR_ContentManager* ContentManager = Singleton->GetContentManager();
	return ContentManager->GetTFromType<UTGOR_Mask>(MaskFilter);
}


bool UTGOR_SurfaceCustomisation::ValidateSkin(UTGOR_Skin* Skin, UTGOR_Bodypart* Bodypart) const
{
	if (IsValid(Skin))
	{
		if (!Skin->IsA(SkinFilter))
		{
			return false;
		}

		if (IsValid(Bodypart))
		{
			return Bodypart->Instanced_SkinInsertions.Contains(Skin);
		}
	}
	return true;
}

UTGOR_Skin* UTGOR_SurfaceCustomisation::GetMandatorySkin(UTGOR_Skin* Skin, UTGOR_Bodypart* Bodypart) const
{
	SINGLETON_RETCHK(nullptr);
	if (IsValid(Skin) && ValidateSkin(Skin, Bodypart))
	{
		return Skin;
	}

	if (IsValid(Bodypart))
	{
		return Bodypart->Instanced_SkinInsertions.GetOfType<UTGOR_Skin>(SkinFilter);
	}

	UTGOR_ContentManager* ContentManager = Singleton->GetContentManager();
	return ContentManager->GetTFromType<UTGOR_Skin>(SkinFilter);
}


UTGOR_Palette* UTGOR_SurfaceCustomisation::GetMandatoryPalette(UTGOR_Palette* Palette, UTGOR_Bodypart* Bodypart) const
{
	SINGLETON_RETCHK(nullptr);
	if (IsValid(Palette))
	{
		return Palette;
	}

	if (IsValid(Bodypart) && IsValid(Bodypart->Instanced_DefaultPaletteInsertion.Collection))
	{
		return Bodypart->Instanced_DefaultPaletteInsertion.Collection;
	}

	UTGOR_ContentManager* ContentManager = Singleton->GetContentManager();
	return ContentManager->GetTFromType<UTGOR_Palette>();
}
