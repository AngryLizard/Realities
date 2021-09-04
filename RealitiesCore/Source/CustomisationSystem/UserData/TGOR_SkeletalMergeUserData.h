// The Gateway of Realities: Planes of Existence.
#pragma once

#include "CoreMinimal.h"

#include "TGOR_GeometryUserData.h"

#include "TGOR_SkeletalMergeUserData.generated.h"


USTRUCT(BlueprintType)
struct FTGOR_SkeletalMergeVertex
{
	GENERATED_USTRUCT_BODY()

	// Source index
	UPROPERTY(EditAnywhere, Category = "MeshMerge")
		uint32 Index;

	// Source UVs
	UPROPERTY(EditAnywhere, Category = "MeshMerge")
		FVector2D Point;

	// Reference UVs
	UPROPERTY(EditAnywhere, Category = "MeshMerge")
		FVector2D UV;

	// Reference UV tangent
	UPROPERTY(EditAnywhere, Category = "MeshMerge")
		FVector4 Tangent;

	// Reference UV bitangent
	UPROPERTY(EditAnywhere, Category = "MeshMerge")
		FVector Bitangent;

	// Target tangent (same normal)
	UPROPERTY(EditAnywhere, Category = "MeshMerge")
		FVector4 Target;
};

USTRUCT(BlueprintType)
struct FTGOR_SkeletalMergeSection
{
	GENERATED_USTRUCT_BODY()

	// Whether to look for a bridge in reference mesh
	UPROPERTY(EditAnywhere, Category = "MeshMerge")
		bool GenerateBridge = true;

	// UV channel pointing to source mesh texture
	UPROPERTY(EditAnywhere, Category = "MeshMerge")
		uint32 SourceUVChannel = 0;

	// UV channel pointing to reference mesh texture
	UPROPERTY(EditAnywhere, Category = "MeshMerge")
		uint32 ReferenceUVChannel = 1;

	// UV coords to be ignored (threshold 0.001)
	UPROPERTY(EditAnywhere, Category = "MeshMerge")
		FVector2D UVIgnore = FVector2D(0.0f, 1.0f);
};

/** Stores baking information for mesh holes */
UCLASS(Blueprintable)
class CUSTOMISATIONSYSTEM_API UTGOR_SkeletalMergeUserData : public UTGOR_GeometryUserData
{
	GENERATED_UCLASS_BODY()

public:
	virtual void Draw(class FPrimitiveDrawInterface* PDI, const class FSceneView* View) const override;
	virtual FIntPoint GetTextureSize(int32 SectionIndex) const override;
	virtual FString GetTextureName(int32 SectionIndex) const override;

	virtual void BakeData(USkeletalMesh* SourceMesh) override;

	UPROPERTY(EditAnywhere, Category = "MeshMerge")
		USkeletalMesh* ReferenceMesh = nullptr;

	// Section-specific options
	UPROPERTY(EditAnywhere, Category = "MeshMerge")
		TArray<FTGOR_SkeletalMergeSection> Sections;

	// How close vertices need to be to count as overlapping
	UPROPERTY(EditAnywhere, Category = "MeshMerge")
		float Threshold = 0.01f;

	// How much padding (distance in UV space) to apply
	UPROPERTY(EditAnywhere, Category = "MeshMerge")
		float PaddingDistance = 0.01f;

	/*
	// How many vertices are overlapping
	UPROPERTY(VisibleAnywhere, Category = "MeshMerge")
		int32 OverlapCount = 0;

	// Vertex source index to reference index mapping
	UPROPERTY()
		TMap<uint32, uint32> Overlaps;


	// How many vertices are duplicates
	UPROPERTY(VisibleAnywhere, Category = "MeshMerge")
		uint32 DuplicateCount = 0;

	// Vertex source index to duplicate vertex index mapping
	UPROPERTY()
		TMap<uint32, uint32> Duplicates;


	// Number of vertices that are mapped to the reference
	UPROPERTY(VisibleAnywhere, Category = "MeshMerge")
		int32 VertexCount = 0;

	// Vertices on source mesh that map to a reference vertices
	UPROPERTY()
		TArray<FTGOR_SkeletalMergeVertex> Vertices;

	// Vertex source index to mapped index mapping
	UPROPERTY()
		TMap<uint32, int32> VertexMapping;


	// Number of triangles that are mapped to the reference
	UPROPERTY(VisibleAnywhere, Category = "MeshMerge")
		int32 TriangleCount = 0;

	// Triangles on source mesh that map to a reference triangle
	UPROPERTY()
		TArray<int32> Triangles;

	// Triangles on source mesh that map to a reference triangle
	UPROPERTY()
		TArray<int32> Adjacencies;

	// Triangle source index to mapped triangle mapping
	UPROPERTY()
		TMap<uint32, int32> TriangleMapping;

	// Number of edges in the hull in UV space
	UPROPERTY(VisibleAnywhere, Category = "MeshMerge")
		int32 EdgeCount = 0;

	// Edges in the hull in UV space (mappend index from-to relationship)
	UPROPERTY()
		TArray<int32> Edges;
	*/

};


