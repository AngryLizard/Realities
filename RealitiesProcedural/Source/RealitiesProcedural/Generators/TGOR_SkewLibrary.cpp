// The Gateway of Realities: Planes of Existence.

#include "TGOR_SkewLibrary.h"
#include "ProceduralMeshComponent/Public/ProceduralMeshComponent.h"
#include "RealitiesUtility/Utility/TGOR_Math.h"

FTGOR_SkewParams::FTGOR_SkewParams()
:	Scale(FVector::OneVector),
	Offset(FVector::ZeroVector),
	XSkew(FVector2D::ZeroVector),
	YSkew(FVector2D::ZeroVector),
	ZSkew(FVector2D::ZeroVector)
{

}


int32 GetNewIndexForOldVertIndex(const FMatrix& Transform, int32 MeshVertIndex, TMap<int32, int32>& MeshToSectionVertMap, const FStaticMeshVertexBuffers& VertexBuffers, TArray<FVector>& Vertices, TArray<FVector>& Normals, TArray<FVector2D>& UVs, TArray<FProcMeshTangent>& Tangents, TArray<FColor>& Colors)
{
	int32* NewIndexPtr = MeshToSectionVertMap.Find(MeshVertIndex);
	if (NewIndexPtr != nullptr)
	{
		return *NewIndexPtr;
	}
	else
	{
		// Copy position
		int32 SectionVertIndex = Vertices.Add(Transform.TransformPosition(VertexBuffers.PositionVertexBuffer.VertexPosition(MeshVertIndex)));

		// Copy normal
		Normals.Add(Transform.TransformVector(VertexBuffers.StaticMeshVertexBuffer.VertexTangentZ(MeshVertIndex)));
		check(Normals.Num() == Vertices.Num());

		// Copy UVs
		UVs.Add(VertexBuffers.StaticMeshVertexBuffer.GetVertexUV(MeshVertIndex, 0));
		check(UVs.Num() == Vertices.Num());

		// Copy tangents
		FVector4 TangentX = VertexBuffers.StaticMeshVertexBuffer.VertexTangentX(MeshVertIndex);
		FProcMeshTangent NewTangent(Transform.TransformVector(TangentX), TangentX.W < 0.f);
		Tangents.Add(NewTangent);
		check(Tangents.Num() == Vertices.Num());

		// Copy colors
		if (VertexBuffers.ColorVertexBuffer.GetNumVertices() > 0)
		{
			Colors.Add(VertexBuffers.ColorVertexBuffer.VertexColor(MeshVertIndex));
		}
		else
		{
			Colors.Add(FColor::White);
		}

		MeshToSectionVertMap.Add(MeshVertIndex, SectionVertIndex);

		return SectionVertIndex;
	}
}

void GetSectionFromStaticMesh(const FMatrix& Transform, UStaticMesh* InMesh, int32 LODIndex, int32 SectionIndex, TArray<FVector>& Vertices, TArray<int32>& Triangles, TArray<FVector>& Normals, TArray<FVector2D>& UVs, TArray<FProcMeshTangent>& Tangents, TArray<FColor>& Colors)
{
	if (InMesh != nullptr)
	{
		if (InMesh->RenderData != nullptr && InMesh->RenderData->LODResources.IsValidIndex(LODIndex))
		{
			const FStaticMeshLODResources& LOD = InMesh->RenderData->LODResources[LODIndex];
			if (LOD.Sections.IsValidIndex(SectionIndex))
			{
				// Empty output buffers
				Vertices.Reset();
				Triangles.Reset();
				Normals.Reset();
				UVs.Reset();
				Tangents.Reset();

				// Map from vert buffer for whole mesh to vert buffer for section of interest
				TMap<int32, int32> MeshToSectionVertMap;

				const FStaticMeshSection& Section = LOD.Sections[SectionIndex];
				const uint32 OnePastLastIndex = Section.FirstIndex + Section.NumTriangles * 3;
				FIndexArrayView Indices = LOD.IndexBuffer.GetArrayView();

				// Iterate over section index buffer, copying verts as needed
				for (uint32 i = Section.FirstIndex; i < OnePastLastIndex; i++)
				{
					uint32 MeshVertIndex = Indices[i];

					// See if we have this vert already in our section vert buffer, and copy vert in if not 
					int32 SectionVertIndex = GetNewIndexForOldVertIndex(Transform, MeshVertIndex, MeshToSectionVertMap, LOD.VertexBuffers, Vertices, Normals, UVs, Tangents, Colors);

					// Add to index buffer
					Triangles.Add(SectionVertIndex);
				}
			}
		}
	}
}
void UTGOR_SkewLibrary::GenerateSkew(
	int32 LOD,
	UStaticMesh* StaticMesh,
	FTGOR_SkewParams Skew,
	TArray<FTGOR_TriangleMesh>& Meshes)
{
	const FMatrix Transform = FMatrix(
		FPlane(Skew.Scale.X, Skew.XSkew.X, Skew.XSkew.Y, Skew.Offset.X),
		FPlane(Skew.YSkew.X, Skew.Scale.Y, Skew.YSkew.Y, Skew.Offset.Y),
		FPlane(Skew.ZSkew.X, Skew.ZSkew.Y, Skew.Scale.Z, Skew.Offset.Z),
		FPlane(0.0f, 0.0f, 0.0f, 1.0f));

	if (StaticMesh != nullptr)
	{
		//// MESH DATA
		int32 NumSections = StaticMesh->GetNumSections(LOD);
		if (NumSections > 0)
		{
			Meshes.SetNum(NumSections);

			for (int32 SectionIndex = 0; SectionIndex < NumSections; SectionIndex++)
			{
				FTGOR_TriangleMesh& Mesh = Meshes[SectionIndex];

				// Buffers for copying geom data
				TArray<FVector> Vertices;
				TArray<int32> Triangles;
				TArray<FVector> Normals;
				TArray<FVector2D> UVs;
				TArray<FVector2D> UVs1;
				TArray<FVector2D> UVs2;
				TArray<FVector2D> UVs3;
				TArray<FProcMeshTangent> Tangents;
				TArray<FColor> Colors;

				// Get geom data from static mesh
				GetSectionFromStaticMesh(Transform, StaticMesh, LOD, SectionIndex, Vertices, Triangles, Normals, UVs, Tangents, Colors);

				const int32 VertexNum = Vertices.Num();
				Mesh.Vertices.SetNum(VertexNum);
				Mesh.Triangulation.Points.SetNum(VertexNum);

				for (int32 Index = 0; Index < VertexNum; Index++)
				{
					Mesh.Triangulation.Points[Index] = Vertices[Index];
					Mesh.Vertices[Index].Color = Colors[Index];
					Mesh.Vertices[Index].Normal = Normals[Index];
					Mesh.Vertices[Index].Tangent = Tangents[Index].TangentX;
					Mesh.Vertices[Index].UV = UVs[Index];
				}

				const int32 TriangleNum = Triangles.Num() / 3;
				Mesh.Triangulation.Triangles.SetNum(Triangles.Num() / 3);

				for (int32 Index = 0; Index < TriangleNum; Index++)
				{
					FTGOR_Triangle& Triangle = Mesh.Triangulation.Triangles[Index];
					Triangle.Verts[0] = Triangles[Index * 3 + 0];
					Triangle.Verts[1] = Triangles[Index * 3 + 1];
					Triangle.Verts[2] = Triangles[Index * 3 + 2];
				}
			}

			//// SIMPLE COLLISION

			if (StaticMesh->BodySetup != nullptr)
			{
				// Just shove everything into the first mesh
				FTGOR_TriangleMesh& Mesh = Meshes[0];

				// Iterate over all convex hulls on static mesh..
				const int32 NumConvex = StaticMesh->BodySetup->AggGeom.ConvexElems.Num();
				for (int ConvexIndex = 0; ConvexIndex < NumConvex; ConvexIndex++)
				{

					// Copy convex verts to ProcMesh
					FKConvexElem MeshConvex = StaticMesh->BodySetup->AggGeom.ConvexElems[ConvexIndex];

					for (FVector& Vertex : MeshConvex.VertexData)
					{
						Vertex = Transform.TransformVector(Vertex);
					}

					Mesh.Convex.Emplace(MeshConvex.VertexData);
				}
			}

			//// MATERIALS

			for (int32 MatIndex = 0; MatIndex < StaticMesh->StaticMaterials.Num(); MatIndex++)
			{
				if (Meshes.IsValidIndex(MatIndex))
				{
					FTGOR_TriangleMesh& Mesh = Meshes[MatIndex];
					Mesh.Material = StaticMesh->GetMaterial(MatIndex);
				}
			}
		}
	}

}
