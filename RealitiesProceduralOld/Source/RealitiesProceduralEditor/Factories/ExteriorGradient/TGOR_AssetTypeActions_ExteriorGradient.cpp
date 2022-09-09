
#include "TGOR_AssetTypeActions_ExteriorGradient.h"
#include "RealitiesProcedural/Textures/TGOR_ExteriorGradient.h"
#include "Interfaces/ITextureEditorModule.h"
#include "ThumbnailRendering/SceneThumbnailInfoWithPrimitive.h"

#define LOCTEXT_NAMESPACE "ExteriorGradient"

UClass* FTGOR_AssetTypeActions_ExteriorGradient::GetSupportedClass() const
{
	return UTGOR_ExteriorGradient::StaticClass();
}

void FTGOR_AssetTypeActions_ExteriorGradient::OpenAssetEditor(const TArray<UObject*>& InObjects, TSharedPtr<IToolkitHost> EditWithinLevelEditor)
{
	EToolkitMode::Type Mode = EditWithinLevelEditor.IsValid() ? EToolkitMode::WorldCentric : EToolkitMode::Standalone;

	for (auto ObjIt = InObjects.CreateConstIterator(); ObjIt; ++ObjIt)
	{
		auto Texture = Cast<UTexture>(*ObjIt);
		if (Texture != NULL)
		{
			ITextureEditorModule* TextureEditorModule = &FModuleManager::LoadModuleChecked<ITextureEditorModule>("TextureEditor");
			TextureEditorModule->CreateTextureEditor(Mode, EditWithinLevelEditor, Texture);
		}
	}
}


UThumbnailInfo* FTGOR_AssetTypeActions_ExteriorGradient::GetThumbnailInfo(UObject* Asset) const
{
	UTGOR_ExteriorGradient* ExteriorGradient = CastChecked<UTGOR_ExteriorGradient>(Asset);
	UThumbnailInfo* ThumbnailInfo = ExteriorGradient->ThumbnailInfo;
	if (ThumbnailInfo == NULL)
	{
		ThumbnailInfo = NewObject<USceneThumbnailInfo>(ExteriorGradient, NAME_None, RF_Transactional);
		ExteriorGradient->ThumbnailInfo = ThumbnailInfo;
	}

	return ThumbnailInfo;
}

#undef LOCTEXT_NAMESPACE
