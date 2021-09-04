
#include "TGOR_SkeletalMergeUserData.h"
#include "RealitiesUtility/Utility/TGOR_Math.h"

#include "Rendering/SkeletalMeshRenderData.h"
#include "Rendering/SkeletalMeshLODRenderData.h"

#include "CustomisationSystem/UserData/TGOR_BodypartUserData.h"

#define LOCTEXT_NAMESPACE "TGOR_SkeletalMergeUserData"

///////////////////////////////////////////////////////////////////////////////

UTGOR_SkeletalMergeUserData::UTGOR_SkeletalMergeUserData(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	FTGOR_SkeletalMergeSection Section;
	Sections.Emplace(Section);

	BakeName = LOCTEXT("BakeNameNone", "Bake Merge Params");
	BakeDescription = LOCTEXT("BakeDescriptionNone", "Bake triangles to parent mesh for merge operations.");
}

void UTGOR_SkeletalMergeUserData::Draw(class FPrimitiveDrawInterface* PDI, const class FSceneView* View) const
{
	Super::Draw(PDI, View);

	//VPORT(View->ViewActor->GetName());
	//PDI->DrawLine;
}

FIntPoint UTGOR_SkeletalMergeUserData::GetTextureSize(int32 SectionIndex) const
{
	if (IsValid(ReferenceMesh))
	{
		UTGOR_BodypartUserData* BodypartUserData = ReferenceMesh->GetAssetUserData<UTGOR_BodypartUserData>();
		if (IsValid(BodypartUserData))
		{
			return BodypartUserData->GetTextureSize(SectionIndex);
		}
	}
	return Super::GetTextureSize(SectionIndex);
}

FString UTGOR_SkeletalMergeUserData::GetTextureName(int32 SectionIndex) const
{
	return FString("Merge_") + Super::GetTextureName(SectionIndex);
}

void UTGOR_SkeletalMergeUserData::BakeData(USkeletalMesh* SourceMesh)
{
	Super::BakeData(SourceMesh);

	if (IsValid(SourceMesh) && IsValid(ReferenceMesh))
	{
		//////////////////////////////////////////////////////////////////////////////////////////////

		int32 OverlapCount = 0;
		TMap<uint32, uint32> Overlaps;
		uint32 DuplicateCount = 0;
		TMap<uint32, uint32> Duplicates;
		int32 VertexCount = 0;
		TArray<FTGOR_SkeletalMergeVertex> Vertices;
		TMap<uint32, int32> VertexMapping;
		int32 TriangleCount = 0;
		TArray<int32> Triangles;
		TMap<uint32, int32> TriangleMapping;

		//////////////////////////////////////////////////////////////////////////////////////////////

		// Source
		const FSkeletalMeshRenderData* SourceMeshRenderData = SourceMesh->GetResourceForRendering();
		const FSkeletalMeshLODRenderData& SourceMergeLODData = SourceMeshRenderData->LODRenderData[0];
		const uint32 SourceVertexNum = SourceMergeLODData.StaticVertexBuffers.PositionVertexBuffer.GetNumVertices();
		const uint32 SourceSectionNum = FMath::Min(Sections.Num(), SourceMergeLODData.RenderSections.Num());
		const uint32 SourceUVNum = SourceMergeLODData.GetNumTexCoords();

		TArray<uint32> SourceIndices;
		SourceMergeLODData.MultiSizeIndexContainer.GetIndexBuffer(SourceIndices);
		const uint32 SourceTriangleNum = SourceIndices.Num() / 3;
		//

		// Reference
		const FSkeletalMeshRenderData* TargetMeshRenderData = ReferenceMesh->GetResourceForRendering();
		const FSkeletalMeshLODRenderData& TargetMergeLODData = TargetMeshRenderData->LODRenderData[0];
		const uint32 TargetVertexNum = TargetMergeLODData.StaticVertexBuffers.PositionVertexBuffer.GetNumVertices();
		//

		RenderSections.SetNum(SourceSectionNum);
		for (uint32 SourceSectionIndex = 0; SourceSectionIndex < SourceSectionNum; SourceSectionIndex++)
		{
			FTGOR_SkeletalMergeSection& Section = Sections[SourceSectionIndex];
			FTGOR_RenderGeometrySection& RenderSection = RenderSections[SourceSectionIndex];
			const FSkelMeshRenderSection& SourceRenderSection = SourceMergeLODData.RenderSections[SourceSectionIndex];

			//////////////////////////////////////////////////////////////////////////////////////////////

			RenderSection.Mapping.Empty();
			RenderSection.Triangles.Empty();
			RenderSection.TriangleCount = 0;

			//////////////////////////////////////////////////////////////////////////////////////////////

			Overlaps.Empty();
			OverlapCount = 0;

			Vertices.Empty();
			VertexMapping.Empty();
			VertexCount = 0;

			Duplicates.Empty();
			DuplicateCount = 0;

			Triangles.Empty();
			TriangleMapping.Empty();
			TriangleCount = 0;

			//////////////////////////////////////////////////////////////////////////////////////////////

			// Find vertex bridges
			if (Section.GenerateBridge)
			{
				for (uint32 SourceVertexIndex = SourceRenderSection.BaseVertexIndex; SourceVertexIndex < SourceRenderSection.BaseVertexIndex + SourceRenderSection.NumVertices; SourceVertexIndex++)
				{
					const FVector SourcePosition = SourceMergeLODData.StaticVertexBuffers.PositionVertexBuffer.VertexPosition(SourceVertexIndex);
					for (uint32 TargetVertexIndex = 0; TargetVertexIndex < TargetVertexNum; TargetVertexIndex++)
					{
						const FVector TargetPosition = TargetMergeLODData.StaticVertexBuffers.PositionVertexBuffer.VertexPosition(TargetVertexIndex);

						if ((SourcePosition - TargetPosition).SizeSquared() < Threshold * Threshold)
						{
							Overlaps.Emplace(SourceVertexIndex, TargetVertexIndex);
							OverlapCount++;
						}
					}
				}
			}

			// Find triangle mappings if UV channels are defined
			if (0 <= Section.SourceUVChannel && Section.SourceUVChannel < SourceUVNum && 0 <= Section.ReferenceUVChannel && Section.ReferenceUVChannel < SourceUVNum)
			{
				for (uint32 TriangleIndex = SourceRenderSection.BaseIndex; TriangleIndex < SourceRenderSection.BaseIndex + SourceRenderSection.NumTriangles; TriangleIndex++)
				{

					auto GenerateVertex = [&](uint32 Sub) -> int32 {

						const uint32 VertexIndex = SourceIndices[TriangleIndex * 3 + Sub];
						int32* Ptr = VertexMapping.Find(VertexIndex);
						if (!Ptr)
						{
							FTGOR_SkeletalMergeVertex Vertex;

							Vertex.Index = VertexIndex;
							Vertex.UV = SourceMergeLODData.StaticVertexBuffers.StaticMeshVertexBuffer.GetVertexUV(Vertex.Index, Section.ReferenceUVChannel);
							if ((FVector2D(0.0f, 0.0f) - Vertex.UV).Size() >= 0.001f &&
								(FVector2D(1.0f, 0.0f) - Vertex.UV).Size() >= 0.001f &&
								(FVector2D(0.0f, 1.0f) - Vertex.UV).Size() >= 0.001f &&
								(FVector2D(1.0f, 1.0f) - Vertex.UV).Size() >= 0.001f)
							{
								Vertex.Point = SourceMergeLODData.StaticVertexBuffers.StaticMeshVertexBuffer.GetVertexUV(Vertex.Index, Section.SourceUVChannel);
								
								Vertex.Tangent = FVector4(0.0f, 0.0f, 0.0f, 0.0f);
								Vertex.Bitangent = FVector::ZeroVector;
								Vertex.Target = FVector4(0.0f, 0.0f, 0.0f, 1.0f);

								const int32 Index = Vertices.Emplace(Vertex);
								VertexMapping.Emplace(VertexIndex, Index);
								VertexCount++;

								return Index;
							}

							return INDEX_NONE;
						}
						return *Ptr;
					};

					const int32 A = GenerateVertex(0);
					const int32 B = GenerateVertex(1);
					const int32 C = GenerateVertex(2);
					
					// Only add triangles that are valid
					if (A != INDEX_NONE && B != INDEX_NONE && C != INDEX_NONE)
					{
						Triangles.Emplace(A);
						Triangles.Emplace(B);
						Triangles.Emplace(C);

						TriangleMapping.Emplace(TriangleIndex, TriangleCount);
						TriangleCount++;

						// Tangent space computation adapted from https://gamedev.stackexchange.com/questions/68612/how-to-compute-tangent-and-bitangent-vectors
						
						FTGOR_SkeletalMergeVertex& Av = Vertices[A];
						FTGOR_SkeletalMergeVertex& Bv = Vertices[B];
						FTGOR_SkeletalMergeVertex& Cv = Vertices[C];

						const FVector Ap = SourceMergeLODData.StaticVertexBuffers.PositionVertexBuffer.VertexPosition(Av.Index);
						const FVector Bp = SourceMergeLODData.StaticVertexBuffers.PositionVertexBuffer.VertexPosition(Bv.Index);
						const FVector Cp = SourceMergeLODData.StaticVertexBuffers.PositionVertexBuffer.VertexPosition(Cv.Index);

						const FVector E1 = Bp - Ap;
						const FVector E2 = Cp - Ap;
						
						const FVector2D F1 = Bv.UV - Av.UV;
						const FVector2D F2 = Cv.UV - Av.UV;
						const FVector2D F3 = Cv.UV - Bv.UV;

						const float InvTan = 1.0F / (F1 ^ F2);
						const FVector4 Tangent = FVector4(F2.Y * E1 - F1.Y * E2, 0.0f) * InvTan;
						const FVector Bitangent = FVector(F1.X * E2 - F2.X * E1) * InvTan;

						Av.Tangent += Tangent;
						Bv.Tangent += Tangent;
						Cv.Tangent += Tangent;

						Av.Bitangent += Bitangent;
						Bv.Bitangent += Bitangent;
						Cv.Bitangent += Bitangent;

						////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
					}
				}
			}

			// Find duplicates and merge tangents, compute extend/center
			FVector MinPos = FVector(FLT_MAX);
			FVector MaxPos = FVector(-FLT_MAX);

			const int32 Vn = Vertices.Num();
			for (int32 Vi = 0; Vi < Vn; Vi++)
			{
				FTGOR_SkeletalMergeVertex& V = Vertices[Vi];

				const FVector Vp = SourceMergeLODData.StaticVertexBuffers.PositionVertexBuffer.VertexPosition(V.Index);
				for (int32 Wi = Vi+1; Wi < Vn; Wi++)
				{
					FTGOR_SkeletalMergeVertex& W = Vertices[Wi];

					// Find 
					const FVector Wp = SourceMergeLODData.StaticVertexBuffers.PositionVertexBuffer.VertexPosition(W.Index);
					if ((Vp - Wp).SizeSquared() < Threshold * Threshold)
					{
						V.Tangent += W.Tangent;
						W.Tangent = V.Tangent;

						Duplicates.Emplace(V.Index, W.Index);
						Duplicates.Emplace(W.Index, V.Index);
						DuplicateCount++;
					}
				}

				MinPos = MinPos.ComponentMin(Vp);
				MaxPos = MaxPos.ComponentMax(Vp);
			}

			RenderSection.Extend = (MaxPos - MinPos) / 2;
			RenderSection.Center = (MaxPos + MinPos) / 2;

			// Vertex Postprocessing
			for (FTGOR_SkeletalMergeVertex& V : Vertices)
			{
				const FVector SourceNormal = FVector(SourceMergeLODData.StaticVertexBuffers.StaticMeshVertexBuffer.VertexTangentZ(V.Index));
				V.Target = SourceMergeLODData.StaticVertexBuffers.StaticMeshVertexBuffer.VertexTangentX(V.Index);

				const FVector TargetY = SourceMergeLODData.StaticVertexBuffers.StaticMeshVertexBuffer.VertexTangentY(V.Index);
				const FVector TargetZ = SourceMergeLODData.StaticVertexBuffers.StaticMeshVertexBuffer.VertexTangentZ(V.Index);
				V.Target.W *= ((FVector(V.Target) ^ TargetY) | TargetZ);

				const FVector Tangent = UTGOR_Math::Normalize(FVector::VectorPlaneProject(FVector(V.Tangent), SourceNormal));
				const FVector Bitangent = Tangent ^ SourceNormal;

				const float Direction = FMath::Sign(Bitangent | V.Bitangent);
				V.Tangent = FVector4(Tangent, Direction);
				V.Bitangent = Bitangent;

				// Merge with connection point if overlap exists
				uint32* OverlapPtr = Overlaps.Find(V.Index);
				if (OverlapPtr)
				{
					V.Tangent = TargetMergeLODData.StaticVertexBuffers.StaticMeshVertexBuffer.VertexTangentX(*OverlapPtr);
					V.Bitangent = TargetMergeLODData.StaticVertexBuffers.StaticMeshVertexBuffer.VertexTangentY(*OverlapPtr);

					const FVector RefNormal = TargetMergeLODData.StaticVertexBuffers.StaticMeshVertexBuffer.VertexTangentZ(*OverlapPtr);
					const FVector Normal = (V.Bitangent ^ FVector(V.Tangent))* V.Tangent.W;

					V.Tangent.W *= (RefNormal | Normal);
				}


				/*
				// Rotatation direction
				V.Rotate.X = (FVector(V.Tangent) | FVector(SourceTangent));
				V.Rotate.Y = -(V.Bitangent | FVector(SourceTangent));
				V.Rotate.Normalize();
				*/
				/*
				FVector  x = XAxis.ToFVector();
				FVector4 z = ZAxis.ToFVector4();
				return (FVector(z) ^ x) * z.W;
				*/

				/*
				const float Dot = (FVector(SourceTangent) | FVector(V.Tangent)); //  + V.Tangent.W* SourceTangent.W
				const float Det = (SourceNormal | (FVector(SourceTangent) ^ FVector(V.Tangent)));// * V.Tangent.W * SourceTangent.W;
				V.Angle = FMath::Atan2(Det, Dot);

				V.Rotate.X = FMath::Cos(V.Angle);
				V.Rotate.Y = FMath::Sin(V.Angle);
				*/
			}

			auto CopyVertex = [&](int32 Vi, FTGOR_MergeUVVert& W)
			{
				const FTGOR_SkeletalMergeVertex& V = Vertices[Vi];

				W.Pos = FVector(V.Point, 0.5f);
				W.UV[0] = V.UV;
				W.UV[1] = V.Point;
				W.Color = FLinearColor(V.Target) / 2 + FLinearColor(0.5f, 0.5f, 0.5f, 0.5f);
				W.Tangent = V.Tangent;
				W.Bitangent = V.Bitangent;
			};

			// Generate projection
			RenderSection.Triangles.SetNumUninitialized(TriangleCount);
			for (int32 TriangleIndex = 0; TriangleIndex < TriangleCount; TriangleIndex++)
			{
				FTGOR_MergeUVTri& Projection = RenderSection.Triangles[TriangleIndex];
				CopyVertex(Triangles[TriangleIndex * 3 + 0], Projection.V0);
				CopyVertex(Triangles[TriangleIndex * 3 + 1], Projection.V1);
				CopyVertex(Triangles[TriangleIndex * 3 + 2], Projection.V2);
				RenderSection.TriangleCount++;
			}

			RenderSection.Mapping = TriangleMapping;


			auto ComputeExpand = [&](const FVector2D& A, const FVector2D& B, const FVector2D& C, float Dist) -> FVector2D
			{
				const FVector2D AB = (A - B).GetSafeNormal();
				const FVector2D AC = (A - C).GetSafeNormal();
				const float Sin = FMath::Sqrt(1.0f - FMath::Square(AB | AC));
				if (Sin < SMALL_NUMBER) return FVector2D::ZeroVector;
				return (AB + AC) * FMath::Min(Dist / Sin, Dist);
			};

			if (PaddingDistance * PaddingDistance > SMALL_NUMBER)
			{
				RenderSection.Triangles.SetNumUninitialized(TriangleCount * 2);
				for (int32 TriangleIndex = 0; TriangleIndex < TriangleCount; TriangleIndex++)
				{
					FTGOR_MergeUVTri& Tri = RenderSection.Triangles[TriangleIndex];
					RenderSection.Triangles[TriangleCount + TriangleIndex] = Tri;
					RenderSection.TriangleCount++;

					Tri.V0.UV[0] += ComputeExpand(Tri.V0.UV[0], Tri.V1.UV[0], Tri.V2.UV[0], PaddingDistance);
					Tri.V1.UV[0] += ComputeExpand(Tri.V1.UV[0], Tri.V2.UV[0], Tri.V0.UV[0], PaddingDistance);
					Tri.V2.UV[0] += ComputeExpand(Tri.V2.UV[0], Tri.V0.UV[0], Tri.V1.UV[0], PaddingDistance);

					const FVector Offset = FVector(0.0f, 0.0f, -0.25f);
					Tri.V0.Pos += Offset;
					Tri.V1.Pos += Offset;
					Tri.V2.Pos += Offset;
				}
			}

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
