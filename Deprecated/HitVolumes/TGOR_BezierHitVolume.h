// TGOR (C) // CHECKED //
#pragma once

#include "CoreMinimal.h"
#include "Actors/Combat/HitVolumes/TGOR_FunctionHitVolume.h"
#include "TGOR_BezierHitVolume.generated.h"

/**
 * ATGOR_BezierHitVolume defines bezier shaped hitvolumes
 */
UCLASS(Blueprintable)
class REALITIES_API ATGOR_BezierHitVolume : public ATGOR_FunctionHitVolume
{
	GENERATED_BODY()


public:

	////////////////////////////////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////// ENGINE ////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////////////////////////////

	ATGOR_BezierHitVolume(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	//////////////////////////////////////////////// IMPLEMENTABLES /////////////////////////////////////////

	/**  */

	////////////////////////////////////////////// COMPONENTS //////////////////////////////////////////////////////
private:

	/**  */

	////////////////////////////////////////////////////////////////////////////////////////////////////
protected:

	/** Bezier target offset direction */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "TGOR HitVolume")
		FVector OffsetTarget;

	/** Bezier offset direction */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "TGOR HitVolume")
		FVector OffsetDirection;

	/** Bezier random offset variance */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "TGOR HitVolume")
		FVector OffsetVariance;

	/////////////////////////////////////////////// INTERNAL ///////////////////////////////////////////

protected:

	FVector RandomVector(const FVector& Range);
	virtual void Setup(FTGOR_FunctionSegment& Segment) override;
	virtual FVector Function(float X, const FTGOR_FunctionSegment& Segment, float Time) override;

private:

};
