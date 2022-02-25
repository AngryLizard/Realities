// The Gateway of Realities: Planes of Existence.

#pragma once

#include "CoreMinimal.h"
#include "Components/Movement/TGOR_SkiingMovementComponent.h"
#include "TGOR_ReplicatedMovementComponent.generated.h"

/**
 * 
 */
UCLASS()
class REALITIES_API UTGOR_ReplicatedMovementComponent : public UTGOR_SkiingMovementComponent
{
	GENERATED_BODY()
	
public:

	virtual class FNetworkPredictionData_Client* GetPredictionData_Client() const override;
	virtual class FNetworkPredictionData_Server* GetPredictionData_Server() const override;

	virtual void SendClientAdjustment() override;

protected:

	virtual void ServerMoveHandleClientError(float ClientTimeStamp, float DeltaTime, const FVector& Accel, const FVector& RelativeClientLocation, UPrimitiveComponent* ClientMovementBase, FName ClientBaseBoneName, uint8 ClientMovementMode) override;

	UFUNCTION(unreliable, client)
		virtual void CreatureClientAdjustPosition(float TimeStamp, FVector NewLoc, FVector NewVel, float Additive, float Yaw, UPrimitiveComponent* NewBase, FName NewBaseBoneName, bool bHasBase, bool bBaseRelativePosition, uint8 ServerMovementMode);
		virtual void CreatureClientAdjustPosition_Implementation(float TimeStamp, FVector NewLoc, FVector NewVel, float Additive, float Yaw, UPrimitiveComponent* NewBase, FName NewBaseBoneName, bool bHasBase, bool bBaseRelativePosition, uint8 ServerMovementMode);
	
	// Movement override
	void UpdateFromCompressedFlags(uint8 Flags) override;
	bool ClientUpdatePositionAfterServerUpdate() override;
	
};



class REALITIES_API FSavedMove_CreatureCharacter : public FSavedMove_Character
{
public:
	bool Submerged;
	float FloorAngle;
	float SpeedFactor;
	float WallSurfacePitch;
	ETGOR_SubMovementMode SubMovement;
	FRotator TurnAdditive;
	FVector SurfaceNormal;
	FVector AimFocus;
	float Clamp;

	float WalkableAngle;
	float TurnSpeed;

	uint8 HealthState;

	float Stamina;
	float ExhaustStaminaThreshold;
	float ExhaustStaminaRate;
	bool IsAggressive;

	virtual void Clear() override;
	virtual void SetMoveFor(ACharacter* C, float InDeltaTime, FVector const& NewAccel, class FNetworkPredictionData_Client_Character & ClientData) override;
	virtual bool IsImportantMove(const FSavedMovePtr& LastAckedMove) const override;
	virtual bool CanCombineWith(const FSavedMovePtr& NewMove, ACharacter* InPawn, float MaxDelta) const override;

	virtual void InitMoveFor(ACharacter* C);
	virtual void PrepMoveFor(ACharacter* C) override;
	virtual uint8 GetCompressedFlags() const override;
};



class REALITIES_API FNetworkPredictionData_Client_CreatureCharacter : public FNetworkPredictionData_Client_Character
{
public:
	FNetworkPredictionData_Client_CreatureCharacter(const UCharacterMovementComponent& ClientMovement);
	virtual FSavedMovePtr AllocateNewMove() override;
};


class REALITIES_API FNetworkPredictionData_Server_CreatureCharacter : public FNetworkPredictionData_Server_Character
{
public:
	FNetworkPredictionData_Server_CreatureCharacter(const UCharacterMovementComponent& ServerMovement);

	float Additive;
	float Yaw;
};