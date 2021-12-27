// TGOR (C) // CHECKED //
#pragma once

#include "CoreMinimal.h"

#include "MovementSystem/Tasks/TGOR_EuclideanMovementTask.h"
#include "TGOR_IPC.generated.h"

/**
*
*/
USTRUCT(BlueprintType)
struct MOVEMENTSYSTEM_API FTGOR_PendulumState
{
	GENERATED_USTRUCT_BODY();

	/** Pendulum X state relative to parent */
	UPROPERTY()
		FVector4 X;

	/** Pendulum Y state relative to parent */
	UPROPERTY()
		FVector4 Y;

	/** Pendulum Z state relative to parent */
	UPROPERTY()
		FVector2D Z;

	/** Pendulum rotation state relative to parent */
	UPROPERTY()
		FVector2D R;
};

/**
*
*/
USTRUCT(BlueprintType)
struct MOVEMENTSYSTEM_API FTGOR_PendulumProperties
{
	GENERATED_USTRUCT_BODY();

	FVector4 PositionToPendulum(const FTGOR_MovementPosition& Position, const FVector& UpDirection, const FVector& ForwardDirection) const;
	FVector4 DynamicToPendulum(const FTGOR_MovementDynamic& Dynamic, const FVector& UpDirection, const FVector& ForwardDirection) const;

	FTGOR_PendulumState PositionToPendulum(const FTGOR_MovementPosition& Position) const;
	FTGOR_PendulumState DynamicToPendulum(const FTGOR_MovementDynamic& Dynamic) const;
	
	FTGOR_MovementPosition PendulumToPosition(const FTGOR_PendulumState& State) const;
	FTGOR_MovementDynamic PendulumToDynamic(const FTGOR_PendulumState& State) const;

	/** Pendulum length */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float Length = 1.0f;

	/** Pendulum mass */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float Mass = 1.0f;

	/** Pendulum inertia */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float Inertia = 0.01f;

	/** Pendulum gravity */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float Gravity = 1.0f;
};

/**
*
*/
USTRUCT(BlueprintType)
struct MOVEMENTSYSTEM_API FTGOR_RiccatiProperties
{
	GENERATED_USTRUCT_BODY();

	/** Ricatti Q weight */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float Q = 1.0f;

	/** Ricatti R weight */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float R = 1.0f;

	/** Ricatti step length */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float Dt = 0.001f;

	/** Ricatti max number of iterations */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		int32 Iterations = 32;
};

/**
*
*/
USTRUCT(BlueprintType)
struct MOVEMENTSYSTEM_API FTGOR_IPCProperties
{
	GENERATED_USTRUCT_BODY();

	void Generate(const FTGOR_PendulumProperties& PendulumProperties, const FTGOR_RiccatiProperties& RiccatiProperties);
	void SimulateForPosition(FVector4& State, float Position, float DeltaTime) const;
	void SimulateForVelocity(FVector4& State, float Velocity, float DeltaTime) const;

	/** Atate coefficients */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FMatrix StateCoef;

	/** Force coefficients */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FVector4 ForceCoef;

	/** Force response */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FVector4 ForceResponse;
};

/**
*
*/
UCLASS(Blueprintable)
class MOVEMENTSYSTEM_API ATGOR_IPCActor : public AActor
{
	GENERATED_BODY()

public:

	////////////////////////////////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////// ENGINE ////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////////////////////////////

	ATGOR_IPCActor();

};
