// The Gateway of Realities: Planes of Existence.

#pragma once

#include "Units/RigUnit.h"
#include "TGORRigUnit_Utility.h"

#include "TGORRigUnit_Triplet.generated.h"




/**
*
*/
USTRUCT(BlueprintType)
struct REALITIESANIMATION_API FTGOR_TripletAnalysis
{
	GENERATED_USTRUCT_BODY()

		FTGOR_TripletAnalysis();

	FQuat RotationBetween(const FTGOR_TripletAnalysis& Other) const;

	////////////////////////////////////////////////////////////////////////////////////////////////////

	/**  */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FVector Normal;

	/**  */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FVector Center;

	/**  */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FVector Direction;

	/**  */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FVector Tangent;

	/** Base distance from Center */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float Base;
};

////////////////////////////////////////////////////////////////////////////////////////////////////

/**
 *
 */
USTRUCT(BlueprintType, meta = (Abstract))
struct REALITIESANIMATION_API FTGORRigUnit_Cones : public FTGORRigUnit_ReadOnly
{
	GENERATED_BODY()

	/**
	 * Corresponding key triplet
	 */
	UPROPERTY(meta = (Input, ExpandByDefault))
		FRigElementKey BaseKey;
	UPROPERTY(meta = (Input, ExpandByDefault))
		FRigElementKey LeftKey;
	UPROPERTY(meta = (Input, ExpandByDefault))
		FRigElementKey RightKey;

	/**
	 * Key forward vector
	 */
	UPROPERTY(meta = (Input, DetailsOnly))
		FVector AimAxis = FVector::ForwardVector;

	// Cache
	UPROPERTY(Transient)
		FCachedRigElement BaseCache;
	UPROPERTY(Transient)
		FCachedRigElement LeftCache;
	UPROPERTY(Transient)
		FCachedRigElement RightCache;
};

////////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * 
 */
USTRUCT(meta = (DisplayName = "Triplet Analysis", Keywords = "TGOR,Utility", PrototypeName = "TripletAnalysis", NodeColor = "1.0 0.44 0.0"))
struct REALITIESANIMATION_API FTGORRigUnit_TripletAnalysis : public FTGORRigUnit_Cones
{
	GENERATED_BODY()

		FTGORRigUnit_TripletAnalysis() {}

	RIGVM_METHOD()
		virtual void Execute(const FRigUnitContext& Context) override;

	virtual FString ProcessPinLabelForInjection(const FString& InLabel) const override;

public:
	static void ComputeTripletPartial(const FVector& Base, const FVector& Left, const FVector& Right, FTGOR_TripletAnalysis& Analysis);
	static void ComputeTripletFinish(const FVector& Base, const FVector& Normal, FTGOR_TripletAnalysis& Analysis);
	static void ComputeTripletAnalysis(const FVector& Base, const FVector& Left, const FVector& Right, const FVector& Normal, FTGOR_TripletAnalysis& Analysis);

	/**
	 * Aligned rotation
	 */
	UPROPERTY(meta = (Output))
		FTGOR_TripletAnalysis Output;
};

////////////////////////////////////////////////////////////////////////////////////////////////////

/**
 *
 */
USTRUCT(meta = (DisplayName = "Triplet Reproject", Keywords = "TGOR,Utility", PrototypeName = "TripletReproject", NodeColor = "1.0 0.44 0.0"))
struct REALITIESANIMATION_API FTGORRigUnit_TripletReproject : public FTGORRigUnit_Cones
{
	GENERATED_BODY()

		FTGORRigUnit_TripletReproject() {}

	RIGVM_METHOD()
		virtual void Execute(const FRigUnitContext& Context) override;

	virtual FString ProcessPinLabelForInjection(const FString& InLabel) const override;

public:

	/**
	 * Key used as output reference
	 */
	UPROPERTY(meta = (Input, ExpandByDefault))
		FRigElementKey RefKey;

	/**
	 * Corresponding cone analysis
	 */
	UPROPERTY(meta = (Input))
		FTGOR_TripletAnalysis Analysis;

	/**
	 * Pivot to reproject around
	 */
	UPROPERTY(meta = (Input, ExpandByDefault))
		FRigElementKey PivotKey = FRigElementKey(FName(), ERigElementType::Control);

	/**
	 * Reprojected transform
	 */
	UPROPERTY(meta = (Output))
		FTransform Output;

	// Cache
	UPROPERTY(Transient)
		FCachedRigElement RefCache;
	UPROPERTY(Transient)
		FCachedRigElement PivotCache;
};


////////////////////////////////////////////////////////////////////////////////////////////////////

/**
 *
 */
USTRUCT(meta = (DisplayName = "Triplet Transform", Keywords = "TGOR,Utility", PrototypeName = "TripletTransform", NodeColor = "1.0 0.44 0.0"))
struct REALITIESANIMATION_API FTGORRigUnit_TripletTransform : public FTGORRigUnit_ReadOnly
{
	GENERATED_BODY()

		FTGORRigUnit_TripletTransform() {}

	RIGVM_METHOD()
		virtual void Execute(const FRigUnitContext& Context) override;

	virtual FString ProcessPinLabelForInjection(const FString& InLabel) const override;

public:

	/**
	 * Key used as output reference
	 */
	UPROPERTY(meta = (Input, ExpandByDefault))
		FRigElementKey RefKey;

	/**
	 * Corresponding cone analysis
	 */
	UPROPERTY(meta = (Input))
		FTGOR_TripletAnalysis Analysis;

	/**
	 * Pivot to reproject around
	 */
	UPROPERTY(meta = (Input, ExpandByDefault))
		FRigElementKey PivotKey = FRigElementKey(FName(), ERigElementType::Control);

	/**
	 * Transformed location
	 */
	UPROPERTY(meta = (Output))
		FVector Output = FVector::ZeroVector;

	// Cache
	UPROPERTY(Transient)
		FCachedRigElement RefCache;
	UPROPERTY(Transient)
		FCachedRigElement PivotCache;
};

