// The Gateway of Realities: Planes of Existence.

#pragma once

#include "TGOR_ProceduralLibrary.h"
#include "TGOR_RidgeFillSplineLibrary.generated.h"

//////////////////////////////////////////// STRUCT /////////////////////////////////////////////////

USTRUCT(BlueprintType)
struct REALITIESPROCEDURAL_API FTGOR_RidgeCurvePoint
{
	GENERATED_USTRUCT_BODY()

		UPROPERTY(EditAnywhere, Category = "Procedural Mesh")
		float Ratio = 0.0f;

	UPROPERTY(EditAnywhere, Category = "Procedural Mesh")
		float Value = 0.0f;

	UPROPERTY(EditAnywhere, Category = "Procedural Mesh")
		float Slope = 0.0f;
};

USTRUCT(BlueprintType)
struct REALITIESPROCEDURAL_API FTGOR_RidgeSplinePoint
{
	GENERATED_USTRUCT_BODY()

		UPROPERTY(EditAnywhere, Category = "Procedural Mesh")
		int32 LeftIndex = -1;

	UPROPERTY(EditAnywhere, Category = "Procedural Mesh")
		int32 RightIndex = -1;
};

USTRUCT(BlueprintType)
struct REALITIESPROCEDURAL_API FTGOR_RidgeSegmentPoint
{
	GENERATED_USTRUCT_BODY()

		UPROPERTY(EditAnywhere, Category = "Procedural Mesh")
		float LeftDistance = 0.0f;

	UPROPERTY(EditAnywhere, Category = "Procedural Mesh")
		float RightDistance = 0.0f;
};

////////////////////////////////////////////////////////////////////////////////////////////////////

UENUM(BlueprintType)
enum class ETGOR_RidgeFillSplineType : uint8
{
	/** Spread edge loops evenly along splines */
	Spread,
	/** Edge loops bridge spline points with same index */
	Match,
	/** Edge loops bridge spline points closest to each other based on distance/angle */
	Dynamic
};


USTRUCT(BlueprintType)
struct REALITIESPROCEDURAL_API FTGOR_RidgeSurfaceParams
{
	GENERATED_USTRUCT_BODY()
		FTGOR_RidgeSurfaceParams();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Procedural Mesh")
		FVector UpVector;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Procedural Mesh")
		FRuntimeFloatCurve Curve;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Procedural Mesh", meta = (ClampMin = 0))
		int32 FillerSplines;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Procedural Mesh", meta = (ClampMin = 0))
		int32 FillerSegments;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Procedural Mesh")
		float FillerHeight;
};

USTRUCT(BlueprintType)
struct REALITIESPROCEDURAL_API FTGOR_RidgeMaterialParams
{
	GENERATED_USTRUCT_BODY()
		FTGOR_RidgeMaterialParams();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Procedural Mesh")
		bool ProjectUV;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Procedural Mesh")
		float UnwrapLane;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Procedural Mesh")
		FTGOR_ProceduralMaterialParams Material;

};

/**
 *
 */
UCLASS()
class REALITIESPROCEDURAL_API UTGOR_RidgeFillSplineLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

	////////////////////////////////////////////////////////////////////////////////////////////////////
public:

	/** Generate skew mesh */
	UFUNCTION(BlueprintPure, Category = "Procedural Mesh", Meta = (Keywords = "C++"))
		static void GenerateRidge(
			USplineComponent* Left,
			USplineComponent* Right,
			const FTransform& Transform,
			FTGOR_RidgeSurfaceParams Surface,
			FTGOR_RidgeMaterialParams Material,
			ETGOR_RidgeFillSplineType Type,

			TArray<FTGOR_TriangleMesh>& Meshes);

};

