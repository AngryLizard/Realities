// The Gateway of Realities: Planes of Existence.

#pragma once

#include "CoreMinimal.h"
#include "Components/SplineComponent.h"
#include "Components/SplineMeshComponent.h"
#include "RealitiesProcedural/Utility/TGOR_Triangulation.h"

#include "TGOR_ProceduralLibrary.generated.h"

class USplineMeshComponent;
class UProceduralMeshComponent;
class UInstancedStaticMeshComponent;

USTRUCT(BlueprintType)
struct REALITIESPROCEDURAL_API FTGOR_ProceduralMaterialParams
{
	GENERATED_USTRUCT_BODY()
		FTGOR_ProceduralMaterialParams();

	FVector2D Transform(const FVector2D& UV, const FVector2D& Bounds) const;
	FVector2D Inverse(const FVector2D& UV, const FVector2D& Bounds) const;

	/** Whether to align UVs with the mesh (uses TexelDensity if off) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Procedural Mesh")
		bool NormalisedUV;

	/** Intended source texture size */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Procedural Mesh", meta = (EditCondition = "!NormalisedUV"))
		FVector2D TextureSize;

	/** Pixels per meter */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Procedural Mesh", meta = (EditCondition = "!NormalisedUV"))
		FVector2D TexelDensity;

	/** UV offset in UV space */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Procedural Mesh")
		FVector2D UVOffset;

	/** UV scaling UV space */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Procedural Mesh")
		FVector2D UVScale;

	/** Material to be used */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Procedural Mesh")
		UMaterialInterface* Material;

	/** Vertex color to be used */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Procedural Mesh")
		FLinearColor VertexColor;
};

USTRUCT(BlueprintType)
struct REALITIESPROCEDURAL_API FTGOR_ProceduralStaticMesh
{
	GENERATED_USTRUCT_BODY()
		FTGOR_ProceduralStaticMesh();

	/** Mesh weight */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Procedural Mesh")
		float Weight;

	/** Mesh offset */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Procedural Mesh")
		FVector Offset;

	/** Mesh to apply */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Procedural Mesh")
		UStaticMesh* StaticMesh;

	/** Override materials */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, AdvancedDisplay, Category = "Procedural Mesh", Meta = (ToolTip = "Material overrides."))
		TArray<UMaterialInterface*> Materials;

	/** Display axis */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Procedural Mesh")
		TEnumAsByte<ESplineMeshAxis::Type> Axis;

};

USTRUCT(BlueprintType)
struct REALITIESPROCEDURAL_API FTGOR_ProceduralSplineMesh : public FTGOR_ProceduralStaticMesh
{
	GENERATED_USTRUCT_BODY()
		FTGOR_ProceduralSplineMesh();

	/** Minimum mesh length */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Procedural Mesh")
		float MinLength;

	/** Maximum mesh length */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Procedural Mesh")
		float MaxLength;

	/** Whether this spline can be used as an end piece */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Procedural Mesh")
		bool CanBeTail;

	/** Whether this spline can be used as a middle piece */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Procedural Mesh")
		bool CanBeSegment;

	/** Whether this spline can be used as a start piece */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Procedural Mesh")
		bool CanBeHead;
};

USTRUCT(BlueprintType)
struct REALITIESPROCEDURAL_API FTGOR_ProceduralPostMesh : public FTGOR_ProceduralStaticMesh
{
	GENERATED_USTRUCT_BODY()
		FTGOR_ProceduralPostMesh();

	/** Minimum corner angle */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Procedural Mesh")
		float MinAngle;

	/** Maximum corner angle */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Procedural Mesh")
		float MaxAngle;
};


USTRUCT(BlueprintType)
struct REALITIESPROCEDURAL_API FTGOR_ProceduralSplineMeshArray
{
	GENERATED_USTRUCT_BODY()
		FTGOR_ProceduralSplineMeshArray();

	int32 SamplePostMeshIndex(float Angle, FRandomStream& Random) const;
	int32 SampleSplineIndex(const TArray<int32>& Indices, FRandomStream& Random) const;
	int32 SampleSplineMesh(float Distance, bool IsFirst, FRandomStream& Random) const;

	/** Whether this array is aligned to the spline points */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Procedural Mesh")
		bool PointAligned;

	/** Distance between posts, posts get aligned with splines if 0 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Procedural Mesh")
		float PostDistances;

	/** Offset applied orthogonally to spline to offset spline meshes */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Procedural Mesh")
		FVector Offset;

	/** Segment indices that ought to be replaced with a mesh (or nothing) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Procedural Mesh")
		TMap<int32, FTGOR_ProceduralStaticMesh> Holes;

	/** Selections to be spread along spline */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Procedural Mesh")
		TArray<FTGOR_ProceduralSplineMesh> SplineMeshes;

	/** Selections to be spread along posts */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Procedural Mesh")
		TArray<FTGOR_ProceduralPostMesh> PostMeshes;

};

USTRUCT(BlueprintType)
struct REALITIESPROCEDURAL_API FTGOR_ProceduralMeshContainer
{
	GENERATED_USTRUCT_BODY()
		FTGOR_ProceduralMeshContainer();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Procedural Mesh")
		TArray<USplineMeshComponent*> SplineMeshes;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Procedural Mesh")
		TArray<USplineMeshComponent*> HoleMeshes;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Procedural Mesh")
		TArray<UStaticMeshComponent*> PostMeshes;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Procedural Mesh")
		TArray<FTransform> Holes;
};

/**
 *
 */
UCLASS()
class REALITIESPROCEDURAL_API UTGOR_ProceduralLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

		////////////////////////////////////////////////////////////////////////////////////////////////////
public:

	/** Generate procedural mesh UVs */
	UFUNCTION(BlueprintCallable, Category = "Procedural Mesh", Meta = (Keywords = "C++"))
		static FVector2D ProjectUV(const FVector& Location, const FVector& Normal, const FVector2D& Bounds);

	/** Compute bounds of two splines in world space */
	UFUNCTION(BlueprintCallable, Category = "Procedural Mesh", Meta = (Keywords = "C++"))
		static FVector2D ComputeTwinBounds(USplineComponent* Left, USplineComponent* Right, const FVector& Normal);

	/** Compute bounds of a spline in world space */
	UFUNCTION(BlueprintCallable, Category = "Procedural Mesh", Meta = (Keywords = "C++"))
		static FVector2D ComputeBounds(USplineComponent* Spline, const FVector& Normal);

	/** Get average distance between spline points */
	UFUNCTION(BlueprintPure, Category = "Procedural Mesh", Meta = (Keywords = "C++"))
		static float GetAveragePointDistance(USplineComponent* Left, USplineComponent* Right);

	/** Get maximum distance between spline points */
	UFUNCTION(BlueprintPure, Category = "Procedural Mesh", Meta = (Keywords = "C++"))
		static float GetMaxPointDistance(USplineComponent* Left, USplineComponent* Right);

	/** Get minimum distance between spline points */
	UFUNCTION(BlueprintPure, Category = "Procedural Mesh", Meta = (Keywords = "C++"))
		static float GetMinPointDistance(USplineComponent* Left, USplineComponent* Right);


	/** Combine two mesh sections into one */
	UFUNCTION(BlueprintPure, Category = "Procedural Mesh", Meta = (Keywords = "C++"))
		static FTGOR_TriangleMesh CombineMesheSection(const FTGOR_TriangleMesh& Meshes, const FTGOR_TriangleMesh& Other);

	/** Combine two meshes into one */
	UFUNCTION(BlueprintPure, Category = "Procedural Mesh", Meta = (Keywords = "C++"))
		static TArray<FTGOR_TriangleMesh> CombineMeshes(const TArray<FTGOR_TriangleMesh>& Meshes, const TArray<FTGOR_TriangleMesh>& Other);

	/** Merge meshes with matching material */
	UFUNCTION(BlueprintPure, Category = "Procedural Mesh", Meta = (Keywords = "C++"))
		static TArray<FTGOR_TriangleMesh> MergeMaterials(const TArray<FTGOR_TriangleMesh>& Meshes);


	/** Create instanced meshes from transform on randomly sampled instanced meshes */
	UFUNCTION(BlueprintCallable, Category = "Procedural Mesh", Meta = (Keywords = "C++"))
		static void PopulateInstancedMeshes(TArray<UInstancedStaticMeshComponent*> Components, const TArray<FTransform>& Transforms);

	/** Create instanced meshes from transform */
	UFUNCTION(BlueprintCallable, Category = "Procedural Mesh", Meta = (Keywords = "C++"))
		static void PopulateInstancedMesh(UInstancedStaticMeshComponent* Component, const TArray<FTransform>& Transforms);


	/** Fill mesh sections of procedural mesh */
	UFUNCTION(BlueprintCallable, Category = "Procedural Mesh", Meta = (Keywords = "C++"))
		static void ApplyToMeshes(UProceduralMeshComponent* ProceduralMesh, const TArray<FTGOR_TriangleMesh>& Meshes, bool EnableCollision);

	/** Destroy spline meshes */
	UFUNCTION(BlueprintCallable, Category = "Procedural Mesh", Meta = (Keywords = "C++"))
		static void ResetSplineMeshes(UPARAM(ref) FTGOR_ProceduralMeshContainer& MeshContainer);

	/** Generate spline meshes */
	UFUNCTION(BlueprintCallable, Category = "Procedural Mesh", Meta = (Keywords = "C++"))
		static void CreateSplineMeshes(USplineComponent* Spline, const TArray<FTGOR_ProceduralSplineMeshArray>& MeshArrays, int32 Seed, UPARAM(ref) FTGOR_ProceduralMeshContainer& MeshContainer);
};


