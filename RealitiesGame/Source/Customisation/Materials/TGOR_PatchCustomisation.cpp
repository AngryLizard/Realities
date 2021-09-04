// The Gateway of Realities: Planes of Existence.


#include "TGOR_PatchCustomisation.h"
#include "CoreSystem/Storage/TGOR_Package.h"
#include "Materials/MaterialInstanceDynamic.h"

#include "RealitiesUGC/Mod/TGOR_ContentManager.h"
#include "CoreSystem/TGOR_Singleton.h"
#include "CustomisationSystem/UserData/TGOR_BodypartUserData.h"
#include "CustomisationSystem/Components/TGOR_CustomisationComponent.h"
#include "CustomisationSystem/Content/TGOR_Skin.h"
#include "CustomisationSystem/Content/TGOR_Mask.h"
#include "CustomisationSystem/Content/TGOR_Palette.h"
#include "CustomisationSystem/Content/TGOR_Bodypart.h"
#include "CustomisationSystem/Content/TGOR_Canvas.h"


UTGOR_PatchCustomisation::UTGOR_PatchCustomisation()
	: Super(),

	ExtendName("Extend"),
	CenterName("Center"),
	LocationBakeName("Locations"),
	TangentBakeName("Tangents"),
	NormalBakeName("Normals"),

	LocationName("PatchLocation"),
	ForwardName("PatchForward"),
	RightName("PatchRight"),
	UpName("PatchUp"),
	ScaleName("PatchScale")
{
	PayloadSize = 34;
}

void UTGOR_PatchCustomisation::BuildResource()
{
	Super::BuildResource();
}

bool UTGOR_PatchCustomisation::Check(UTGOR_CustomisationComponent* Master, const FTGOR_CustomisationPayload& Payload, UTGOR_Bodypart* Bodypart)
{
	UTGOR_Skin* Skin;
	UTGOR_Mask* Mask;
	UTGOR_Palette* Palette;
	FTransform Transform;
	FromPayload(Payload, Transform, Skin, Mask, Palette);

	return true;
}

void UTGOR_PatchCustomisation::Apply(UTGOR_CustomisationComponent* Master, const FTGOR_CustomisationPayload& Payload, UTGOR_Bodypart* Bodypart, int32 NodeIndex, UMaterialInstanceDynamic* Instance, const FTGOR_CustomisationMeshSection& Allocation)
{
	UTGOR_Skin* Skin;
	UTGOR_Mask* Mask;
	UTGOR_Palette* Palette;
	FTransform Transform;
	FromPayload(Payload, Transform, Skin, Mask, Palette);
	if (IsValid(Bodypart))
	{
		// All skins/masks with the right filter are allowed
		Palette = GetMandatoryPalette(Palette, Bodypart);
		Skin = GetMandatorySkin(Skin, nullptr);
		Mask = GetMandatoryMask(Mask, nullptr);

		UTGOR_BodypartUserData* UserData = Bodypart->Mesh->GetAssetUserData<UTGOR_BodypartUserData>();
		if (IsValid(UserData) && IsValid(Skin) && IsValid(Mask) && IsValid(Palette))
		{
			// Set all textures as instructed by skin resourceable
			for (const auto& MaterialPair : DynamicMaterials)
			{
				// Find all canvases that fit this request
				for (const auto& AllocatedPair : Allocation.Textures)
				{
					if (AllocatedPair.Key->IsA(MaterialPair.Key) && IsValid(MaterialPair.Value))
					{
						const int32* SectionPtr = Bodypart->Instanced_SectionInsertions.Collection.Find(Instanced_SectionInsertion.Collection);
						if (SectionPtr)
						{
							MaterialPair.Value->SetTextureParameterValue(NormalName, Skin->Normal);
							MaterialPair.Value->SetTextureParameterValue(SurfaceName, Skin->Surface);
							MaterialPair.Value->SetTextureParameterValue(MaskName, Mask->Texture);
							MaterialPair.Value->SetTextureParameterValue(ColorName, Palette->Color);
							MaterialPair.Value->SetTextureParameterValue(MaterialName, Palette->Material);

							const FTGOR_RenderGeometrySection& Section = UserData->RenderSections[*SectionPtr];
							MaterialPair.Value->SetVectorParameterValue(ExtendName, Section.Extend);
							MaterialPair.Value->SetVectorParameterValue(CenterName, Section.Center);

							if (Section.Bakes.Contains(LocationBakeName) && Section.Bakes.Contains(TangentBakeName) && Section.Bakes.Contains(NormalBakeName))
							{
								MaterialPair.Value->SetTextureParameterValue(LocationBakeName, Section.Bakes[LocationBakeName]);
								MaterialPair.Value->SetTextureParameterValue(TangentBakeName, Section.Bakes[TangentBakeName]);
								MaterialPair.Value->SetTextureParameterValue(NormalBakeName, Section.Bakes[NormalBakeName]);
							}

							MaterialPair.Value->SetVectorParameterValue(LocationName, Transform.GetLocation());
							MaterialPair.Value->SetVectorParameterValue(ForwardName, Transform.TransformVectorNoScale(FVector::ForwardVector));
							MaterialPair.Value->SetVectorParameterValue(RightName, Transform.TransformVectorNoScale(FVector::RightVector));
							MaterialPair.Value->SetVectorParameterValue(UpName, Transform.TransformVectorNoScale(FVector::UpVector));
							MaterialPair.Value->SetScalarParameterValue(ScaleName, Transform.GetScale3D().GetMax());

							//const FVector2D Size = FVector2D(AllocatedPair.Value.RenderTarget->SizeX, AllocatedPair.Value.RenderTarget->SizeY);
							//FTGOR_TriangleItem TriangleItem(UserData, *SectionPtr, FTransform::Identity, MaterialPair.Value->GetRenderProxy());
							//AllocatedPair.Value.Canvas->DrawItem(TriangleItem);

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
	}
}

void UTGOR_PatchCustomisation::Query(const UTGOR_CustomisationComponent* Master, FTGOR_CustomisationPayload& Payload, UTGOR_Bodypart* Bodypart, int32 NodeIndex, UMaterialInstanceDynamic* Instance, const FTGOR_CustomisationMeshSection& Allocation) const
{
	ToPayload(Payload, FTransform::Identity, nullptr, nullptr, nullptr);
}

void UTGOR_PatchCustomisation::Write(FTGOR_GroupWritePackage& Package, const FTGOR_CustomisationPayload& Payload)
{
	UTGOR_Skin* Skin;
	UTGOR_Mask* Mask;
	UTGOR_Palette* Palette;
	FTransform Transform;
	FromPayload(Payload, Transform, Skin, Mask, Palette);
	Package.WriteEntry("Transform", Transform);
	Package.WriteEntry("Skin", Skin);
	Package.WriteEntry("Mask", Mask);
	Package.WriteEntry("Palette", Palette);
}

bool UTGOR_PatchCustomisation::Read(FTGOR_GroupReadPackage& Package, FTGOR_CustomisationPayload& Payload) const
{
	UTGOR_Skin* Skin;
	UTGOR_Mask* Mask;
	UTGOR_Palette* Palette;
	FTransform Transform;
	Package.ReadEntry("Transform", Transform);
	Package.ReadEntry("Skin", Skin);
	Package.ReadEntry("Mask", Mask);
	Package.ReadEntry("Palette", Palette);
	ToPayload(Payload, Transform, Skin, Mask, Palette);
	return true;
}


void UTGOR_PatchCustomisation::ToPayload(FTGOR_CustomisationPayload& Payload, const FTransform& Transform, UTGOR_Skin* Skin, UTGOR_Mask* Mask, UTGOR_Palette* Palette) const
{
	INIT_TO_PAYLOAD;

	const FVector Location = Transform.GetLocation();
	const FQuat Rotation = Transform.GetRotation();
	const float Scale = FMath::Loge(Transform.GetScale3D().GetMax());

	CONTENT_TO_PAYLOAD(Skin);
	CONTENT_TO_PAYLOAD(Mask);
	CONTENT_TO_PAYLOAD(Palette);

	FLOAT_TO_PAYLOAD(Location.X);
	FLOAT_TO_PAYLOAD(Location.Y);
	FLOAT_TO_PAYLOAD(Location.Z);

	FLOAT_TO_PAYLOAD(Rotation.X);
	FLOAT_TO_PAYLOAD(Rotation.Y);
	FLOAT_TO_PAYLOAD(Rotation.Z);

	FLOAT_TO_PAYLOAD(Scale);
}

void UTGOR_PatchCustomisation::FromPayload(const FTGOR_CustomisationPayload& Payload, FTransform& Transform, UTGOR_Skin*& Skin, UTGOR_Mask*& Mask, UTGOR_Palette*& Palette)
{
	INIT_FROM_PAYLOAD;

	FVector Location;
	FQuat Rotation;
	float Scale;

	CONTENT_FROM_PAYLOAD(Skin);
	CONTENT_FROM_PAYLOAD(Mask);
	CONTENT_FROM_PAYLOAD(Palette);

	FLOAT_FROM_PAYLOAD(Location.X);
	FLOAT_FROM_PAYLOAD(Location.Y);
	FLOAT_FROM_PAYLOAD(Location.Z);

	FLOAT_FROM_PAYLOAD(Rotation.X);
	FLOAT_FROM_PAYLOAD(Rotation.Y);
	FLOAT_FROM_PAYLOAD(Rotation.Z);

	FLOAT_FROM_PAYLOAD(Scale);

	const float SQ = FMath::Min((Rotation.X * Rotation.X + Rotation.Y * Rotation.Y + Rotation.Z * Rotation.Z), 1.0f);
	Rotation.W = FMath::Sqrt(1.0f - SQ);

	Transform.SetLocation(Location);
	Transform.SetRotation(Rotation);
	Transform.SetScale3D(FVector(FMath::Exp(Scale)));
}