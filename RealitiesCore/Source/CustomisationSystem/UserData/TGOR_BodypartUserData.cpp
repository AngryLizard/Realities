
#include "TGOR_BodypartUserData.h"

#include "RealitiesUtility/Utility/TGOR_Error.h"

#include "Rendering/SkeletalMeshRenderData.h"
#include "Rendering/SkeletalMeshLODRenderData.h"

#if WITH_EDITOR
#include "Rendering/SkeletalMeshModel.h"
#endif // #if WITH_EDITOR


#define LOCTEXT_NAMESPACE "TGOR_BodypartUserData"

///////////////////////////////////////////////////////////////////////////////

UTGOR_BodypartUserData::UTGOR_BodypartUserData(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer),
	MatchThreshold(0.1f)
{
	FTGOR_BodypartPatchSection Section;
	Sections.Emplace(Section);

	BakeName = LOCTEXT("BakeNameNone", "Bake Bodypart Params");
	BakeDescription = LOCTEXT("BakeDescriptionNone", "Bake triangles for this mesh. Adapt vertex normals to reference mesh.");
}

void UTGOR_BodypartUserData::Draw(class FPrimitiveDrawInterface* PDI, const class FSceneView* View) const
{
	Super::Draw(PDI, View);
}

FIntPoint UTGOR_BodypartUserData::GetTextureSize(int32 SectionIndex) const
{
	if (Sections.IsValidIndex(SectionIndex))
	{
		return FIntPoint(Sections[SectionIndex].SizeX, Sections[SectionIndex].SizeY);
	}
	return Super::GetTextureSize(SectionIndex);
}

FString UTGOR_BodypartUserData::GetTextureName(int32 SectionIndex) const
{
	return FString("Bodypart_") + Super::GetTextureName(SectionIndex);
}

void UTGOR_BodypartUserData::BakeData(USkeletalMesh* SourceMesh)
{
	Super::BakeData(SourceMesh);
	if (IsValid(SourceMesh))
	{
		//////////////////////////////////////////////////////////////////////////////////////////////

		// Source
		FSkeletalMeshRenderData* SourceMeshRenderData = SourceMesh->GetResourceForRendering();
		FSkeletalMeshLODRenderData& SourceMergeLODData = SourceMeshRenderData->LODRenderData[0];
		const uint32 SourceVertexNum = SourceMergeLODData.StaticVertexBuffers.PositionVertexBuffer.GetNumVertices();
		const uint32 SourceSectionNum = FMath::Min(Sections.Num(), SourceMergeLODData.RenderSections.Num());

		TArray<uint32> SourceIndices;
		SourceMergeLODData.MultiSizeIndexContainer.GetIndexBuffer(SourceIndices);
		const uint32 SourceTriangleNum = SourceIndices.Num() / 3;
		//

		// Collect extension meshes
		TSet<USkeletalMesh*> ExtensionMeshes;
		for (uint32 SourceSectionIndex = 0; SourceSectionIndex < SourceSectionNum; SourceSectionIndex++)
		{
			FTGOR_BodypartPatchSection& Section = Sections[SourceSectionIndex];
			for (const FTGOR_BodypartPatchExtension& Extension : Section.Extensions)
			{
				ExtensionMeshes.Emplace(Extension.Mesh);
			}
		}

		// Match normals of overlapping vertices
		MatchCount = 0;

#if WITH_EDITOR
		FSkeletalMeshModel* SourceImportedMesh = SourceMesh->GetImportedModel();
		for (USkeletalMesh* ExtensionMesh : ExtensionMeshes)
		{

			FSkeletalMeshModel* TargetImportedMesh = ExtensionMesh->GetImportedModel();
			const int32 LODNum = FMath::Min(SourceImportedMesh->LODModels.Num(), TargetImportedMesh->LODModels.Num());

			for (int32 LODIndex = 0; LODIndex < LODNum; LODIndex++)
			{
				FSkeletalMeshLODModel& SourceMatchLODData = SourceImportedMesh->LODModels[LODIndex];
				const FSkeletalMeshLODModel& TargetMatchLODData = TargetImportedMesh->LODModels[LODIndex];
				const uint32 MatchSectionNum = FMath::Min(SourceMatchLODData.Sections.Num(), TargetMatchLODData.Sections.Num());

				for (uint32 SectionIndex = 0; SectionIndex < MatchSectionNum; SectionIndex++)
				{
					FSkelMeshSection& SourceMatchSection = SourceMatchLODData.Sections[SectionIndex];
					const uint32 SourceMatchVertexNum = SourceMatchSection.SoftVertices.Num();

					const FSkelMeshSection& TargetMatchSection = TargetMatchLODData.Sections[SectionIndex];
					const uint32 TargetMatchVertexNum = TargetMatchSection.SoftVertices.Num();

					for (uint32 SourceVertexIndex = 0; SourceVertexIndex < SourceMatchVertexNum; SourceVertexIndex++)
					{
						FSoftSkinVertex& SourceVertex = SourceMatchSection.SoftVertices[SourceVertexIndex];
						for (uint32 TargetVertexIndex = 0; TargetVertexIndex < TargetMatchVertexNum; TargetVertexIndex++)
						{
							const FSoftSkinVertex& TargetVertex = TargetMatchSection.SoftVertices[TargetVertexIndex];
							if ((SourceVertex.Position - TargetVertex.Position).SizeSquared() < MatchThreshold * MatchThreshold)
							{
								const FVector TargetTangentX = FVector::VectorPlaneProject(SourceVertex.TangentX, TargetVertex.TangentZ);
								FVector TargetTangentY = (TargetVertex.TangentZ ^ TargetTangentX).GetSafeNormal();
								if ((TargetTangentY | SourceVertex.TangentY) < 0.0f)
								{
									TargetTangentY = -TargetTangentY;
								}

								SourceVertex.TangentX = TargetTangentX;
								SourceVertex.TangentY = TargetTangentY;
								SourceVertex.TangentZ = TargetVertex.TangentZ;
								MatchCount++;
							}
						}
					}
				}
			}
		}
#endif // #if WITH_EDITOR



		RenderSections.SetNum(SourceSectionNum);
		for (uint32 SourceSectionIndex = 0; SourceSectionIndex < SourceSectionNum; SourceSectionIndex++)
		{
			FTGOR_BodypartPatchSection& Section = Sections[SourceSectionIndex];
			FTGOR_RenderGeometrySection& RenderSection = RenderSections[SourceSectionIndex];

			//////////////////////////////////////////////////////////////////////////////////////////////

			RenderSection.Mapping.Empty();
			RenderSection.Triangles.Empty();
			RenderSection.TriangleCount = 0;

			//////////////////////////////////////////////////////////////////////////////////////////////

			auto PopulateVertex = [&](uint32 VertexIndex, const FSkeletalMeshLODRenderData& MergeData, FTGOR_MergeUVVert& W)
			{
				const uint32 UVNum = MergeData.GetNumTexCoords();
				W.Pos = MergeData.StaticVertexBuffers.PositionVertexBuffer.VertexPosition(VertexIndex); // FVector(UV, 0.0f); //
				if (UVNum > 0) W.UV[0] = MergeData.StaticVertexBuffers.StaticMeshVertexBuffer.GetVertexUV(VertexIndex, 0); else W.UV[0] = FVector2D::ZeroVector;
				if (UVNum > 1) W.UV[1] = MergeData.StaticVertexBuffers.StaticMeshVertexBuffer.GetVertexUV(VertexIndex, 1); else W.UV[1] = FVector2D::ZeroVector;
				if (UVNum > 2) W.UV[2] = MergeData.StaticVertexBuffers.StaticMeshVertexBuffer.GetVertexUV(VertexIndex, 2); else W.UV[2] = FVector2D::ZeroVector;
				W.Color = FLinearColor(0.0f, 0.0f, 0.0f, 0.0f);
				W.Tangent = MergeData.StaticVertexBuffers.StaticMeshVertexBuffer.VertexTangentX(VertexIndex);
				W.Bitangent = MergeData.StaticVertexBuffers.StaticMeshVertexBuffer.VertexTangentY(VertexIndex);
			};

			//////////////////////////////////////////////////////////////////////////////////////////////

			for (const FTGOR_BodypartPatchExtension& Extension : Section.Extensions)
			{
				const FSkeletalMeshRenderData* ExtensionMeshRenderData = Extension.Mesh->GetResourceForRendering();
				const FSkeletalMeshLODRenderData& ExtensionMergeLODData = ExtensionMeshRenderData->LODRenderData[0];
				const uint32 ExtensionVertexNum = ExtensionMergeLODData.StaticVertexBuffers.PositionVertexBuffer.GetNumVertices();
				const uint32 ExtensionSectionNum = ExtensionMergeLODData.RenderSections.Num();
				if (0 <= Extension.SectionIndex && Extension.SectionIndex < ExtensionSectionNum)
				{
					const uint32 ExtensionUVNum = ExtensionMergeLODData.GetNumTexCoords();
					if (0 <= Extension.UVChannel && Extension.UVChannel < ExtensionUVNum)
					{
						TArray<uint32> ExtensionIndices;
						ExtensionMergeLODData.MultiSizeIndexContainer.GetIndexBuffer(ExtensionIndices);
						const uint32 ExtensionTriangleNum = ExtensionIndices.Num() / 3;

						const FSkelMeshRenderSection& ExtensionRenderSection = ExtensionMergeLODData.RenderSections[Extension.SectionIndex];
						for (uint32 TriangleIndex = ExtensionRenderSection.BaseIndex; TriangleIndex < ExtensionRenderSection.BaseIndex + ExtensionRenderSection.NumTriangles; TriangleIndex++)
						{
							////////////
							auto CopyVertex = [&](uint32 Sub, FTGOR_MergeUVVert& W)
							{
								const uint32 VertexIndex = ExtensionIndices[TriangleIndex * 3 + Sub];
								PopulateVertex(VertexIndex, ExtensionMergeLODData, W);
							};
							////////////

							FTGOR_MergeUVTri Projection;
							CopyVertex(0, Projection.V0);
							CopyVertex(1, Projection.V1);
							CopyVertex(2, Projection.V2);
							RenderSection.Triangles.Emplace(Projection);
							RenderSection.TriangleCount++;
						}
					}

				}


			}

			//////////////////////////////////////////////////////////////////////////////////////////////

			FVector MinPos = FVector(FLT_MAX);
			FVector MaxPos = FVector(-FLT_MAX);

			const FSkelMeshRenderSection& SourceRenderSection = SourceMergeLODData.RenderSections[SourceSectionIndex];
			for (uint32 TriangleIndex = SourceRenderSection.BaseIndex; TriangleIndex < SourceRenderSection.BaseIndex + SourceRenderSection.NumTriangles; TriangleIndex++)
			{
				////////////
				auto CopyVertex = [&](uint32 Sub, FTGOR_MergeUVVert& W)
				{
					const uint32 VertexIndex = SourceIndices[TriangleIndex * 3 + Sub];

					PopulateVertex(VertexIndex, SourceMergeLODData, W);

					MinPos = MinPos.ComponentMin(W.Pos);
					MaxPos = MaxPos.ComponentMax(W.Pos);

					SourceMergeLODData.StaticVertexBuffers.PositionVertexBuffer.VertexPosition(VertexIndex);
				};
				////////////

				FTGOR_MergeUVTri Projection;
				CopyVertex(0, Projection.V0);
				CopyVertex(1, Projection.V1);
				CopyVertex(2, Projection.V2);
				const int32 Index = RenderSection.Triangles.Emplace(Projection);
				RenderSection.Mapping.Emplace(TriangleIndex, Index);
				RenderSection.TriangleCount++;
			}

			RenderSection.Extend = (MaxPos - MinPos) / 2;
			RenderSection.Center = (MaxPos + MinPos) / 2;


#if WITH_EDITOR
			if (CreateBakeAsset)
			{
				UWorld* World = GEditor->GetEditorWorldContext().World();
				EnsureVertexBake(World, SourceMesh, SourceSectionIndex, true);
			}
#endif
		}
	}
}

#undef LOCTEXT_NAMESPACE
