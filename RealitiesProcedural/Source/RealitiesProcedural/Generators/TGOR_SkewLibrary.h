// The Gateway of Realities: Planes of Existence.

#pragma once

#include "TGOR_ProceduralLibrary.h"
#include "TGOR_SkewLibrary.generated.h"

//////////////////////////////////////////// STRUCTS /////////////////////////////////////////////////

USTRUCT(BlueprintType)
struct REALITIESPROCEDURAL_API FTGOR_SkewParams
{
	GENERATED_USTRUCT_BODY()
		FTGOR_SkewParams();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Procedural Mesh")
		FVector Scale;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Procedural Mesh")
		FVector Offset;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Procedural Mesh")
		FVector2D XSkew;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Procedural Mesh")
		FVector2D YSkew;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Procedural Mesh")
		FVector2D ZSkew;
};

/**
 *
 */
UCLASS()
class REALITIESPROCEDURAL_API UTGOR_SkewLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

		////////////////////////////////////////////////////////////////////////////////////////////////////
public:

	/** Generate skew mesh */
	UFUNCTION(BlueprintPure, Category = "Procedural Mesh", Meta = (Keywords = "C++"))
		static void GenerateSkew(
			int32 LOD,
			UStaticMesh* StaticMesh,
			FTGOR_SkewParams Skew,

			TArray<FTGOR_TriangleMesh>& Meshes);

};

