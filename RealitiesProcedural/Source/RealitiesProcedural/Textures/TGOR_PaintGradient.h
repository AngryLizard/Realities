// The Gateway of Realities: Planes of Existence.

#pragma once

#include "CoreMinimal.h"
#include "Curves/CurveLinearColor.h"
#include "TGOR_ProceduralGradient.h"

#include "RealitiesProcedural/Utility/TGOR_NormalCurve.h"
#include "TGOR_PaintGradient.generated.h"

class UTGOR_PaintGradient;

////////////////////////////////////////////////////////////////////////////////////////////////////

USTRUCT(BlueprintType)
struct REALITIESPROCEDURAL_API FTGOR_GradientFilter
{
	GENERATED_USTRUCT_BODY()
		FTGOR_GradientFilter();

	FLinearColor HSVFilter(const FLinearColor& HSV, float Time) const;
	FLinearColor RGBFilter(const FLinearColor& RGB, float Time) const;

	UPROPERTY(EditAnywhere, Category = "!TGOR Procedural", meta = (ClampMin = -1.0, ClampMax = 1.0))
		FTGOR_UnitCurve Temperature;

	UPROPERTY(EditAnywhere, Category = "!TGOR Procedural", meta = (ClampMin = -1.0, ClampMax = 1.0))
		FTGOR_UnitCurve MagentaGreen;

	UPROPERTY(EditAnywhere, Category = "!TGOR Procedural", meta = (ClampMin = 0.0, ClampMax = 360))
		FTGOR_UnitCurve Hue;

	UPROPERTY(EditAnywhere, Category = "!TGOR Procedural", meta = (ClampMin = -1.0, ClampMax = 1.0))
		FTGOR_UnitCurve Saturation;

	UPROPERTY(EditAnywhere, Category = "!TGOR Procedural", meta = (ClampMin = -1.0, ClampMax = 1.0))
		FTGOR_UnitCurve Value;
};

USTRUCT(BlueprintType)
struct REALITIESPROCEDURAL_API FTGOR_GradientSection
{
	GENERATED_USTRUCT_BODY()
		FTGOR_GradientSection();

	UPROPERTY(EditAnywhere, Category = "!TGOR Procedural")
		FTGOR_GradientFilter Horizontal;

	UPROPERTY(EditAnywhere, Category = "!TGOR Procedural")
		FTGOR_GradientFilter Vertical;

	UPROPERTY(EditAnywhere, Category = "!TGOR Procedural")
		TArray<UCurveLinearColor*> Curves;
};

USTRUCT(BlueprintType)
struct REALITIESPROCEDURAL_API FTGOR_GradientCache
{
	GENERATED_USTRUCT_BODY()
		FTGOR_GradientCache();

	UPROPERTY(EditAnywhere, Category = "!TGOR Procedural")
		int32 SizeX;

	UPROPERTY(EditAnywhere, Category = "!TGOR Procedural")
		int32 SizeY;

	UPROPERTY(EditAnywhere, Category = "!TGOR Procedural")
		int32 Height;

	UPROPERTY(EditAnywhere, Category = "!TGOR Procedural")
		int32 Offset;

	UPROPERTY(EditAnywhere, Category = "!TGOR Procedural")
		TArray<FLinearColor> Pixels;

	/** Base colors */
	UPROPERTY(EditAnywhere, Category = "!TGOR Procedural")
		TArray<UCurveLinearColor*> Curves;
};

////////////////////////////////////////////////////////////////////////////////////////////////////

UCLASS(HideCategories = Object, BlueprintType)
class REALITIESPROCEDURAL_API UTGOR_PaintGradient : public UTGOR_ProceduralGradient
{
	GENERATED_UCLASS_BODY()

public:

#if WITH_EDITOR
	void UpdateCurveSlot(UCurveBase* Curve, EPropertyChangeType::Type ChangeType);
#endif

	/**
	 * Sample gradient curves (checks cache first)
	 */
	UFUNCTION()
		void SampleGradientCurves();

	virtual void GenerateGradient(int32 SizeX, int32 SizeY, TArray<FLinearColor>& Pixels) override;

	/** Whether the pixel cache is marked dirty */
	UPROPERTY(Transient)
		bool CacheDirty;

protected:

	/** Pixel cache after curve sampler */
	UPROPERTY(Transient)
		TArray<FTGOR_GradientCache> PixelCache;

public:

	/** Colors */
	UPROPERTY(EditAnywhere, Category = "!TGOR Procedural")
		TArray<FTGOR_GradientSection> Sections;

};
