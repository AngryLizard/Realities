// The Gateway of Realities: Planes of Existence.

#pragma once

#include "Components/ArrowComponent.h"

#include "TGOR_ProceduralLibrary.h"
#include "TGOR_RingLibrary.generated.h"

//////////////////////////////////////////// STRUCT /////////////////////////////////////////////////

USTRUCT(BlueprintType)
struct REALITIESPROCEDURAL_API FTGOR_RingShapeParams
{
	GENERATED_USTRUCT_BODY()
		FTGOR_RingShapeParams();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Procedural Mesh", meta = (ClampMin = 0))
		int32 Segments;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Procedural Mesh", meta = (ClampMin = 0))
		float Radius;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Procedural Mesh", meta = (ClampMin = 0))
		float Girth;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Procedural Mesh")
		float Principal;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Procedural Mesh")
		float Residual;
};

USTRUCT(BlueprintType)
struct REALITIESPROCEDURAL_API FTGOR_RingMaterialParams
{
	GENERATED_USTRUCT_BODY()
		FTGOR_RingMaterialParams();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Procedural Mesh")
		bool ProjectUV;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Procedural Mesh")
		FTGOR_ProceduralMaterialParams Material;

};

/**
 *
 */
UCLASS()
class REALITIESPROCEDURAL_API UTGOR_RingLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

	////////////////////////////////////////////////////////////////////////////////////////////////////
public:

	/** Generate ring mesh */
	UFUNCTION(BlueprintPure, Category = "Procedural Mesh", Meta = (Keywords = "C++"))
		static void GenerateRing(
			UArrowComponent* Direction,
			const FTransform& Transform,
			FTGOR_RingShapeParams Shape,
			FTGOR_RingMaterialParams Material,

			TArray<FTGOR_TriangleMesh>& Meshes);

};

