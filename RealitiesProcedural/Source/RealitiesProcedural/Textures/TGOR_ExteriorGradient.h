// The Gateway of Realities: Planes of Existence.

#pragma once

#include "CoreMinimal.h"
#include "TGOR_ProceduralGradient.h"

#include "RealitiesProcedural/Utility/TGOR_NormalCurve.h"
#include "TGOR_ExteriorGradient.generated.h"


USTRUCT(BlueprintType)
struct REALITIESPROCEDURAL_API FTGOR_ExteriorColumn
{
	GENERATED_USTRUCT_BODY()
		FTGOR_ExteriorColumn();

	UPROPERTY(EditAnywhere, Category = "!TGOR Procedural", meta = (DisplayName = "Color X/Y", Color="Red"))
		FTGOR_NormalCurve Color;

	UPROPERTY(EditAnywhere, Category = "!TGOR Procedural", meta = (DisplayName = "Roughness Min/Max", Color = "Green"))
		FTGOR_NormalCurve Roughness;

	UPROPERTY(EditAnywhere, Category = "!TGOR Procedural", meta = (DisplayName = "Metal/Glow", Color = "Blue"))
		FTGOR_NormalCurve Meglow;
};

USTRUCT(BlueprintType)
struct REALITIESPROCEDURAL_API FTGOR_ExteriorRow
{
	GENERATED_USTRUCT_BODY()
		FTGOR_ExteriorRow();

	UPROPERTY(EditAnywhere, Category = "!TGOR Procedural")
		FString Comment;

	UPROPERTY(EditAnywhere, Category = "!TGOR Procedural")
		FTGOR_ExteriorColumn Columns[4];
};

////////////////////////////////////////////////////////////////////////////////////////////////////

UCLASS(HideCategories = Object, BlueprintType)
class REALITIESPROCEDURAL_API UTGOR_ExteriorGradient : public UTGOR_ProceduralGradient
{
	GENERATED_UCLASS_BODY()

public:
	virtual void GenerateGradient(int32 SizeX, int32 SizeY, TArray<FLinearColor>& Pixels) override;

	/** Colors */
	UPROPERTY(EditAnywhere, Category = "!TGOR Procedural")
		TArray<FTGOR_ExteriorRow> Rows;

	UPROPERTY(EditAnywhere, Category = "!TGOR Procedural", meta = (ClampMin = 1, ClampMax = 64))
		int32 RowThickness;

};
