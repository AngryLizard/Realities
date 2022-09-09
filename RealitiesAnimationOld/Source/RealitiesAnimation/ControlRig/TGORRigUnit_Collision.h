// The Gateway of Realities: Planes of Existence.

#pragma once

#include "Units/RigUnit.h"
#include "TGORRigUnit_IK.h"
#include "TGORRigUnit_Utility.h"

#include "Animation/InputScaleBias.h"
#include "TGORRigUnit_Collision.generated.h"

/**
 * Ellipsoid node
 */
USTRUCT(BlueprintType, meta = (Abstract))
struct REALITIESANIMATION_API FTGORRigUnit_Ellipsoid : public FTGORRigUnit_ReadOnly
{
	GENERATED_BODY()

		FTGORRigUnit_Ellipsoid() {}
public:

	/**
	* Undeformed sphere radius
	*/
	UPROPERTY(meta = (Input))
		float Radius = 100.0f;

	/**
	 * Ellipsoid control to use for collision. Needs to be a sphere with radius 100
	 */
	UPROPERTY(meta = (Input, ExpandByDefault))
		FRigElementKey Key = FRigElementKey(FName(), ERigElementType::Control);

	// Cache
	UPROPERTY(Transient)
		FCachedRigElement Cache;
};

////////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * Closest point on an ellipsoid
 */
USTRUCT(meta = (DisplayName = "Ellipsoid Projection", Category = "TGOR EllipsoidCollision", Keywords = "TGOR,Collision", PrototypeName = "EllipsoidProjection", NodeColor = "1.0 0.44 0.0"))
struct REALITIESANIMATION_API FTGORRigUnit_EllipsoidProjection : public FTGORRigUnit_Ellipsoid
{
	GENERATED_BODY()

		FTGORRigUnit_EllipsoidProjection() {}

	RIGVM_METHOD()
		virtual void Execute(const FRigUnitContext& Context) override;

	

public:

	static void ComputeEllispoidProjection(const FTransform& Transform, float Radius, const FVector& Point, FVector& Closest, FVector& Normal);

	/**
	* Projection point
	*/
	UPROPERTY(meta = (Input))
		FVector Point = FVector::ZeroVector;

	/**
	* Closest point
	*/
	UPROPERTY(meta = (Output))
		FVector Closest = FVector::ZeroVector;

	/**
	* Impact normal
	*/
	UPROPERTY(meta = (Output))
		FVector Normal = FVector::ForwardVector;
};

////////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * Raycast on an ellipsoid
 */
USTRUCT(meta = (DisplayName = "Ellipsoid Raycast", Category = "TGOR EllipsoidCollision", Keywords = "TGOR,Collision", PrototypeName = "EllipsoidRaycast", NodeColor = "1.0 0.44 0.0"))
struct REALITIESANIMATION_API FTGORRigUnit_EllipsoidRaycast : public FTGORRigUnit_Ellipsoid
{
	GENERATED_BODY()

		FTGORRigUnit_EllipsoidRaycast() {}

	RIGVM_METHOD()
		virtual void Execute(const FRigUnitContext& Context) override;

	

public:

	static float ComputeEllispoidRaycast(const FTransform& Transform, float Radius, const FVector& Start, const FVector& End, FVector& Impact, FVector& Normal);

	/**
	* Raycast start point
	*/
	UPROPERTY(meta = (Input))
		FVector Start = FVector::ZeroVector;

	/**
	* Raycast end point
	*/
	UPROPERTY(meta = (Input))
		FVector End = FVector::ZeroVector;

	/**
	* Distance to the sphere if not intersecting
	*/
	UPROPERTY(meta = (Output))
		float Distance = 0.0f;

	/**
	* Impact point
	*/
	UPROPERTY(meta = (Output))
		FVector Impact = FVector::ZeroVector;

	/**
	* Impact normal
	*/
	UPROPERTY(meta = (Output))
		FVector Normal = FVector::ForwardVector;
};

////////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * Collides a line with
 */
USTRUCT(meta = (DisplayName = "Ellipsoid RayCollision", Category = "TGOR EllipsoidCollision", Keywords = "TGOR,Collision", PrototypeName = "EllipsoidRayCollision", NodeColor = "1.0 0.44 0.0"))
struct REALITIESANIMATION_API FTGORRigUnit_EllipsoidRayCollide : public FTGORRigUnit_Ellipsoid
{
	GENERATED_BODY()

		FTGORRigUnit_EllipsoidRayCollide() {}

	RIGVM_METHOD()
		virtual void Execute(const FRigUnitContext& Context) override;

	

public:
	
	static FVector ComputeEllispoidRayCollide(const FTransform& Transform, float Radius, const FVector& Start, const FVector& End, const FVector& Direction, float Adapt);

	/**
	* Line start
	*/
	UPROPERTY(meta = (Input))
		FVector Start = FVector::ZeroVector;

	/**
	* Line end
	*/
	UPROPERTY(meta = (Input))
		FVector End = FVector::ZeroVector;

	/**
	* Sweep direction
	*/
	UPROPERTY(meta = (Input))
		FVector Direction = FVector::ForwardVector;

	/**
	* Distance in which we smoothly lerp if outside collision radius instead of snapping
	*/
	UPROPERTY(meta = (Input, DetailsOnly))
		float Adapt = 50.0f;

	/**
	* Deflected point
	*/
	UPROPERTY(meta = (Output))
		FVector Deflect = FVector::ZeroVector;
};

////////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * Ellipsoid collision for a rotating chain
 */
USTRUCT(meta = (DisplayName = "Ellipsoid Chain Collision", Category = "TGOR EllipsoidCollision", Keywords = "TGOR,Collision", PrototypeName = "EllipsoidChainCollision", NodeColor = "1.0 0.44 0.0"))
struct REALITIESANIMATION_API FTGORRigUnit_EllipsoidChainCollide : public FTGORRigUnit_Mutable
{
	GENERATED_BODY()

		FTGORRigUnit_EllipsoidChainCollide() {}

	RIGVM_METHOD()
		virtual void Execute(const FRigUnitContext& Context) override;

	
public:

	/**
	* Undeformed sphere radius
	*/
	UPROPERTY(meta = (Input))
		float Radius = 100.0f;

	/**
	 * The chain to adapt (Has to be continuous chain)
	 */
	UPROPERTY(meta = (Input, ExpandByDefault))
		FRigElementKeyCollection Chain;

	/**
	 * Ellipsoid control to use for collision. Needs to be a sphere with radius 100
	 */
	UPROPERTY(meta = (Input, ExpandByDefault))
		FRigElementKey Key = FRigElementKey(FName(), ERigElementType::Control);

	/**
	* Ratio along each segment that should be used to determine collision distance
	*/
	UPROPERTY(meta = (Input, DetailsOnly))
		float NormalRatio = 0.5f;

	/**
	* Distance ratio at which chain will start to adapt
	*/
	UPROPERTY(meta = (Input, DetailsOnly))
		float Discovery = 0.25f;

	/**
	* Max angle against rotation angle
	*/
	UPROPERTY(meta = (Input))
		float MaxAngle = 20.0f;

	/**
	* Rotation axis for each segment
	*/
	UPROPERTY(meta = (Input, DetailsOnly))
		FVector RotationAxis = FVector::ForwardVector;

	/**
	* Rotation angle for each segment
	*/
	UPROPERTY(meta = (Input))
		float RotationAngle = 0.0f;

	// Cache
	UPROPERTY(Transient)
		FCachedRigElement Cache;
};

////////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * Ellipsoid projections for an objective transform
 */
USTRUCT(meta = (DisplayName = "Ellipsoid Transform Project", Category = "TGOR EllipsoidCollision", Keywords = "TGOR,Collision", PrototypeName = "EllipsoidTransformProject", NodeColor = "1.0 0.44 0.0"))
struct REALITIESANIMATION_API FTGORRigUnit_EllipsoidTransformProject : public FTGORRigUnit_Ellipsoid
{
	GENERATED_BODY()

		FTGORRigUnit_EllipsoidTransformProject() {}

	RIGVM_METHOD()
		virtual void Execute(const FRigUnitContext& Context) override;

	
public:

	/**
	 * Objective settings
	 */
	UPROPERTY(meta = (Input, DetailsOnly))
		FRigUnit_ObjectiveSettings ObjectiveSettings;

	/**
	* Target location/rotation for the chain end
	*/
	UPROPERTY(meta = (Input))
		FTransform Objective = FTransform::Identity;

	/**
	* How far away in up direction we poll for ellipsoid collision
	*/
	UPROPERTY(meta = (Input))
		float Discovery = 150.0f;

	/**
	* Max distance from ellipsoid where adaptation happens
	*/
	UPROPERTY(meta = (Input))
		float Distance = 50.0f;

	/**
	* Max angle to rotate hand
	*/
	UPROPERTY(meta = (Input))
		float MaxAngle = 30.0f;

	/**
	* Projected transform
	*/
	UPROPERTY(meta = (Output))
		FTransform Projection = FTransform::Identity;

	// Cache
	UPROPERTY(Transient)
		FCachedRigElement AnchorCache;
};