// The Gateway of Realities: Planes of Existence.

#pragma once

#include "TGOR_ProceduralLibrary.h"
#include "TGOR_FillDelaunayLibrary.generated.h"


//////////////////////////////////////////// STRUCT /////////////////////////////////////////////////


USTRUCT(BlueprintType)
struct REALITIESPROCEDURAL_API FTGOR_FillSurfaceParams
{
	GENERATED_USTRUCT_BODY()
		FTGOR_FillSurfaceParams();


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Procedural Mesh")
		FVector UpVector;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Procedural Mesh")
		float CurveThreshold;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Procedural Mesh")
		bool Delaunay;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Procedural Mesh")
		float Thickness;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Procedural Mesh")
		FVector Extrude;
};

USTRUCT(BlueprintType)
struct REALITIESPROCEDURAL_API FTGOR_FillMaterialParams
{
	GENERATED_USTRUCT_BODY()
		FTGOR_FillMaterialParams();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Procedural Mesh")
		FTGOR_ProceduralMaterialParams Top;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Procedural Mesh")
		FTGOR_ProceduralMaterialParams Rim;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Procedural Mesh")
		FTGOR_ProceduralMaterialParams Bottom;
};

/**
 *
 */
UCLASS()
class REALITIESPROCEDURAL_API UTGOR_FillDelaunayLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

		////////////////////////////////////////////////////////////////////////////////////////////////////
public:

	/** Generate skew mesh */
	UFUNCTION(BlueprintPure, Category = "Procedural Mesh", Meta = (Keywords = "C++"))
		static void GenerateFill(
			USplineComponent* Spline,
			const FTransform& Transform,
			FTGOR_FillSurfaceParams Surface,
			FTGOR_FillMaterialParams Material,

			TArray<FTGOR_TriangleMesh>& Meshes);
};


