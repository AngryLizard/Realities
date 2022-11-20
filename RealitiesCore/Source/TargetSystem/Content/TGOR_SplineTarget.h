// The Gateway of Realities: Planes of Existence. By Salireths.

#pragma once

#include "../TGOR_AimInstance.h"

#include "TGOR_ComponentTarget.h"
#include "TGOR_SplineTarget.generated.h"


UENUM(BlueprintType)
enum class ETGOR_SplinePointEnumeration : uint8
{
	Start,
	Between,
	End
};

/**
 * 
 */
UCLASS(Blueprintable)
class TARGETSYSTEM_API UTGOR_SplineTarget : public UTGOR_ComponentTarget
{
	GENERATED_BODY()
		
public:
	UTGOR_SplineTarget();

	virtual bool OverlapTarget(UTGOR_AimTargetComponent* Component, const FVector& Origin, FTGOR_AimPoint& Point) const override;
	virtual FVector QueryAimLocation(const FTGOR_AimInstance& Instance) const override;
	virtual FVector QueryStickyLocation(const FTGOR_AimInstance& Instance) const override;

	////////////////////////////////////////////////////////////////////////////////////////////////////
protected:

	/** Which spline point to sample */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "!TGOR Target", Meta = (Keywords = "C++"))
		ETGOR_SplinePointEnumeration SplinePoint;
};
