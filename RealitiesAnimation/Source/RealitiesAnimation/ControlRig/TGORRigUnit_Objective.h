// The Gateway of Realities: Planes of Existence.

#pragma once

#include "Units/RigUnit.h"
#include "TGORRigUnit_Utility.h"
#include "TGORRigUnit_Objective.generated.h"

/**
 * Retargeting bones around some pivot
 */
USTRUCT(BlueprintType, meta = (Abstract))
struct REALITIESANIMATION_API FTGORRigUnit_Objective : public FTGORRigUnit_Mutable
{
	GENERATED_BODY()

	/**
	 * The base objective element
	 */
	UPROPERTY(meta = (Input, ExpandByDefault))
		FRigElementKey ObjBaseKey = FRigElementKey(FName(), ERigElementType::Control);

	/**
	 * Which axis determines the objective up axis for alignment
	 */
	UPROPERTY(meta = (Input, DetailsOnly))
		FVector ObjectiveUpAxis = FVector::UpVector;

	// Cache
	UPROPERTY(Transient)
		FCachedRigElement ObjBaseCache;
};

////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * Project an objective on a plane
 */
USTRUCT(meta = (DisplayName = "Objective Planar Project", Category = "TGOR Articulation", Keywords = "TGOR,Objective", PrototypeName = "ObjectivePlanarProject", NodeColor = "1.0 0.44 0.0"))
struct REALITIESANIMATION_API FTGORRigUnit_ObjectivePlanarProject : public FTGORRigUnit_Objective
{
	GENERATED_BODY()

		FTGORRigUnit_ObjectivePlanarProject() {}

	RIGVM_METHOD()
		virtual void Execute(const FRigUnitContext& Context) override;

	virtual FString ProcessPinLabelForInjection(const FString& InLabel) const override;

	/**
	 * Pivot to project towards
	 */
	UPROPERTY(meta = (Input))
		FVector Pivot = FVector::ZeroVector;

	/**
	 * Maximum ratio the objective is going to be projected with
	 */
	UPROPERTY(meta = (Input, DetailsOnly))
		float MaxProject = 0.5f;

	/**
	 * How relatively close we start rotating the objective to match
	 */
	UPROPERTY(meta = (Input, DetailsOnly))
		float RotationThreshold = 0.25f;

	/**
	 * Which axis determines the plane normal
	 */
	UPROPERTY(meta = (Input, DetailsOnly))
		FVector PlaneAxis = FVector::UpVector;

	/**
	 * The plane element
	 */
	UPROPERTY(meta = (Input, ExpandByDefault))
		FRigElementKey PlaneKey = FRigElementKey(FName(), ERigElementType::Control);


	// Cache
	UPROPERTY(Transient)
		FCachedRigElement ObjPlaneCache;
};



////////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * Bellcurve for Bulge
 */
USTRUCT(meta = (DisplayName = "Bulge Bell Curve", Category = "TGOR Utility", Keywords = "TGOR,Utility", PrototypeName = "BulgeBellCurve", NodeColor = "1.0 0.44 0.0"))
struct REALITIESANIMATION_API FTGORRigUnit_BulgeBellCurve : public FTGORRigUnit_Mutable
{
	GENERATED_BODY()

		FTGORRigUnit_BulgeBellCurve() {}

	RIGVM_METHOD()
		virtual void Execute(const FRigUnitContext& Context) override;

	virtual FString ProcessPinLabelForInjection(const FString& InLabel) const override;

public:

	/**
	 * Control used for bulge distance
	 */
	UPROPERTY(meta = (Input, ExpandByDefault))
		FRigElementKey ObjectKey = FRigElementKey(FName(), ERigElementType::Control);

	/**
	 * Bone to apply bulge to
	 */
	UPROPERTY(meta = (Input, ExpandByDefault))
		FRigElementKey BulgeKey = FRigElementKey(FName(), ERigElementType::Bone);

	/**
	 * Bellcurve variance
	 */
	UPROPERTY(meta = (Input))
		float Variance = 1.0f;

	/**
	 * Input radius ratio
	 */
	UPROPERTY(meta = (Input))
		float Radius = 1.0f;

	/**
	 * Bulge offset
	 */
	UPROPERTY(meta = (Input))
		FVector Offset = FVector::ZeroVector;

	// Cache
	UPROPERTY(Transient)
		FCachedRigElement ObjectCache;
	UPROPERTY(Transient)
		FCachedRigElement BulgeCache;
};