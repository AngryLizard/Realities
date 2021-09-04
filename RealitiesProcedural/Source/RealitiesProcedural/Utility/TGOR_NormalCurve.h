// The Gateway of Realities: Planes of Existence.

#pragma once

#include "CoreMinimal.h"

#include "TGOR_NormalCurve.generated.h"


USTRUCT(BlueprintType)
struct REALITIESPROCEDURAL_API FTGOR_NormalCurve
{
	GENERATED_USTRUCT_BODY()
		FTGOR_NormalCurve();

	bool operator==(const FTGOR_NormalCurve& Other) const;

	FVector2D Sample(float Time) const;

	UPROPERTY(EditAnywhere, Category = "!TGOR Procedural")
		FVector X;

	UPROPERTY(EditAnywhere, Category = "!TGOR Procedural")
		FVector Y;
};

USTRUCT(BlueprintType)
struct REALITIESPROCEDURAL_API FTGOR_UnitCurve
{
	GENERATED_USTRUCT_BODY()
		FTGOR_UnitCurve();

	bool operator==(const FTGOR_UnitCurve& Other) const;

	float Sample(float Time) const;

	UPROPERTY(EditAnywhere, Category = "!TGOR Procedural")
		FRichCurve CurveData;

};
