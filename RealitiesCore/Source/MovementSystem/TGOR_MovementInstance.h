// The Gateway of Realities: Planes of Existence.

#pragma once


#include "DimensionSystem/TGOR_PilotInstance.h"

#include "Engine/UserDefinedStruct.h"
#include "TGOR_MovementInstance.generated.h"

class UTGOR_Socket;
class UTGOR_Movement;
class UTGOR_Animation;
class UTGOR_AttachComponent;
class UTGOR_MovementComponent;
class UTGOR_MobilityComponent;
class UTGOR_RigidComponent;
class UTGOR_PilotComponent;
class ATGOR_PhysicsVolume;
class UTGOR_SocketStorage;
class UTGOR_PilotTask;
class UTGOR_Mobile;

///**
//*
//*/
//USTRUCT(BlueprintType)
//struct MOVEMENTSYSTEM_API FTGOR_MovementBase : public FTGOR_MovementDynamic
//{
//	GENERATED_USTRUCT_BODY()
//		SERIALISE_INIT_HEADER;
//
//	FTGOR_MovementBase();
//	FTGOR_MovementBase(UTGOR_MobilityComponent*  Component);
//
//	void Send(FTGOR_NetworkWritePackage& Package, UTGOR_Singleton* Context) const;
//	void Recv(FTGOR_NetworkReadPackage& Package, UTGOR_Singleton* Context);
//
//	FTGOR_MovementBase Lerp(const FTGOR_MovementBase& Other, float Alpha) const;
//	float Compare(const FTGOR_MovementBase& Base, const FTGOR_MovementThreshold& Threshold) const;
//	bool HasCycle() const;
//	
//	// Sets networked values
//	void Adjust(const FTGOR_MovementBase& Base);
//
//	////////////////////////////////////////////////////////////////////////////////////////////////////
//
//	/** Compute world space recursively */
//	FTGOR_MovementPosition ComputePosition() const;
//
//	/** Compute parent world space recursively */
//	FTGOR_MovementPosition ComputeParentPosition() const;
//
//	////////////////////////////////////////////////////////////////////////////////////////////////////
//
//	/** Compute world space recursively */
//	FTGOR_MovementSpace ComputeSpace() const;
//
//	/** Compute parent world space recursively */
//	FTGOR_MovementSpace ComputeParentSpace() const;
//
//
//	////////////////////////////////////////////////////////////////////////////////////////////////////
//
//	/** Parent component index (-1 for no component), used e.g. for bone index for skeletal mesh */
//	UPROPERTY(EditAnywhere, BlueprintReadWrite)
//		FTGOR_Index Index;
//
//	/** Parent socket type we're attached to (if attached at all) */
//	UPROPERTY(EditAnywhere, BlueprintReadWrite)
//		UTGOR_Socket* Socket;
//
//	/** Base of this base */
//	UPROPERTY(EditAnywhere, BlueprintReadWrite)
//		TWeakObjectPtr<UTGOR_MobilityComponent> Parent;
//
//	/** Own Component this base is attached to */
//	UPROPERTY(EditAnywhere, BlueprintReadWrite)
//		TWeakObjectPtr<UTGOR_MobilityComponent> Component;
//};
//
//template<> struct TStructOpsTypeTraits<FTGOR_MovementBase> : public TStructOpsTypeTraitsBase2<FTGOR_MovementBase>
//{
//	enum 
//	{ 
//		WithNetSerializer = true
//	};
//};



/**
* TGOR_MovementInstance stores movement customisation information
*/
USTRUCT(BlueprintType)
struct MOVEMENTSYSTEM_API FTGOR_MovementInstance
{
	GENERATED_USTRUCT_BODY()
		SERIALISE_INIT_HEADER;

	FTGOR_MovementInstance();

	CTGOR_GroupPackageCache LegacyCache;
	void Write(FTGOR_GroupWritePackage& Package, UTGOR_Singleton* Context) const;
	void Read(FTGOR_GroupReadPackage& Package, UTGOR_Singleton* Context);
	void Send(FTGOR_NetworkWritePackage& Package, UTGOR_Context* Context) const;
	void Recv(FTGOR_NetworkReadPackage& Package, UTGOR_Context* Context);

	////////////////////////////////////////////////////////////////////////////////////////////////////

	/** Currently linked loadout */
	UPROPERTY(BlueprintReadOnly, Category = "!TGOR Movement")
		UTGOR_Mobile* Mobile;

};

template<>
struct TStructOpsTypeTraits<FTGOR_MovementInstance> : public TStructOpsTypeTraitsBase2<FTGOR_MovementInstance>
{
	enum
	{
		WithNetSerializer = true
	};
};



/**
* FTGOR_MovementState
*/
USTRUCT(BlueprintType)
struct MOVEMENTSYSTEM_API FTGOR_MovementState
{
	GENERATED_USTRUCT_BODY()
		SERIALISE_INIT_HEADER;

	FTGOR_MovementState();
	void Send(FTGOR_NetworkWritePackage& Package, UTGOR_Singleton* Context) const;
	void Recv(FTGOR_NetworkReadPackage& Package, UTGOR_Singleton* Context);

	/** Currently active slot */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		int32 ActiveSlot;
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
struct MOVEMENTSYSTEM_API FTGOR_MovementGround
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
		float GroundRatio;
};

/**
*
*/
USTRUCT(BlueprintType)
struct MOVEMENTSYSTEM_API FTGOR_MovementContact : public FTGOR_MovementGround
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

	/** Input orthogonal to frame in euclidean space */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float FrameOrtho;

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
struct MOVEMENTSYSTEM_API FTGOR_MovementRepel
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
struct MOVEMENTSYSTEM_API FTGOR_MovementFrame
{
	GENERATED_USTRUCT_BODY()
	SERIALISE_INIT_HEADER;

	FTGOR_MovementFrame();

	void Send(FTGOR_NetworkWritePackage& Package, UTGOR_Singleton* Context) const;
	void Recv(FTGOR_NetworkReadPackage& Package, UTGOR_Singleton* Context);

	void SetLerpFrame(const FTGOR_MovementFrame& Other, float Alpha);

	////////////////////////////////////////////////////////////////////////////////////////////////////

	/** Scale factor for movement speed */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "!TGOR Movement")
		float Agility;

	/** Scale factor for movement strength */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "!TGOR Movement")
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
struct MOVEMENTSYSTEM_API FTGOR_MovementInput
{
	GENERATED_USTRUCT_BODY()
	SERIALISE_INIT_HEADER;

	FTGOR_MovementInput();

	void Send(FTGOR_NetworkWritePackage& Package, UTGOR_Singleton* Context) const;
	void Recv(FTGOR_NetworkReadPackage& Package, UTGOR_Singleton* Context);

	void SetLerpInput(const FTGOR_MovementInput& Other, float Alpha);
	float CompareInput(const FTGOR_MovementInput& State, double InputThres) const;
	
	////////////////////////////////////////////////////////////////////////////////////////////////////
	
	/** Input vector in euclidean space */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FVector Input;

	/** View vector in euclidean space */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FVector View;
};

template<>
struct TStructOpsTypeTraits<FTGOR_MovementInput> : public TStructOpsTypeTraitsBase2<FTGOR_MovementInput>
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
struct MOVEMENTSYSTEM_API FTGOR_MovementContent
{
	GENERATED_USTRUCT_BODY()
	SERIALISE_INIT_HEADER;

	FTGOR_MovementContent();

	void Send(FTGOR_NetworkWritePackage& Package, UTGOR_Singleton* Context) const;
	void Recv(FTGOR_NetworkReadPackage& Package, UTGOR_Singleton* Context);

	void SetLerpContent(const FTGOR_MovementContent& Other, float Alpha);
	float CompareContent(const FTGOR_MovementContent& State) const;

	////////////////////////////////////////////////////////////////////////////////////////////////////

	/** Current active movement */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		UTGOR_Movement* Movement;

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
struct MOVEMENTSYSTEM_API FTGOR_MovementObstacle
{
	GENERATED_USTRUCT_BODY()
		FTGOR_MovementObstacle();

	/** Collider extend */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FTGOR_MovementDynamic Dynamic;

	/** Collider extend */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FVector Extent;
};