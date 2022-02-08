// The Gateway of Realities: Planes of Existence.

#pragma once

#include "RealitiesUtility/Structures/TGOR_Time.h"
#include "RealitiesUtility/Structures/TGOR_Normal.h"

#include "CoreSystem/Storage/TGOR_Serialisation.h"
#include "CoreSystem/Storage/TGOR_PackageNetwork.h"
#include "CoreSystem/Storage/TGOR_PackageGroup.h"

#include "Engine/UserDefinedStruct.h"
#include "TGOR_MobilityInstance.generated.h"

/**
*
*/
USTRUCT(BlueprintType)
struct DIMENSIONSYSTEM_API FTGOR_MovementThreshold
{
	GENERATED_USTRUCT_BODY()

	FTGOR_MovementThreshold();
	FTGOR_MovementThreshold(const FTGOR_MovementThreshold& Other, float Factor);

	///////////////////////////////////////////

	float ThresholdDistance(const FVector& A, const FVector& B, float SpeedSquared) const;
	float ThresholdDistance(float A, float B, float SpeedSquared) const;
	float ThresholdAngle(const FVector& A, const FVector& B, float SpeedSquared) const;
	float ThresholdAngle(const FQuat& A, const FQuat& B, float SpeedSquared) const;
	float ThresholdTime(const FTGOR_Time& A, const FTGOR_Time& B) const;

	///////////////////////////////////////////

	/** Threshold multiplier depending on speed */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "!TGOR Movement")
		float SpeedMultiplier;

	/** Distance threshold */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "!TGOR Movement")
		float DistanceThreshold;

	/** Angle threshold (cosine space) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "!TGOR Movement")
		float AngleThreshold;

	/** Time threshold */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "!TGOR Movement")
		float TimeThreshold;
};

/**
*
*/
USTRUCT(BlueprintType)
struct DIMENSIONSYSTEM_API FTGOR_MovementForce
{
	GENERATED_USTRUCT_BODY()

		FTGOR_MovementForce();
	void SetLerpForce(const FTGOR_MovementForce& Other, float Alpha);
	float CompareForce(const FTGOR_MovementForce& Other, const FTGOR_MovementThreshold& Threshold) const;

	////////////////////////////////////////////////////////////////////////////////////////////////////

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FVector Force;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FVector Torque;
};


// Declaration for BaseToSpace output
struct FTGOR_MovementSpace;

/**
*
*/
USTRUCT(BlueprintType)
struct DIMENSIONSYSTEM_API FTGOR_MovementPosition
{
	GENERATED_USTRUCT_BODY()

		FTGOR_MovementPosition();
	FTGOR_MovementPosition(const FVector& Linear, const FQuat& Angular);
	void SetLerpPosition(const FTGOR_MovementPosition& Other, float Alpha);
	float ComparePosition(const FTGOR_MovementPosition& Other, const FTGOR_MovementThreshold& Threshold) const;

	////////////////////////////////////////////////////////////////////////////////////////////////////

	/** Compute world space from parent and local base */
	FTGOR_MovementPosition BaseToPosition(const FTGOR_MovementPosition& ParentPosition) const;

	/** Compute world space from parent and local base */
	FTGOR_MovementSpace BaseToSpace(const FTGOR_MovementDynamic& ParentDynamic) const;

	/** Set local base from position and parent world space */
	void PositionToBase(const FTGOR_MovementPosition& ParentPosition, const FTGOR_MovementPosition& Position);

	/** Tansform local position to world */
	FVector TransformPosition(const FVector& Local) const;

	/** Inverse transform world position to local */
	FVector InverseTransformPosition(const FVector& World) const;

	/** Tansform local vector to world */
	FVector TransformVector(const FVector& Local) const;

	/** Inverse transform world vector to local */
	FVector InverseTransformVector(const FVector& World) const;

	////////////////////////////////////////////////////////////////////////////////////////////////////

	/** Location in cm */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FVector Linear;

	/** Rotation in radians */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FQuat Angular;
};

/**
*
*/
USTRUCT(BlueprintType)
struct DIMENSIONSYSTEM_API FTGOR_MovementDynamic : public FTGOR_MovementPosition
{
	GENERATED_USTRUCT_BODY()

		FTGOR_MovementDynamic();
	FTGOR_MovementDynamic(const FTGOR_MovementPosition& Position);

	void Write(FTGOR_GroupWritePackage& Package, UTGOR_Singleton* Context) const;
	void Read(FTGOR_GroupReadPackage& Package, UTGOR_Singleton* Context);
	void Send(FTGOR_NetworkWritePackage& Package, UTGOR_Singleton* Context) const;
	void Recv(FTGOR_NetworkReadPackage& Package, UTGOR_Singleton* Context);

	void SetLerpDynamic(const FTGOR_MovementDynamic& Other, float Alpha);
	float CompareDynamic(const FTGOR_MovementDynamic& Other, const FTGOR_MovementThreshold& Threshold) const;

	////////////////////////////////////////////////////////////////////////////////////////////////////

	FVector TurningVelocity(const FVector& Point) const;
	FVector PointVelocity(const FVector& Point) const;

	/** Compute world space from parent and local base */
	FTGOR_MovementSpace BaseToSpace(const FTGOR_MovementDynamic& ParentDynamic) const;

	/** Set local base from transform and parent world space */
	void DynamicToBase(const FTGOR_MovementDynamic& ParentDynamic, const FTGOR_MovementDynamic& Dynamic);

	/** Compute dynamics of a local position */
	FTGOR_MovementDynamic TransformPosition(const FTGOR_MovementPosition& Position) const;


	////////////////////////////////////////////////////////////////////////////////////////////////////

	/** Velocity in cm/s */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FVector LinearVelocity;

	/** Velocity in rad/s */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FVector AngularVelocity;

	/** Velocity in cm^2/s^2 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FVector LinearAcceleration;

	/** Velocity in rad^2/s^2 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FVector AngularAcceleration;
};

/**
* Current movement situation relative to current movement base
*/
USTRUCT(BlueprintType)
struct DIMENSIONSYSTEM_API FTGOR_MovementSpace : public FTGOR_MovementDynamic
{
	GENERATED_USTRUCT_BODY()

		FTGOR_MovementSpace();
	FTGOR_MovementSpace(const FTGOR_MovementDynamic& Other);

	// Sets linear velocity while also adapting RelativeLinearVelocity
	void SetLinearVelocity(const FVector& Velocity);

	// Sets angular velocity while also adapting RelativeAngularVelocity
	void SetAngularVelocity(const FVector& Velocity);

	// Adds linear velocity while also adapting RelativeLinearVelocity
	void AddLinearVelocity(const FVector& VelocityDelta);

	// Adds angular velocity while also adapting RelativeAngularVelocity
	void AddAngularVelocity(const FVector& VelocityDelta);

	////////////////////////////////////////////////////////////////////////////////////////////////////

	/** Relative velocity in cm/s */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FVector RelativeLinearVelocity;

	/** Relative velocity in rad/s */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FVector RelativeAngularVelocity;

	/** Relative acceleration in cm/s */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FVector RelativeLinearAcceleration;

	/** Relative acceleration in rad/s */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FVector RelativeAngularAcceleration;
};

/**
*
*/
USTRUCT(BlueprintType)
struct DIMENSIONSYSTEM_API FTGOR_PhysicsProperties
{
	GENERATED_USTRUCT_BODY()

		FTGOR_PhysicsProperties();
	void SetLerpProperties(const FTGOR_PhysicsProperties& Other, float Alpha);

	////////////////////////////////////////////////////////////////////////////////////////////////////

	/** Gravity vector */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FVector Gravity;

	/** Material density */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float Density;

	/** Material drag */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float DragCoeff;

	/** Current depth in current physics volume */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float Depth;

	/** Current velocity in current physics volume */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FVector Velocity;

	/** Current vorticity in current physics volume */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FVector Vorticity;
};

/**
*
*/
USTRUCT(BlueprintType)
struct DIMENSIONSYSTEM_API FTGOR_MovementExternal : public FTGOR_MovementForce
{
	GENERATED_USTRUCT_BODY()

		FTGOR_MovementExternal();

	////////////////////////////////////////////////////////////////////////////////////////////////////

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FTGOR_PhysicsProperties Surroundings;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FVector UpVector;
};

/**
*
*/
USTRUCT(BlueprintType)
struct DIMENSIONSYSTEM_API FTGOR_MovementDelta
{
	GENERATED_USTRUCT_BODY()

		FTGOR_MovementDelta();
	void SetLerpDelta(const FTGOR_MovementDelta& Other, float Alpha);
	float CompareDelta(const FTGOR_MovementDelta& Other, const FTGOR_MovementThreshold& Threshold) const;

	////////////////////////////////////////////////////////////////////////////////////////////////////

	/** External force in kg*m/s */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FVector LinearMomentum;

	/** External force in kg*m^2/s */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FVector AngularMomentum;
};

/**
*
*/
USTRUCT(BlueprintType)
struct DIMENSIONSYSTEM_API FTGOR_MovementDamper
{
	GENERATED_USTRUCT_BODY()

		FTGOR_MovementDamper();

	////////////////////////////////////////////////////////////////////////////////////////////////////

	/** Damping that occured this frame */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float Damping;

	/** Total Force/Torque */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FVector Vector;
};

/**
*
*/
USTRUCT(BlueprintType)
struct DIMENSIONSYSTEM_API FTGOR_MovementShape
{
	GENERATED_USTRUCT_BODY()
		SERIALISE_INIT_HEADER;

	FTGOR_MovementShape();

	void Send(FTGOR_NetworkWritePackage& Package, UTGOR_Singleton* Context) const;
	void Recv(FTGOR_NetworkReadPackage& Package, UTGOR_Singleton* Context);

	void SetLerpShape(const FTGOR_MovementShape& Other, float Alpha);

	////////////////////////////////////////////////////////////////////////////////////////////////////

	/** Current radius in cm */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "!TGOR Movement")
		float Radius;

	/** Current height in cm */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "!TGOR Movement")
		float Height;

	/** Current volume m^3 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "!TGOR Movement")
		float Volume;
};

template<> struct TStructOpsTypeTraits<FTGOR_MovementShape> : public TStructOpsTypeTraitsBase2<FTGOR_MovementShape>
{
	enum
	{
		WithNetSerializer = true
	};
};

/**
*
*/
USTRUCT(BlueprintType)
struct DIMENSIONSYSTEM_API FTGOR_MovementBody : public FTGOR_MovementShape
{
	GENERATED_USTRUCT_BODY()
		SERIALISE_INIT_HEADER;

	FTGOR_MovementBody();

	void Send(FTGOR_NetworkWritePackage& Package, UTGOR_Singleton* Context) const;
	void Recv(FTGOR_NetworkReadPackage& Package, UTGOR_Singleton* Context);

	void SetLerpBody(const FTGOR_MovementBody& Other, float Alpha);

	////////////////////////////////////////////////////////////////////////////////////////////////////
public:

	/** Fills movement frame properties from a given box */
	void SetFromBox(const FVector& SurfaceSparsity, const FVector& Dimensions, float Weight);

	/** Fills movement frame properties from a given sphere */
	void SetFromSphere(const FVector& SurfaceSparsity, float BodyRadius, float Weight);

	/** Fills movement frame properties from a given capsule */
	void SetFromCapsule(const FVector& SurfaceSparsity, float BodyRadius, float BodyHalfHeight, float Weight);

	////////////////////////////////////////////////////////////////////////////////////////////////////

	/** Get current compression ratio depending on depth */
	float GetCompressionRatio(const FTGOR_PhysicsProperties& Surroundings) const;

	/** Get mass of matter of surroundings */
	float GetDisplacedMass(const FTGOR_PhysicsProperties& Surroundings) const;

	/** Get bouyancy ratio to determined how much we're currently floating */
	float GetBouyancyRatio(const FTGOR_PhysicsProperties& Surroundings) const;

	/** Drag response computation given a velocity and drag coefficient. */
	float ComputeDragResponse(const FTGOR_MovementPosition& Position, const FTGOR_PhysicsProperties& Surroundings, const FVector& RelativeVelocity, float Coeff) const;

	/** External force applied to a body */
	FTGOR_MovementDamper ComputeExternalForce(const FTGOR_MovementSpace& Space, const FTGOR_PhysicsProperties& Surroundings) const;

	/** External torque applied to a body */
	FTGOR_MovementDamper ComputeExternalTorque(const FTGOR_MovementSpace& Space, const FTGOR_PhysicsProperties& Surroundings) const;

	/** External torque applied to a body */
	FTGOR_MovementSpace SimulateForce(const FTGOR_MovementSpace& Space, const FTGOR_MovementForce& Force, const FTGOR_MovementForce& External, float DeltaTime) const;

	////////////////////////////////////////////////////////////////////////////////////////////////////

	/** Get linear acceleration/velocity from force/momentum */
	FVector GetUnmassedLinear(const FVector& Vector) const;

	/** Get angular acceleration/velocity from torque/momentum given current rotation */
	FVector GetUnmassedAngular(const FQuat& Rotation, const FVector& Vector) const;

	/** Get linear force/momentum from acceleration/velocity */
	FVector GetMassedLinear(const FVector& Vector) const;

	/** Get angular torque/momentum from acceleration/velocity given current rotation */
	FVector GetMassedAngular(const FQuat& Rotation, const FVector& Vector) const;

	////////////////////////////////////////////////////////////////////////////////////////////////////

	/** Current mass in kg */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "!TGOR Movement")
		float Mass;

	/** Inertia tensor diagonal in rad/kgm^2  */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "!TGOR Movement")
		FVector Inertia;

	/** Surface area from each axis in m^2 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "!TGOR Movement")
		FVector Faces;

	/** Core component elasticity on collision response [0, 1] */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "!TGOR Movement")
		FTGOR_Normal Elasticity;

	/** Core component friction coefficient [0, 1] */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "!TGOR Movement")
		FTGOR_Normal Friction;
};

template<> struct TStructOpsTypeTraits<FTGOR_MovementBody> : public TStructOpsTypeTraitsBase2<FTGOR_MovementBody>
{
	enum
	{
		WithNetSerializer = true
	};
};
