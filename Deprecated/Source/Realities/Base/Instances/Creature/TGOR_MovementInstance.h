// The Gateway of Realities: Planes of Existence.

#pragma once

#include "Realities/Utility/Error/TGOR_Error.h"
#include "Realities/Utility/Structs/TGOR_Index.h"
#include "Realities/Utility/Datastructures/TGOR_Time.h"
#include "Realities/Utility/TGOR_CustomEnumerations.h"
#include "Realities/Base/Instances/Inventory/TGOR_SocketInstance.h"

#include "Realities/Utility/Storage/TGOR_Serialisation.h"
#include "Realities/Utility/Storage/TGOR_PackageNetwork.h"

#include "Engine/UserDefinedStruct.h"
#include "TGOR_MovementInstance.generated.h"

class UTGOR_Socket;
class UTGOR_Movement;
class UTGOR_Animation;
class UTGOR_MovementComponent;
class UTGOR_MobilityComponent;
class UTGOR_PilotComponent;
class ATGOR_PhysicsVolume;

/**
*
*/
USTRUCT(BlueprintType)
struct REALITIES_API FTGOR_PositionThreshold
{
	GENERATED_USTRUCT_BODY()

	FTGOR_PositionThreshold();
	FTGOR_PositionThreshold(float Location, float Rotation);
	FTGOR_PositionThreshold(const FTGOR_PositionThreshold& Other, float Multiplier);

	////////////////////////////////////////////////////////////////////////////////////////////////////

	/** Location threshold */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TGOR Movement")
		float Location;

	/** Rotation (radians) threshold */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TGOR Movement")
		float Rotation;
};

/**
*
*/
USTRUCT(BlueprintType)
struct REALITIES_API FTGOR_MovementThreshold : public FTGOR_PositionThreshold
{
	GENERATED_USTRUCT_BODY()

	FTGOR_MovementThreshold();
	FTGOR_MovementThreshold(float Location, float Rotation, float LinearVelocity, float AngularVelocity, float VelocityPositionMultiplier, float TimeThreshold);
	FTGOR_MovementThreshold(const FTGOR_MovementThreshold& Other, float Multiplier);
	FTGOR_PositionThreshold operator()(float Linear, float Angular) const;
	///////////////////////////////////////////

	/** Velocity threshold */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TGOR Movement")
		float LinearVelocity;

	/** Velocity (radians) threshold */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TGOR Movement")
		float AngularVelocity;

	/** How much the position threshold increases with velocity */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TGOR Movement")
		float VelocityPositionMultiplier;

	/** Time (seconds) threshold */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TGOR Movement")
		float TimeThreshold;
};

/**
*
*/
USTRUCT(BlueprintType)
struct REALITIES_API FTGOR_MovementForce
{
	GENERATED_USTRUCT_BODY()

		FTGOR_MovementForce();
	FTGOR_MovementForce Lerp(const FTGOR_MovementForce& Other, float Alpha) const;

	////////////////////////////////////////////////////////////////////////////////////////////////////

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FVector Force;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FVector Torque;
};

/**
*
*/
USTRUCT(BlueprintType)
struct REALITIES_API FTGOR_MovementExternal : public FTGOR_MovementForce
{
	GENERATED_USTRUCT_BODY()

	FTGOR_MovementExternal();

	////////////////////////////////////////////////////////////////////////////////////////////////////

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FVector UpVector;
};

/**
*
*/
USTRUCT(BlueprintType)
struct REALITIES_API FTGOR_MovementPosition
{
	GENERATED_USTRUCT_BODY()

		FTGOR_MovementPosition();
	FTGOR_MovementPosition Lerp(const FTGOR_MovementPosition& Other, float Alpha) const;
	float Compare(const FTGOR_MovementPosition& Position, const FTGOR_PositionThreshold& Threshold) const;

	////////////////////////////////////////////////////////////////////////////////////////////////////

	/** Compute world space from parent and local base */
	FTGOR_MovementPosition BaseToPosition(const FTGOR_MovementPosition& ParentSpace) const;

	/** Set local base from position and parent world space */
	void PositionToBase(const FTGOR_MovementPosition& ParentPosition, const FTGOR_MovementPosition& Position);

	////////////////////////////////////////////////////////////////////////////////////////////////////

	/** Location in cm */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FVector Linear;

	/** Rotation in radians */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FQuat Angular;
};

// Declaration for BaseToSpace output
struct FTGOR_MovementSpace;

/**
*
*/
USTRUCT(BlueprintType)
struct REALITIES_API FTGOR_MovementDynamic : public FTGOR_MovementPosition
{
	GENERATED_USTRUCT_BODY()

	FTGOR_MovementDynamic();
	FTGOR_MovementDynamic Lerp(const FTGOR_MovementDynamic& Other, float Alpha) const;

	////////////////////////////////////////////////////////////////////////////////////////////////////

	FVector TurningVelocity(const FVector& Point) const;
	FVector PointVelocity(const FVector& Point) const;

	/** Compute world space from parent and local base */
	FTGOR_MovementSpace BaseToSpace(const FTGOR_MovementDynamic& ParentDynamic) const;

	/** Set local base from transform and parent world space */
	void DynamicToBase(const FTGOR_MovementDynamic& ParentDynamic, const FTGOR_MovementDynamic& Dynamic);

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
struct REALITIES_API FTGOR_MovementSpace : public FTGOR_MovementDynamic
{
	GENERATED_USTRUCT_BODY()

		FTGOR_MovementSpace();

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
struct REALITIES_API FTGOR_MovementBase : public FTGOR_MovementDynamic
{
	GENERATED_USTRUCT_BODY()
		SERIALISE_INIT_HEADER;

	FTGOR_MovementBase();
	FTGOR_MovementBase(UTGOR_MobilityComponent*  Component);

	void Send(FTGOR_NetworkWritePackage& Package, UTGOR_Singleton* Context) const;
	void Recv(FTGOR_NetworkReadPackage& Package, UTGOR_Singleton* Context);

	FTGOR_MovementBase Lerp(const FTGOR_MovementBase& Other, float Alpha) const;
	float Compare(const FTGOR_MovementBase& Base, const FTGOR_MovementThreshold& Threshold) const;
	bool HasCycle() const;
	
	// Sets networked values
	void Adjust(const FTGOR_MovementBase& Base);

	////////////////////////////////////////////////////////////////////////////////////////////////////

	/** Compute world space recursively */
	FTGOR_MovementPosition ComputePosition() const;

	/** Compute parent world space recursively */
	FTGOR_MovementPosition ComputeParentPosition() const;

	////////////////////////////////////////////////////////////////////////////////////////////////////

	/** Compute world space recursively */
	FTGOR_MovementSpace ComputeSpace() const;

	/** Compute parent world space recursively */
	FTGOR_MovementSpace ComputeParentSpace() const;


	////////////////////////////////////////////////////////////////////////////////////////////////////

	/** Parent component index (-1 for no component), used e.g. for bone index for skeletal mesh */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FTGOR_Index Index;

	/** Parent socket type we're attached to (if attached at all) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		UTGOR_Socket* Socket;

	/** Base of this base */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TWeakObjectPtr<UTGOR_MobilityComponent> Parent;

	/** Parenting data */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FTGOR_SocketInstance Stage;

	/** Own Component this base is attached to */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TWeakObjectPtr<UTGOR_MobilityComponent> Component;
};

template<> struct TStructOpsTypeTraits<FTGOR_MovementBase> : public TStructOpsTypeTraitsBase2<FTGOR_MovementBase>
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
struct REALITIES_API FTGOR_MovementRecord
{
	GENERATED_USTRUCT_BODY()

	FTGOR_MovementRecord();
	FTGOR_MovementRecord(const FTGOR_MovementBase& Base);

	FTGOR_MovementRecord Lerp(const FTGOR_MovementRecord& Other, float Alpha) const;

	void Simulate() const;
	const FTGOR_MovementBase& GetRoot() const;
	void SetRoot(const FTGOR_MovementBase& Base);

	/** Movement base root */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FTGOR_MovementBase Root;

	/** Movement base parent */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FTGOR_MovementDynamic Parent;
};


/**
* Current movement situation relative to current tick
*/
USTRUCT(BlueprintType)
struct REALITIES_API FTGOR_MovementTick
{
	GENERATED_USTRUCT_BODY()

		FTGOR_MovementTick();

	////////////////////////////////////////////////////////////////////////////////////////////////////

	/** Amount of time to simulate */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float Deltatime;

	/** Max damping coefficient for this frame to prevent oscillation */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float MaxLinearDamping;

	/** Max damping coefficient for this frame to prevent oscillation */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float MaxAngularDamping;
};

/**
*
*/
USTRUCT(BlueprintType)
struct REALITIES_API FTGOR_MovementDelta
{
	GENERATED_USTRUCT_BODY()

		FTGOR_MovementDelta();
	FTGOR_MovementDelta Lerp(const FTGOR_MovementDelta& Other, float Alpha) const;

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
struct REALITIES_API FTGOR_MovementDamper
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
struct REALITIES_API FTGOR_MovementOutput : public FTGOR_MovementForce
{
	GENERATED_USTRUCT_BODY()

	FTGOR_MovementOutput();
	void SetBaseFrom(AActor* Owner, const FName& Name);
	void SetBaseFrom(UActorComponent* Component, const FName& Name);
	void SetBaseFromVolume(ATGOR_PhysicsVolume* Volume, const FName& Name);

	////////////////////////////////////////////////////////////////////////////////////////////////////

	/** Base component */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		UTGOR_MobilityComponent* Base;

	/** Index of the part that has been hit (e.g. bones) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		int32 Index;

	/** Location in euclidean space */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FVector BaseImpactPoint;

	/** Orientation force in kg*m^2/s^2 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FVector Orientation;

	/** Whether to use collision detection this frame */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		bool CollisionEnabled;

	/** Max damping that occured this frame */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float MaxLinearDamping;

	/** Max damping that occured this frame */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float MaxAngularDamping;
};

/**
*
*/
USTRUCT(BlueprintType)
struct REALITIES_API FTGOR_MovementGround
{
	GENERATED_USTRUCT_BODY()

		FTGOR_MovementGround();

	////////////////////////////////////////////////////////////////////////////////////////////////////

	/** Up direction along surface in euclidean space */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FVector SurfaceNormal;

	/** Offset from ground to center of mass */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FVector SurfaceOffset;

	/** Surface slope multiplier in [0, 1]*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float Slope;

	/** Distance from ground in cm */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float GroundDistance;

	/** Ground primitive */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		UPrimitiveComponent* Component;

	/** Ground name (e.g. bone) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FName Name;
};

/**
*
*/
USTRUCT(BlueprintType)
struct REALITIES_API FTGOR_MovementContact : public FTGOR_MovementGround
{
	GENERATED_USTRUCT_BODY()

		FTGOR_MovementContact();

	////////////////////////////////////////////////////////////////////////////////////////////////////

	/** Up direction of current movement frame in euclidean space */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FVector FrameNormal;
	
	/** Right direction along frame in euclidean space */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FVector FrameRight;

	/** Forward direction along frame in euclidean space */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FVector FrameForward;

	/** Input along frame in euclidean space */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FVector FrameInput;

	/** Relative velocity along frame in cm/s */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FVector FrameVelocity;

	/** Relative velocity along frame in rad/s */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FVector FrameAngular;

	/** Relative velocity up the frame in rad/s */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float VerticalSpeed;
};


/**
*
*/
USTRUCT(BlueprintType)
struct FTGOR_VisualContact
{
	GENERATED_USTRUCT_BODY()

public:
	FTGOR_VisualContact();

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FVector Location;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FVector Normal;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		bool Blocking;
};

/**
*
*/
USTRUCT(BlueprintType)
struct REALITIES_API FTGOR_MovementRepel
{
	GENERATED_USTRUCT_BODY()

		FTGOR_MovementRepel();
	FVector Project(const FVector& Direction, float Threshold = SMALL_NUMBER) const;

	////////////////////////////////////////////////////////////////////////////////////////////////////

	/** Repel direction in euclidean space */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FVector RepelNormal;

	/** Repel force in kg*m^2/s^2 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FVector RepelForce;

	/** Whether Repel hit */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		bool RepelHit;
};


/**
*
*/
USTRUCT(BlueprintType)
struct REALITIES_API FTGOR_PhysicsProperties
{
	GENERATED_USTRUCT_BODY()

	FTGOR_PhysicsProperties();
	FTGOR_PhysicsProperties Lerp(const FTGOR_PhysicsProperties& Other, float Alpha) const;

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
struct REALITIES_API FTGOR_MovementShape
{
	GENERATED_USTRUCT_BODY()
	SERIALISE_INIT_HEADER;

	FTGOR_MovementShape();

	void Send(FTGOR_NetworkWritePackage& Package, UTGOR_Singleton* Context) const;
	void Recv(FTGOR_NetworkReadPackage& Package, UTGOR_Singleton* Context);

	FTGOR_MovementShape Lerp(const FTGOR_MovementShape& Other, float Alpha) const;

	////////////////////////////////////////////////////////////////////////////////////////////////////

	/** Current radius in cm */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TGOR Movement")
		float Radius;

	/** Current height in cm */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TGOR Movement")
		float Height;

	/** Current volume m^3 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TGOR Movement")
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
struct REALITIES_API FTGOR_MovementBody : public FTGOR_MovementShape
{
	GENERATED_USTRUCT_BODY()
		SERIALISE_INIT_HEADER;

	FTGOR_MovementBody();

	void Send(FTGOR_NetworkWritePackage& Package, UTGOR_Singleton* Context) const;
	void Recv(FTGOR_NetworkReadPackage& Package, UTGOR_Singleton* Context);

	FTGOR_MovementBody Lerp(const FTGOR_MovementBody& Other, float Alpha) const;



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

	/** Drag response computation given a velocity and drag coefficient. */
	float ComputeDragResponse(const FTGOR_MovementPosition& Position, const FTGOR_PhysicsProperties& Surroundings, const FVector& RelativeVelocity, float Coeff) const;

	/** External force applied to a body */
	FTGOR_MovementDamper ComputeExternalForce(const FTGOR_MovementSpace& Space, const FTGOR_PhysicsProperties& Surroundings) const;

	/** External torque applied to a body */
	FTGOR_MovementDamper ComputeExternalTorque(const FTGOR_MovementSpace& Space, const FTGOR_PhysicsProperties& Surroundings) const;

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
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TGOR Movement")
		float Mass;

	/** Inertia tensor diagonal in rad/kgm^2  */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TGOR Movement")
		FVector Inertia;

	/** Surface area from each axis in m^2 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TGOR Movement")
		FVector Faces;
};

template<> struct TStructOpsTypeTraits<FTGOR_MovementBody> : public TStructOpsTypeTraitsBase2<FTGOR_MovementBody>
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
struct REALITIES_API FTGOR_MovementFrame
{
	GENERATED_USTRUCT_BODY()
	SERIALISE_INIT_HEADER;

	FTGOR_MovementFrame();

	void Send(FTGOR_NetworkWritePackage& Package, UTGOR_Singleton* Context) const;
	void Recv(FTGOR_NetworkReadPackage& Package, UTGOR_Singleton* Context);

	FTGOR_MovementFrame Lerp(const FTGOR_MovementFrame& Other, float Alpha) const;

	////////////////////////////////////////////////////////////////////////////////////////////////////

	/** Scale factor for movement speed */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TGOR Movement")
		float Agility;

	/** Scale factor for movement strength */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TGOR Movement")
		float Strength;
};

template<> struct TStructOpsTypeTraits<FTGOR_MovementFrame> : public TStructOpsTypeTraitsBase2<FTGOR_MovementFrame>
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
struct REALITIES_API FTGOR_MovementState
{
	GENERATED_USTRUCT_BODY()
	SERIALISE_INIT_HEADER;

	FTGOR_MovementState();

	void Send(FTGOR_NetworkWritePackage& Package, UTGOR_Singleton* Context) const;
	void Recv(FTGOR_NetworkReadPackage& Package, UTGOR_Singleton* Context);

	FTGOR_MovementState Lerp(const FTGOR_MovementState& Other, float Alpha) const;
	float Compare(const FTGOR_MovementState& State, double InputThres) const;
	
	////////////////////////////////////////////////////////////////////////////////////////////////////
	
	/** Input vector in euclidean space */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FVector Input;

	/** View vector in euclidean space */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FVector View;
};

template<>
struct TStructOpsTypeTraits<FTGOR_MovementState> : public TStructOpsTypeTraitsBase2<FTGOR_MovementState>
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
struct REALITIES_API FTGOR_MovementContent
{
	GENERATED_USTRUCT_BODY()
	SERIALISE_INIT_HEADER;

	FTGOR_MovementContent();

	void Send(FTGOR_NetworkWritePackage& Package, UTGOR_Singleton* Context) const;
	void Recv(FTGOR_NetworkReadPackage& Package, UTGOR_Singleton* Context);

	FTGOR_MovementContent Lerp(const FTGOR_MovementContent& Other, float Alpha) const;
	float Compare(const FTGOR_MovementContent& State) const;

	////////////////////////////////////////////////////////////////////////////////////////////////////

	/** Current active movement */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		UTGOR_Movement* Movement;

	/** Current active animation */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		UTGOR_Animation* Animation;

	/** Whether movement is currently being overridden (used by preemption e.g. for assigned movement modes) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		bool HasOverride;
};

template<>
struct TStructOpsTypeTraits<FTGOR_MovementContent> : public TStructOpsTypeTraitsBase2<FTGOR_MovementContent>
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
struct REALITIES_API FTGOR_MovementCapture
{
	GENERATED_USTRUCT_BODY()
	SERIALISE_INIT_HEADER;

	FTGOR_MovementCapture();

	void Send(FTGOR_NetworkWritePackage& Package, UTGOR_Singleton* Context) const;
	void Recv(FTGOR_NetworkReadPackage& Package, UTGOR_Singleton* Context);

	FTGOR_MovementCapture Lerp(const FTGOR_MovementCapture& Other, float Alpha) const;
	float Compare(const FTGOR_MovementCapture& State, float AngleThres) const;

	////////////////////////////////////////////////////////////////////////////////////////////////////

	/** Current physics volume */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TWeakObjectPtr<ATGOR_PhysicsVolume> PhysicsVolume;

	/** Current surrounding physics properties */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FTGOR_PhysicsProperties Surroundings;

	/** Current Up direction in world space */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FVector UpVector;
};

template<>
struct TStructOpsTypeTraits<FTGOR_MovementCapture> : public TStructOpsTypeTraitsBase2<FTGOR_MovementCapture>
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
struct REALITIES_API FTGOR_MovementUpdate
{
	GENERATED_USTRUCT_BODY()
	SERIALISE_INIT_HEADER;

		FTGOR_MovementUpdate();

		virtual void Send(FTGOR_NetworkWritePackage& Package, UTGOR_Singleton* Context) const;
		virtual void Recv(FTGOR_NetworkReadPackage& Package, UTGOR_Singleton* Context);

	////////////////////////////////////////////////////////////////////////////////////////////////////

	/** Recorded timestamp */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FTGOR_Time Timestamp;

	/** Everything in movement base shall be sent */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FTGOR_MovementBase Base;

	/** Current orientation needs updating as well */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FVector UpVector;
};

template<>
struct TStructOpsTypeTraits<FTGOR_MovementUpdate> : public TStructOpsTypeTraitsBase2<FTGOR_MovementUpdate>
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
struct REALITIES_API FTGOR_MovementContentUpdate : public FTGOR_MovementUpdate
{
	GENERATED_USTRUCT_BODY()
	SERIALISE_INIT_HEADER;

	FTGOR_MovementContentUpdate();

	virtual void Send(FTGOR_NetworkWritePackage& Package, UTGOR_Singleton* Context) const override;
	virtual void Recv(FTGOR_NetworkReadPackage& Package, UTGOR_Singleton* Context) override;

	////////////////////////////////////////////////////////////////////////////////////////////////////
	
	/** Current active movement */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		UTGOR_Movement* Movement;

};

template<>
struct TStructOpsTypeTraits<FTGOR_MovementContentUpdate> : public TStructOpsTypeTraitsBase2<FTGOR_MovementContentUpdate>
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
struct REALITIES_API FTGOR_MovementInputUpdate : public FTGOR_MovementContentUpdate
{
	GENERATED_USTRUCT_BODY()
	SERIALISE_INIT_HEADER;

	FTGOR_MovementInputUpdate();
	FTGOR_MovementInputUpdate(const FTGOR_MovementUpdate& Other);

	virtual void Send(FTGOR_NetworkWritePackage& Package, UTGOR_Singleton* Context) const override;
	virtual void Recv(FTGOR_NetworkReadPackage& Package, UTGOR_Singleton* Context) override;

	////////////////////////////////////////////////////////////////////////////////////////////////////

	/** Current input state */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FTGOR_MovementState State;

};

template<>
struct TStructOpsTypeTraits<FTGOR_MovementInputUpdate> : public TStructOpsTypeTraitsBase2<FTGOR_MovementInputUpdate>
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
struct REALITIES_API FTGOR_MovementVisuals
{
	GENERATED_USTRUCT_BODY()

		FTGOR_MovementVisuals();

	////////////////////////////////////////////////////////////////////////////////////////////////////

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float Blend;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float Radius;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		UTGOR_Content* Content;
};
