// The Gateway of Realities: Planes of Existence.
#pragma once

#include "CoreMinimal.h"

#include "TGOR_GeometryUserData.generated.h"

USTRUCT(BlueprintType)
struct FTGOR_MergeUVVert
{
	GENERATED_BODY()

	// Position of vertex
	UPROPERTY()
		FVector3f Pos;

	// UVs of vertex
	UPROPERTY()
		FVector2f UV[3];

	// Color of vertex
	UPROPERTY()
		FLinearColor Color;

	// Tangent of vertex
	UPROPERTY()
		FVector4f Tangent;

	// Bitangent of vertex
	UPROPERTY()
		FVector3f Bitangent;

	FVector3f Normal() const
	{
		return (Bitangent ^ FVector3f(Tangent)) * Tangent.W;
	}

	FTGOR_MergeUVVert()
		: Pos(ForceInit)
		, UV()
		, Color(ForceInit)
		, Tangent(ForceInit)
		, Bitangent(ForceInit)
	{ 
		UV[0] = FVector2f::ZeroVector;
		UV[1] = FVector2f::ZeroVector;
		UV[2] = FVector2f::ZeroVector;
	}
};

USTRUCT(BlueprintType)
struct FTGOR_MergeUVTri
{
	GENERATED_BODY()

	// First vertex
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = CanvasUVTri)
		FTGOR_MergeUVVert V0;

	// Second vertex
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = CanvasUVTri)
		FTGOR_MergeUVVert V1;

	// Third vertex
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = CanvasUVTri)
		FTGOR_MergeUVVert V2;

	FTGOR_MergeUVTri()
		: V0()
		, V1()
		, V2()
	{ }
};

USTRUCT(BlueprintType)
struct FTGOR_RenderGeometrySection
{
	GENERATED_USTRUCT_BODY()

	// UV channel pointing to source mesh texture
	UPROPERTY(VisibleAnywhere, Category = "MeshMerge")
		uint32 TriangleCount = 0;

	// Merge patch triangles prebaked, ready for rendering
	UPROPERTY()
		TArray<FTGOR_MergeUVTri> Triangles;

	// Mesh to render triangle mapping
	UPROPERTY()
		TMap<uint32, int32> Mapping;


	// Patch triangle extend in local space
	UPROPERTY(VisibleAnywhere, Category = "MeshMerge")
		FVector Extend = FVector::ZeroVector;

	// Patch triangle center in local space
	UPROPERTY(VisibleAnywhere, Category = "MeshMerge")
		FVector Center = FVector::ZeroVector;

	// Vertex bake textures
	UPROPERTY(VisibleAnywhere, Category = "MeshMerge")
		TMap<FName, UTexture2D*> Bakes;
};

USTRUCT(BlueprintType)
struct FTGOR_RenderGeometryBake
{
	GENERATED_USTRUCT_BODY()

	// Material to bake with
	UPROPERTY(EditAnywhere, Category = "MeshMerge")
		UMaterialInterface* BakeMaterial = nullptr;

	// Material used for padding
	UPROPERTY(EditAnywhere, Category = "MeshMerge")
		UMaterialInterface* PaddingMaterial = nullptr;

	// How many padding iterations should be applied for baked textures
	UPROPERTY(EditAnywhere, Category = "MeshMerge")
		int32 PaddingIterations = 1;
};

/** Stores geometry information for rendering */
UCLASS()
class CUSTOMISATIONSYSTEM_API UTGOR_GeometryUserData : public UAssetUserData
{
	GENERATED_UCLASS_BODY()

public:
	virtual void Draw(class FPrimitiveDrawInterface* PDI, const class FSceneView* View) const override;
	virtual FIntPoint GetTextureSize(int32 SectionIndex) const;
	virtual FString GetTextureName(int32 SectionIndex) const;


	UPROPERTY(EditAnywhere, Category = "MeshMerge")
		int32 BakeScaleX = -2;

	UPROPERTY(EditAnywhere, Category = "MeshMerge")
		int32 BakeScaleY = -2;

	// Materials used for vertex baking
	UPROPERTY(EditAnywhere, Category = "MeshMerge")
		TMap<FName, FTGOR_RenderGeometryBake> BakeMaterials;

	// Whether to create a texture asset on texture bake
	UPROPERTY(EditAnywhere, Category = "MeshMerge")
		bool CreateBakeAsset = false;

	// Name to use for bake button
	UPROPERTY(EditAnywhere, Category = "MeshMerge")
		FText BakeName;

	// Description to use for bake button
	UPROPERTY(EditAnywhere, Category = "MeshMerge")
		FText BakeDescription;

	// Section-specific options
	UPROPERTY(VisibleAnywhere, Category = "MeshMerge")
		TArray<FTGOR_RenderGeometrySection> RenderSections;

	UFUNCTION(BlueprintCallable, Category = "!TGOR Pawn", Meta = (Keywords = "C++"))
		void EnsureVertexBake(UObject* WorldContextObject, USkeletalMesh* SourceMesh, int32 SectionIndex, bool CreateAsset);

	UFUNCTION(BlueprintCallable, Category = "!TGOR Pawn", Meta = (Keywords = "C++"))
		virtual void BakeData(USkeletalMesh* SourceMesh);

	UFUNCTION(BlueprintCallable, Category = "!TGOR Pawn", Meta = (Keywords = "C++"))
		virtual void TestBake(USkeletalMesh* SourceMesh);
};

