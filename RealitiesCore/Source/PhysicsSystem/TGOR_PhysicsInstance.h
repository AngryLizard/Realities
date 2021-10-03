// The Gateway of Realities: Planes of Existence.

#pragma once


#include "DimensionSystem/TGOR_PilotInstance.h"

#include "Engine/UserDefinedStruct.h"
#include "TGOR_PhysicsInstance.generated.h"

class UTGOR_MobilityComponent;

/**
*
*/
USTRUCT(BlueprintType)
struct PHYSICSSYSTEM_API FTGOR_MovementCapture
{
	GENERATED_USTRUCT_BODY()
	SERIALISE_INIT_HEADER;

	FTGOR_MovementCapture();

	void Send(FTGOR_NetworkWritePackage& Package, UTGOR_Singleton* Context) const;
	void Recv(FTGOR_NetworkReadPackage& Package, UTGOR_Singleton* Context);

	void SetLerpCapture(const FTGOR_MovementCapture& Other, float Alpha);
	float CompareCapture(const FTGOR_MovementCapture& State, float AngleThres) const;

	////////////////////////////////////////////////////////////////////////////////////////////////////

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
* Current movement situation relative to current tick
*/
USTRUCT(BlueprintType)
struct PHYSICSSYSTEM_API FTGOR_MovementTick
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
struct PHYSICSSYSTEM_API FTGOR_MovementOutput : public FTGOR_MovementForce
{
	GENERATED_USTRUCT_BODY()

		FTGOR_MovementOutput();

	////////////////////////////////////////////////////////////////////////////////////////////////////

	/** Orientation force in kg*m^2/s^2 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FVector Orientation;

	/** Max damping that occured this frame */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float MaxLinearDamping;

	/** Max damping that occured this frame */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float MaxAngularDamping;
};
