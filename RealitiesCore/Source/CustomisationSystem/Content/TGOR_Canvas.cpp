// The Gateway of Realities: Planes of Existence.


#include "TGOR_Canvas.h"
#include "Materials/MaterialInstanceDynamic.h"

#include "CustomisationSystem/UserData/TGOR_TriangleRender.h"
#include "CustomisationSystem/UserData/TGOR_BodypartUserData.h"
#include "CustomisationSystem/UserData/TGOR_SkeletalMergeUserData.h"
#include "CustomisationSystem/Content/TGOR_Bodypart.h"
#include "CustomisationSystem/Content/TGOR_Section.h"


UTGOR_Canvas::UTGOR_Canvas()
	: Super(), 
	Format(ETextureRenderTargetFormat::RTF_RGBA8),
	CompressionSettings(TextureCompressionSettings::TC_Default),
	ParameterName("Texture"),
	ExportType(ETGOR_PBRExport::Emissive),
	BlendTextureName("Texture"),
	BlendTangentsName("Tangents"),
	BlendNormalsName("Normals"),
	BlendReferenceName("Reference"),
	BlendConvertName("Convert"),
	BlendMaskName("Mask"),
	BlendMaterial(nullptr)
{
}

void UTGOR_Canvas::BuildResource()
{
	Super::BuildResource();

	if (IsValid(BlendMaterial))
	{
		DynamicBlendMaterial = UMaterialInstanceDynamic::Create(BlendMaterial, this);
	}

	if (IsValid(BlendTextureMaterial))
	{
		DynamicBlendTextureMaterial = UMaterialInstanceDynamic::Create(BlendTextureMaterial, this);
	}
}


void UTGOR_Canvas::RenderBlend(UTGOR_SkeletalMergeUserData* MergeData, UTGOR_BodypartUserData* BodypartData, UTGOR_Bodypart* Bodypart, UTGOR_Section* Section, UTexture* Texture, const FTransform& Transform, FCanvas* Target)
{
	if (IsValid(MergeData) && IsValid(BodypartData))
	{
		const int32* SectionPtr = Bodypart->Instanced_SectionInsertions.Collection.Find(Section);
		if (SectionPtr && BodypartData->Sections.IsValidIndex(*SectionPtr) && BodypartData->RenderSections.IsValidIndex(*SectionPtr))
		{
			const FTGOR_RenderGeometrySection& RenderSection = BodypartData->RenderSections[*SectionPtr];
			const FTGOR_BodypartPatchSection& BodypartSection = BodypartData->Sections[*SectionPtr];
			if (IsValid(DynamicBlendMaterial) && IsValid(BodypartSection.BlendMask))
			{
				DynamicBlendMaterial->SetTextureParameterValue(BlendTextureName, Texture);
				DynamicBlendMaterial->SetTextureParameterValue(BlendMaskName, BodypartSection.BlendMask);

				if (RenderSection.Bakes.Contains(BlendTangentsName))
				{
					DynamicBlendTextureMaterial->SetTextureParameterValue(BlendTangentsName, RenderSection.Bakes[BlendTangentsName]);
				}
				if (RenderSection.Bakes.Contains(BlendNormalsName))
				{
					DynamicBlendTextureMaterial->SetTextureParameterValue(BlendNormalsName, RenderSection.Bakes[BlendNormalsName]);
				}
				if (RenderSection.Bakes.Contains(BlendReferenceName))
				{
					DynamicBlendTextureMaterial->SetTextureParameterValue(BlendReferenceName, RenderSection.Bakes[BlendReferenceName]);
				}

				FTGOR_TriangleItem TriangleItem(MergeData, *SectionPtr, Transform, DynamicBlendMaterial->GetRenderProxy());
				Target->DrawItem(TriangleItem);
			}
		}
	}
}

void UTGOR_Canvas::RenderBlendTexture(UTGOR_BodypartUserData* BodypartData, UTGOR_BodypartUserData* ParentData, UTGOR_Bodypart* Bodypart, UTGOR_Section* Section, UTexture* Texture, FCanvas* Target, const FVector2D& Pos, const FVector2D& Size)
{
	if (IsValid(DynamicBlendMaterial) && IsValid(BodypartData) && IsValid(ParentData))
	{
		const int32* SectionPtr = Bodypart->Instanced_SectionInsertions.Collection.Find(Section);
		if (SectionPtr && BodypartData->RenderSections.IsValidIndex(*SectionPtr) && ParentData->RenderSections.IsValidIndex(*SectionPtr) && ParentData->Sections.IsValidIndex(*SectionPtr))
		{
			//const FTGOR_SkeletalBlendSection& BlendSection = UserData->Sections[*SectionPtr];
			const FTGOR_RenderGeometrySection& RenderSection = BodypartData->RenderSections[*SectionPtr];
			const FTGOR_RenderGeometrySection& ParentRenderSection = ParentData->RenderSections[*SectionPtr];
			const FTGOR_BodypartPatchSection& ParentBodypartSection = ParentData->Sections[*SectionPtr];
			if (IsValid(DynamicBlendTextureMaterial) && IsValid(ParentBodypartSection.BlendMask))
			{

				DynamicBlendTextureMaterial->SetTextureParameterValue(BlendTextureName, Texture);
				DynamicBlendTextureMaterial->SetTextureParameterValue(BlendMaskName, ParentBodypartSection.BlendMask);

				if (RenderSection.Bakes.Contains(BlendTangentsName))
				{
					DynamicBlendTextureMaterial->SetTextureParameterValue(BlendTangentsName, RenderSection.Bakes[BlendTangentsName]);
				}
				if (RenderSection.Bakes.Contains(BlendNormalsName))
				{
					DynamicBlendTextureMaterial->SetTextureParameterValue(BlendNormalsName, RenderSection.Bakes[BlendNormalsName]);
				}
				if (RenderSection.Bakes.Contains(BlendReferenceName))
				{
					DynamicBlendTextureMaterial->SetTextureParameterValue(BlendReferenceName, RenderSection.Bakes[BlendReferenceName]);
				}
				if (ParentRenderSection.Bakes.Contains(BlendTangentsName))
				{
					DynamicBlendTextureMaterial->SetTextureParameterValue(BlendConvertName, ParentRenderSection.Bakes[BlendTangentsName]);
				}

				FCanvasTileItem TileItem(Pos, DynamicBlendTextureMaterial->GetRenderProxy(), Size, FVector2D(0.0f, 0.0f), FVector2D(1.0f, 1.0f));

				//FTGOR_TriangleItem TriangleItem(UserData, *SectionPtr, Transform, DynamicBlendMaterial->GetRenderProxy());
				Target->DrawItem(TileItem);
			}
		}
	}
}