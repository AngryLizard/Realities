// The Gateway of Realities: Planes of Existence.

#pragma once

#include "TGOR_ProceduralLibrary.h"
#include "TGOR_DelaunayFillSplineLibrary.generated.h"


//////////////////////////////////////////// STRUCTS /////////////////////////////////////////////////

USTRUCT(BlueprintType)
struct REALITIESPROCEDURAL_API FTGOR_DelaunaySurfaceParams
{
	GENERATED_USTRUCT_BODY()
		FTGOR_DelaunaySurfaceParams();

	float SampleCurve(int32 Index, float Time) const;
	float SampleCurves(float X, float Y) const;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Procedural Mesh")
		FVector UpVector;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Procedural Mesh")
		TArray<FRuntimeFloatCurve> Curves;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Procedural Mesh")
		float FillerMaxSize;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Procedural Mesh")
		float FillerHeight;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Procedural Mesh")
		bool Delaunay;
};

USTRUCT(BlueprintType)
struct REALITIESPROCEDURAL_API FTGOR_DelaunayMaterialParams
{
	GENERATED_USTRUCT_BODY()
		FTGOR_DelaunayMaterialParams();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Procedural Mesh")
		bool ProjectUV;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Procedural Mesh")
		FTGOR_ProceduralMaterialParams Material;
};

USTRUCT(BlueprintType)
struct REALITIESPROCEDURAL_API FTGOR_DelaunayInstanceParams
{
	GENERATED_USTRUCT_BODY()
		FTGOR_DelaunayInstanceParams();

	FVector SampleAlignment(FRandomStream& Random);
	float SampleRotation(FRandomStream& Random);

	// Size of the instance in each dimension
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Procedural Mesh")
		FVector InstanceExtend;
	
	// All possible up-vector directions the instances can spawn in
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Procedural Mesh")
		TArray<FVector> InstanceAlignments;

	// All possible rotations around Z-Axis in radians, interval form
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Procedural Mesh")
		TArray<FVector2D> InstanceRotations;

	// Whether to place the instances on the mean of all connecting vertices or according to the voronoi points.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Procedural Mesh")
		bool PlaceOnMean;

	// Whether to take neighbouring neighbour's principal axis into consideration or not
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Procedural Mesh")
		bool RotateAlongPrincipal;
};

USTRUCT(BlueprintType)
struct REALITIESPROCEDURAL_API FTGOR_DelaunayHoleParams
{
	GENERATED_USTRUCT_BODY()
		FTGOR_DelaunayHoleParams();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Procedural Mesh")
		float Radius;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Procedural Mesh")
		TArray<FTransform> Transforms;
};

/**
 *
 */
UCLASS()
class REALITIESPROCEDURAL_API UTGOR_DelaunayFillSplineLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

		////////////////////////////////////////////////////////////////////////////////////////////////////
public:

		/** This fills the space between two splines using delaunay triangulation.
		* For every triangle an instance (output as transforms) can be generated.
		* These instances are rotated according to the instance options.
		*/
		UFUNCTION(BlueprintPure, Category = "Procedural Mesh", Meta = (Keywords = "C++"))
			static void GenerateDelaunay(
				USplineComponent* Left, 
				USplineComponent* Right,
				const FTransform& Transform,
				FTGOR_DelaunaySurfaceParams Surface,
				FTGOR_DelaunayMaterialParams Material,
				FTGOR_DelaunayInstanceParams Instances,
				FTGOR_DelaunayHoleParams Holes,
				
				TArray<FTGOR_TriangleMesh>& Meshes,
				TArray<FTransform>& Transforms);

};
