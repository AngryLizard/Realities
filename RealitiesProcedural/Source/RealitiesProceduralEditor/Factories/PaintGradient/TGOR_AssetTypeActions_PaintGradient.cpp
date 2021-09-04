
#include "TGOR_AssetTypeActions_PaintGradient.h"
#include "RealitiesProcedural/Textures/TGOR_PaintGradient.h"
#include "Interfaces/ITextureEditorModule.h"
#include "ThumbnailRendering/SceneThumbnailInfoWithPrimitive.h"

#define LOCTEXT_NAMESPACE "PaintGradient"

UClass* FTGOR_AssetTypeActions_PaintGradient::GetSupportedClass() const
{
	return UTGOR_PaintGradient::StaticClass();
}

void FTGOR_AssetTypeActions_PaintGradient::OpenAssetEditor(const TArray<UObject*>& InObjects, TSharedPtr<IToolkitHost> EditWithinLevelEditor)
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


UThumbnailInfo* FTGOR_AssetTypeActions_PaintGradient::GetThumbnailInfo(UObject* Asset) const
{
	UTGOR_PaintGradient* PaintGradient = CastChecked<UTGOR_PaintGradient>(Asset);
	UThumbnailInfo* ThumbnailInfo = PaintGradient->ThumbnailInfo;
	if (ThumbnailInfo == NULL)
	{
		ThumbnailInfo = NewObject<USceneThumbnailInfo>(PaintGradient, NAME_None, RF_Transactional);
		PaintGradient->ThumbnailInfo = ThumbnailInfo;
	}

	return ThumbnailInfo;
}

#undef LOCTEXT_NAMESPACE
