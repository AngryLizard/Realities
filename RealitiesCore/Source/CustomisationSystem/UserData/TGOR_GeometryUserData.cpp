
#include "TGOR_GeometryUserData.h"
#include "RealitiesUtility/Utility/TGOR_Error.h"

#include "Engine/Canvas.h"
#include "Kismet/KismetRenderingLibrary.h"
#include "Materials/MaterialInstanceDynamic.h"

#include "TGOR_TriangleRender.h"


#if WITH_EDITOR
#include "Dialogs/DlgPickAssetPath.h"
#include "AssetRegistryModule.h"
#include "AssetToolsModule.h"
#endif

#define LOCTEXT_NAMESPACE "TGOR_GeometryUserData"

///////////////////////////////////////////////////////////////////////////////

UTGOR_GeometryUserData::UTGOR_GeometryUserData(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	BakeName = LOCTEXT("BakeNameNone", "Bake None");
	BakeDescription = LOCTEXT("BakeDescriptionNone", "Standard user bake data.");
}

void UTGOR_GeometryUserData::Draw(class FPrimitiveDrawInterface* PDI, const class FSceneView* View) const
{
	Super::Draw(PDI, View);
}

FIntPoint UTGOR_GeometryUserData::GetTextureSize(int32 SectionIndex) const
{
	return FIntPoint(0, 0);
}

FString UTGOR_GeometryUserData::GetTextureName(int32 SectionIndex) const
{
	return FString::FromInt(SectionIndex);
}

void UTGOR_GeometryUserData::EnsureVertexBake(UObject* WorldContextObject, USkeletalMesh* SourceMesh, int32 SectionIndex, bool CreateAsset)
{
	if (RenderSections.IsValidIndex(SectionIndex))
	{
		FTGOR_RenderGeometrySection& Section = RenderSections[SectionIndex];

		FIntPoint Size = GetTextureSize(SectionIndex);
		const int32 Width = (1 << FMath::Max(Size.X + BakeScaleX, 1));
		const int32 Height = (1 << FMath::Max(Size.Y + BakeScaleY, 1));

		/// /////////////////////////////////////////////////////////

		auto GenerateRenderTarget = [&](const FTGOR_RenderGeometryBake& BakeMaterial, FName Key)->UTextureRenderTarget2D*
		{
			if (IsValid(BakeMaterial.BakeMaterial) && (!Section.Bakes.Contains(Key) || (CreateBakeAsset && CreateAsset)))
			{
				UTextureRenderTarget2D* RenderTarget = UKismetRenderingLibrary::CreateRenderTarget2D(WorldContextObject, Width, Height, ETextureRenderTargetFormat::RTF_RGBA16f, FLinearColor(0.0f, 0.0f, 0.0f, 0.0f), false);
				if (IsValid(RenderTarget))
				{
					UCanvas* Canvas;
					FVector2D Size;
					FDrawToRenderTargetContext Context;
					UKismetRenderingLibrary::BeginDrawCanvasToRenderTarget(WorldContextObject, RenderTarget, Canvas, Size, Context);

					UMaterialInstanceDynamic* Instance = UMaterialInstanceDynamic::Create(BakeMaterial.BakeMaterial, this);
					Instance->SetDoubleVectorParameterValue("Extend", Section.Extend);
					Instance->SetDoubleVectorParameterValue("Center", Section.Center);

					FTGOR_TriangleItem TriangleItem(this, SectionIndex, FTransform::Identity, Instance->GetRenderProxy());
					Canvas->DrawItem(TriangleItem);

					//Canvas->K2_DrawMaterialTriangle(Material, Triangles);
					UKismetRenderingLibrary::EndDrawCanvasToRenderTarget(WorldContextObject, Context);

					if (IsValid(BakeMaterial.PaddingMaterial) && CreateBakeAsset && CreateAsset)
					{
						UTextureRenderTarget2D* PaddingTarget = UKismetRenderingLibrary::CreateRenderTarget2D(WorldContextObject, Width, Height, ETextureRenderTargetFormat::RTF_RGBA16f, FLinearColor(0.0f, 0.0f, 0.0f, 0.0f), false);
						if (IsValid(PaddingTarget))
						{
							for (int32 PaddingIteration = 0; PaddingIteration < BakeMaterial.PaddingIterations; PaddingIteration++)
							{
								RenderTarget->Filter = TextureFilter::TF_Nearest;

								UCanvas* PaddingCanvas;
								FVector2D PaddingSize;
								FDrawToRenderTargetContext PaddingContext;
								UKismetRenderingLibrary::BeginDrawCanvasToRenderTarget(WorldContextObject, PaddingTarget, PaddingCanvas, PaddingSize, PaddingContext);

								UMaterialInstanceDynamic* PaddingInstance = UMaterialInstanceDynamic::Create(BakeMaterial.PaddingMaterial, this);
								PaddingInstance->SetTextureParameterValue("Texture", RenderTarget);
								FCanvasTileItem TileItem(FVector2D(0.0f, 0.0f), PaddingInstance->GetRenderProxy(), PaddingSize, FVector2D(0.0f, 0.0f), FVector2D(1.0f, 1.0f));
								Canvas->DrawItem(TileItem);

								UKismetRenderingLibrary::EndDrawCanvasToRenderTarget(WorldContextObject, PaddingContext);

								// Swap rendertargets
								UTextureRenderTarget2D* TempTarget = RenderTarget;
								RenderTarget = PaddingTarget;
								PaddingTarget = TempTarget;

								RenderTarget->Filter = TextureFilter::TF_Default;
							}
						}
					}

				}
				return RenderTarget;
			}
			return nullptr;
		};

		/// /////////////////////////////////////////////////////////

		// Create rendertargets where needed
		TMap<FName, UTextureRenderTarget2D*> RenderTargets;
		for (const auto& Pair : BakeMaterials)
		{
			UTextureRenderTarget2D* RenderTarget = GenerateRenderTarget(Pair.Value, Pair.Key);
			if (IsValid(RenderTarget))
			{
				RenderTargets.Emplace(Pair.Key, RenderTarget);
			}
		}


		// Check which bakes are textures that can be updated already
		TMap<FName, UTexture2D*> Bakes;
		for (const auto& Pair : BakeMaterials)
		{
			if (Section.Bakes.Contains(Pair.Key))
			{
				UTexture2D* Bake = Section.Bakes[Pair.Key];
				if (IsValid(Bake))
				{
					Bakes.Emplace(Pair.Key, Bake);
				}
			}
		}

		// Create new texture objects
		for (const auto& Pair : RenderTargets)
		{
			if (IsValid(Pair.Value) && !Bakes.Contains(Pair.Key))
			{
				FName TextureBakeName = FName(*("Texture_" + Pair.Key.ToString()));
				UTexture2D* Bake = NewObject<UTexture2D>(this, TextureBakeName);
				Bakes.Emplace(Pair.Key, Bake);
			}
		}

		if (RenderTargets.Num() > 0)
		{
			for (const auto& Pair : Bakes)
			{
				UTextureRenderTarget2D* RenderTarget = RenderTargets[Pair.Key];

				RenderTarget->UpdateTexture2D(Pair.Value, ETextureSourceFormat::TSF_RGBA16F);
				Section.Bakes.Emplace(Pair.Key, Pair.Value);

#if WITH_EDITOR
				Pair.Value->PostEditChange();
#endif
			}
		}

		/*
		if (CreateBakeAsset && CreateAsset && RenderTargets.Num() > 0)
		{

#if WITH_EDITOR

			// Check which bakes are textures that can be updated already
			TMap<FName, UTexture2D*> Bakes;
			for (const auto& Pair : BakeMaterials)
			{
				if (Section.Bakes.Contains(Pair.Key))
				{
					UTexture2D* Bake = Section.Bakes[Pair.Key];
					if (IsValid(Bake))
					{
						Bakes.Emplace(Pair.Key, Bake);
					}
				}
			}

			if (Bakes.Num() != RenderTargets.Num())
			{
				FString Name;
				FString PackageName = SourceMesh->GetPathName() + FString("_") + GetTextureName(SectionIndex) + TEXT("_VertexTexture");
				PackageName.ReplaceCharInline(L'.', L'_');
				FAssetToolsModule& AssetToolsModule = FModuleManager::LoadModuleChecked<FAssetToolsModule>("AssetTools");
				//AssetToolsModule.Get().CreateUniqueAssetName(PackageName, TEXT(""), PackageName, Name);

				// Assemble pathname
				int32 IndexOfLastSlash = INDEX_NONE;
				PackageName.FindLastChar('/', IndexOfLastSlash);
				FString PathName = PackageName.Mid(0, IndexOfLastSlash);
				FString FileName = PackageName.Mid(IndexOfLastSlash + 1);
				PackageName = PathName + "/Textures/" + FileName;

				// Ask user
				TSharedPtr<SDlgPickAssetPath> PickAssetPathWidget =
					SNew(SDlgPickAssetPath)
					.Title(LOCTEXT("Store texture to file", "Choose New Texture Location"))
					.DefaultAssetPath(FText::FromString(PackageName));

				if (PickAssetPathWidget->ShowModal() == EAppReturnType::Ok)
				{
					// Get the full name of where we want to create the physics asset.
					FString UserPackageName = PickAssetPathWidget->GetFullAssetPath().ToString();
					FString TextureName(*FPackageName::GetLongPackageAssetName(UserPackageName));

					// Check if the user inputed a valid asset name, if they did not, give it the generated default name
					if (TextureName.IsEmpty())
					{
						// Use the defaults that were already generated.
						UserPackageName = PackageName;
						TextureName = Name;
					}

					// Then find/create it.
					UPackage* Package = CreatePackage(*UserPackageName);
					check(Package);

					// Create new texture objects
					for (const auto& Pair : RenderTargets)
					{
						if (IsValid(Pair.Value) && !Bakes.Contains(Pair.Key))
						{
							FName TextureBakeName = FName(*(TextureName + "_" + Pair.Key.ToString()));
							UTexture2D* Bake = NewObject<UTexture2D>(Package, TextureBakeName, RF_Public | RF_Standalone);
							FAssetRegistryModule::AssetCreated(Bake);

							Bakes.Emplace(Pair.Key, Bake);
						}
					}
				}
				else
				{
					// Cancel
					return;
				}
			}

			for (const auto& Pair : Bakes)
			{
				UTextureRenderTarget2D* RenderTarget = RenderTargets[Pair.Key];

				RenderTarget->UpdateTexture2D(Pair.Value, ETextureSourceFormat::TSF_RGBA16F);
				Section.Bakes.Emplace(Pair.Key, Pair.Value);
				Pair.Value->PostEditChange();
				Pair.Value->MarkPackageDirty();
			}

			return;
#endif

		}

		// Assign rendertargets directly
		for (const auto& Pair : RenderTargets)
		{
			Section.Bakes.Emplace(Pair.Key, Pair.Value);
		}
	*/
	}
}

void UTGOR_GeometryUserData::BakeData(USkeletalMesh* SourceMesh)
{
#if WITH_EDITOR
	SourceMesh->PostEditChange();
	SourceMesh->MarkPackageDirty();
#endif
}

void UTGOR_GeometryUserData::TestBake(USkeletalMesh* SourceMesh)
{

}

#undef LOCTEXT_NAMESPACE
