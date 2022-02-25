// The Gateway of Realities: Planes of Existence.

#include "Realities.h"
#include "TGOR_ReplicatedMovementComponent.h"

#include "GameFramework/GameNetworkManager.h"
#include "Actors/Pawns/TGOR_MovementCharacter.h"

#include "Components/Combat/TGOR_AimComponent.h"
#include "Components/Combat/Stat/TGOR_StaminaComponent.h"
#include "Components/Combat/Stat/TGOR_HealthComponent.h"
#include "Components/Combat/Stat/TGOR_energyComponent.h"


FNetworkPredictionData_Client* UTGOR_ReplicatedMovementComponent::GetPredictionData_Client() const
{
	// Should only be called on client or listen server (for remote clients) in network games
	check(CharacterOwner != NULL);
	checkSlow(CharacterOwner->Role < ROLE_Authority || (CharacterOwner->GetRemoteRole() == ROLE_AutonomousProxy && GetNetMode() == NM_ListenServer));
	checkSlow(GetNetMode() == NM_Client || GetNetMode() == NM_ListenServer);

	if (!ClientPredictionData)
	{
		UTGOR_ReplicatedMovementComponent* MutableThis = const_cast<UTGOR_ReplicatedMovementComponent*>(this);
		MutableThis->ClientPredictionData = new FNetworkPredictionData_Client_CreatureCharacter(*this);
	}

	return ClientPredictionData;
}

FNetworkPredictionData_Server* UTGOR_ReplicatedMovementComponent::GetPredictionData_Server() const
{
	// Should only be called on server in network games
	check(CharacterOwner != NULL);
	check(CharacterOwner->Role == ROLE_Authority);
	checkSlow(GetNetMode() < NM_Client);

	if (!ServerPredictionData)
	{
		UTGOR_ReplicatedMovementComponent* MutableThis = const_cast<UTGOR_ReplicatedMovementComponent*>(this);
		MutableThis->ServerPredictionData = new FNetworkPredictionData_Server_CreatureCharacter(*this);
	}

	return ServerPredictionData;
}


void UTGOR_ReplicatedMovementComponent::SendClientAdjustment()
{
	if (!HasValidData())
	{
		return;
	}

	FNetworkPredictionData_Server_CreatureCharacter* ServerData = static_cast<FNetworkPredictionData_Server_CreatureCharacter*>(GetPredictionData_Server_Character());
	check(ServerData);

	if (ServerData->PendingAdjustment.TimeStamp <= 0.f)
	{
		return;
	}

	if (ServerData->PendingAdjustment.bAckGoodMove == true)
	{
		// just notify client this move was received
		ClientAckGoodMove(ServerData->PendingAdjustment.TimeStamp);
	}
	else
	{
		const bool bIsPlayingNetworkedRootMotionMontage = CharacterOwner->IsPlayingNetworkedRootMotionMontage();
		if (HasRootMotionSources())
		{
			FRotator Rotation = ServerData->PendingAdjustment.NewRot.GetNormalized();
			FVector_NetQuantizeNormal CompressedRotation(Rotation.Pitch / 180.f, Rotation.Yaw / 180.f, Rotation.Roll / 180.f);
			ClientAdjustRootMotionSourcePosition
			(
				ServerData->PendingAdjustment.TimeStamp,
				CurrentRootMotion,
				bIsPlayingNetworkedRootMotionMontage,
				bIsPlayingNetworkedRootMotionMontage ? CharacterOwner->GetRootMotionAnimMontageInstance()->GetPosition() : -1.f,
				ServerData->PendingAdjustment.NewLoc,
				CompressedRotation,
				ServerData->PendingAdjustment.NewVel.Z,
				ServerData->PendingAdjustment.NewBase,
				ServerData->PendingAdjustment.NewBaseBoneName,
				ServerData->PendingAdjustment.NewBase != NULL,
				ServerData->PendingAdjustment.bBaseRelativePosition,
				PackNetworkMovementMode()
			);
		}
		else if (bIsPlayingNetworkedRootMotionMontage)
		{
			FRotator Rotation = ServerData->PendingAdjustment.NewRot.GetNormalized();
			FVector_NetQuantizeNormal CompressedRotation(Rotation.Pitch / 180.f, Rotation.Yaw / 180.f, Rotation.Roll / 180.f);
			ClientAdjustRootMotionPosition
			(
				ServerData->PendingAdjustment.TimeStamp,
				CharacterOwner->GetRootMotionAnimMontageInstance()->GetPosition(),
				ServerData->PendingAdjustment.NewLoc,
				CompressedRotation,
				ServerData->PendingAdjustment.NewVel.Z,
				ServerData->PendingAdjustment.NewBase,
				ServerData->PendingAdjustment.NewBaseBoneName,
				ServerData->PendingAdjustment.NewBase != NULL,
				ServerData->PendingAdjustment.bBaseRelativePosition,
				PackNetworkMovementMode()
			);
		}
		else if (ServerData->PendingAdjustment.NewVel.IsZero())
		{
			ClientVeryShortAdjustPosition
			(
				ServerData->PendingAdjustment.TimeStamp,
				ServerData->PendingAdjustment.NewLoc,
				ServerData->PendingAdjustment.NewBase,
				ServerData->PendingAdjustment.NewBaseBoneName,
				ServerData->PendingAdjustment.NewBase != NULL,
				ServerData->PendingAdjustment.bBaseRelativePosition,
				PackNetworkMovementMode()
			);
		}
		else
		{
			CreatureClientAdjustPosition
			(
				ServerData->PendingAdjustment.TimeStamp,
				ServerData->PendingAdjustment.NewLoc,
				ServerData->PendingAdjustment.NewVel,
				ServerData->Additive,
				ServerData->Yaw,
				ServerData->PendingAdjustment.NewBase,
				ServerData->PendingAdjustment.NewBaseBoneName,
				ServerData->PendingAdjustment.NewBase != NULL,
				ServerData->PendingAdjustment.bBaseRelativePosition,
				PackNetworkMovementMode()
			);
		}
	}

	ServerData->PendingAdjustment.TimeStamp = 0;
	ServerData->PendingAdjustment.bAckGoodMove = false;
	ServerData->bForceClientUpdate = false;
}


void UTGOR_ReplicatedMovementComponent::ServerMoveHandleClientError(float ClientTimeStamp, float DeltaTime, const FVector& Accel, const FVector& RelativeClientLoc, UPrimitiveComponent* ClientMovementBase, FName ClientBaseBoneName, uint8 ClientMovementMode)
{
	if (RelativeClientLoc == FVector(1.f, 2.f, 3.f)) // first part of double servermove
	{
		return;
	}

	FNetworkPredictionData_Server_CreatureCharacter* ServerData = static_cast<FNetworkPredictionData_Server_CreatureCharacter*>(GetPredictionData_Server_Character());
	check(ServerData);

	// Don't prevent more recent updates from being sent if received this frame.
	// We're going to send out an update anyway, might as well be the most recent one.
	APlayerController* PC = Cast<APlayerController>(CharacterOwner->GetController());
	if ((ServerData->LastUpdateTime != GetWorld()->TimeSeconds) && GetDefault<AGameNetworkManager>()->WithinUpdateDelayBounds(PC, ServerData->LastUpdateTime))
	{
		return;
	}

	// Offset may be relative to base component
	FVector ClientLoc = RelativeClientLoc;
	if (MovementBaseUtility::UseRelativeLocation(ClientMovementBase))
	{
		FVector BaseLocation;
		FQuat BaseRotation;
		MovementBaseUtility::GetMovementBaseTransform(ClientMovementBase, ClientBaseBoneName, BaseLocation, BaseRotation);
		ClientLoc += BaseLocation;
	}

	// Compute the client error from the server's position
	// If client has accumulated a noticeable positional error, correct him.
	if (ServerData->bForceClientUpdate || ServerCheckClientError(ClientTimeStamp, DeltaTime, Accel, ClientLoc, RelativeClientLoc, ClientMovementBase, ClientBaseBoneName, ClientMovementMode))
	{
		const FRotator Rotator = UpdatedComponent->GetComponentRotation();
		UPrimitiveComponent* MovementBase = CharacterOwner->GetMovementBase();
		ServerData->PendingAdjustment.NewVel = Velocity;
		ServerData->PendingAdjustment.NewBase = MovementBase;
		ServerData->PendingAdjustment.NewBaseBoneName = CharacterOwner->GetBasedMovement().BoneName;
		ServerData->PendingAdjustment.NewLoc = FRepMovement::RebaseOntoZeroOrigin(UpdatedComponent->GetComponentLocation(), this);
		ServerData->PendingAdjustment.NewRot = Rotator;

		ServerData->PendingAdjustment.bBaseRelativePosition = MovementBaseUtility::UseRelativeLocation(MovementBase);
		if (ServerData->PendingAdjustment.bBaseRelativePosition)
		{
			// Relative location
			ServerData->PendingAdjustment.NewLoc = CharacterOwner->GetBasedMovement().Location;

			// TODO: this could be a relative rotation, but all client corrections ignore rotation right now except the root motion one, which would need to be updated.
			//ServerData->PendingAdjustment.NewRot = CharacterOwner->GetBasedMovement().Rotation;
		}

		ServerData->LastUpdateTime = GetWorld()->TimeSeconds;
		ServerData->PendingAdjustment.DeltaTime = DeltaTime;
		ServerData->PendingAdjustment.TimeStamp = ClientTimeStamp;
		ServerData->PendingAdjustment.bAckGoodMove = false;
		ServerData->PendingAdjustment.MovementMode = PackNetworkMovementMode();

		ServerData->Additive = TurnAdditive.Yaw;
		ServerData->Yaw = Rotator.Yaw;
	}
	else
	{
		if (GetDefault<AGameNetworkManager>()->ClientAuthorativePosition)
		{
			const FVector LocDiff = UpdatedComponent->GetComponentLocation() - ClientLoc;
			if (!LocDiff.IsZero() || ClientMovementMode != PackNetworkMovementMode() || GetMovementBase() != ClientMovementBase || (CharacterOwner && CharacterOwner->GetBasedMovement().BoneName != ClientBaseBoneName))
			{
				// Just set the position. On subsequent moves we will resolve initially overlapping conditions.
				UpdatedComponent->SetWorldLocation(ClientLoc, false);

				// Trust the client's movement mode.
				ApplyNetworkMovementMode(ClientMovementMode);

				// Update base and floor at new location.
				SetBase(ClientMovementBase, ClientBaseBoneName);
				UpdateFloorFromAdjustment();

				// Even if base has not changed, we need to recompute the relative offsets (since we've moved).
				SaveBaseLocation();

				LastUpdateLocation = UpdatedComponent ? UpdatedComponent->GetComponentLocation() : FVector::ZeroVector;
				LastUpdateRotation = UpdatedComponent ? UpdatedComponent->GetComponentQuat() : FQuat::Identity;
				LastUpdateVelocity = Velocity;
			}
		}

		// acknowledge receipt of this successful servermove()
		ServerData->PendingAdjustment.TimeStamp = ClientTimeStamp;
		ServerData->PendingAdjustment.bAckGoodMove = true;
	}

	ServerData->bForceClientUpdate = false;
}

void UTGOR_ReplicatedMovementComponent::CreatureClientAdjustPosition_Implementation(
	float TimeStamp, 
	FVector NewLoc, 
	FVector NewVel, 
	float Additive, 
	float Yaw, 
	UPrimitiveComponent* NewBase, 
	FName NewBaseBoneName, 
	bool bHasBase, 
	bool bBaseRelativePosition, 
	uint8 ServerMovementMode
)
{
	if (!HasValidData() || !IsActive())
	{
		return;
	}

	// Set additive
	TurnAdditive.Yaw = Additive;

	// Set rotation
	FRotator Rotator = UpdatedComponent->GetComponentRotation();
	Rotator.Yaw = Yaw;
	UpdatedComponent->SetWorldRotation(Rotator, false);

	// Do default
	ClientAdjustPosition_Implementation
	(
		TimeStamp,
		NewLoc,
		NewVel,
		NewBase,
		NewBaseBoneName,
		bHasBase,
		bBaseRelativePosition,
		ServerMovementMode
	);
}











void UTGOR_ReplicatedMovementComponent::UpdateFromCompressedFlags(uint8 Flags)
{
	Super::UpdateFromCompressedFlags(Flags);

	//yourBool = ((Flags & FSavedMove_Character::FLAG_Custom_0) != 0);
}


bool UTGOR_ReplicatedMovementComponent::ClientUpdatePositionAfterServerUpdate()
{
	if (!HasValidData())
	{
		return false;
	}

	FNetworkPredictionData_Client_Character* ClientData = GetPredictionData_Client_Character();
	check(ClientData);

	if (!ClientData->bUpdatePosition)
	{
		return false;
	}

	if (bIgnoreClientMovementErrorChecksAndCorrection)
	{
		return false;
	}

	ClientData->bUpdatePosition = false;

	// Don't do any network position updates on things running PHYS_RigidBody
	if (CharacterOwner->GetRootComponent() && CharacterOwner->GetRootComponent()->IsSimulatingPhysics())
	{
		return false;
	}

	if (ClientData->SavedMoves.Num() == 0)
	{
		UE_LOG(LogNetPlayerMovement, VeryVerbose, TEXT("ClientUpdatePositionAfterServerUpdate No saved moves to replay"), ClientData->SavedMoves.Num());

		// With no saved moves to resimulate, the move the server updated us with is the last move we've done, no resimulation needed.
		CharacterOwner->bClientResimulateRootMotion = false;
		if (CharacterOwner->bClientResimulateRootMotionSources)
		{
			// With no resimulation, we just update our current root motion to what the server sent us
			UE_LOG(LogRootMotion, VeryVerbose, TEXT("CurrentRootMotion getting updated to ServerUpdate state: %s"), *CharacterOwner->GetName());
			CurrentRootMotion.UpdateStateFrom(CharacterOwner->SavedRootMotion);
			CharacterOwner->bClientResimulateRootMotionSources = false;
		}

		return false;
	}

	// Save important values that might get affected by the replay.
	const float SavedAnalogInputModifier = AnalogInputModifier;
	const FRootMotionMovementParams BackupRootMotionParams = RootMotionParams; // For animation root motion
	const FRootMotionSourceGroup BackupRootMotion = CurrentRootMotion;
	const bool bRealJump = CharacterOwner->bPressedJump;
	const bool bRealCrouch = bWantsToCrouch;
	const bool bRealForceMaxAccel = bForceMaxAccel;
	CharacterOwner->bClientWasFalling = (MovementMode == MOVE_Falling);
	CharacterOwner->bClientUpdating = true;
	bForceNextFloorCheck = true;
	
	// Set initial parameters
	FSavedMove_CreatureCharacter* FirstMove = static_cast<FSavedMove_CreatureCharacter*>(ClientData->SavedMoves[0].Get());
	FirstMove->InitMoveFor(CharacterOwner);

	// Replay moves that have not yet been acked.
	UE_LOG(LogNetPlayerMovement, VeryVerbose, TEXT("ClientUpdatePositionAfterServerUpdate Replaying %d Moves, starting at Timestamp %f"), ClientData->SavedMoves.Num(), ClientData->SavedMoves[0]->TimeStamp);
	for (int32 i = 0; i<ClientData->SavedMoves.Num(); i++)
	{
		const FSavedMovePtr& CurrentMove = ClientData->SavedMoves[i];
		CurrentMove->PrepMoveFor(CharacterOwner);
		MoveAutonomous(CurrentMove->TimeStamp, CurrentMove->DeltaTime, CurrentMove->GetCompressedFlags(), CurrentMove->Acceleration);
		CurrentMove->PostUpdate(CharacterOwner, FSavedMove_Character::PostUpdate_Replay);
	}

	if (ClientData->PendingMove.IsValid())
	{
		ClientData->PendingMove->bForceNoCombine = true;
	}

	// Restore saved values.
	AnalogInputModifier = SavedAnalogInputModifier;
	RootMotionParams = BackupRootMotionParams;
	CurrentRootMotion = BackupRootMotion;
	if (CharacterOwner->bClientResimulateRootMotionSources)
	{
		// If we were resimulating root motion sources, it's because we had mismatched state
		// with the server - we just resimulated our SavedMoves and now need to restore
		// CurrentRootMotion with the latest "good state"
		UE_LOG(LogRootMotion, VeryVerbose, TEXT("CurrentRootMotion getting updated after ServerUpdate replays: %s"), *CharacterOwner->GetName());
		CurrentRootMotion.UpdateStateFrom(CharacterOwner->SavedRootMotion);
		CharacterOwner->bClientResimulateRootMotionSources = false;
	}
	CharacterOwner->SavedRootMotion.Clear();
	CharacterOwner->bClientResimulateRootMotion = false;
	CharacterOwner->bClientUpdating = false;
	CharacterOwner->bPressedJump = bRealJump;
	bWantsToCrouch = bRealCrouch;
	bForceMaxAccel = bRealForceMaxAccel;
	bForceNextFloorCheck = true;

	return (ClientData->SavedMoves.Num() > 0);
}




void FSavedMove_CreatureCharacter::Clear()
{
	FSavedMove_Character::Clear();

	//yourBool = false;

	Submerged = false;
	SpeedFactor = 0.0f;
	WallSurfacePitch = 0.0f;
	SubMovement = ETGOR_SubMovementMode::Outside;
	TurnAdditive = FRotator(0.0f, 0.0f, 0.0f);
	SurfaceNormal = FVector(0.0f, 0.0f, 0.0f);
	AimFocus = FVector(0.0f, 0.0f, 0.0f);
	Clamp = 0.0f;

	WalkableAngle = 0.0f;
	TurnSpeed = 0.0f;

	HealthState = (uint8)ETGOR_HealthState::Normal;

	Stamina = 0.0f;
	ExhaustStaminaThreshold = 0.0f;
	ExhaustStaminaRate = 0.0f;
	IsAggressive = false;
}

void FSavedMove_CreatureCharacter::SetMoveFor(ACharacter* C, float InDeltaTime, FVector const& NewAccel, class FNetworkPredictionData_Client_Character & ClientData)
{
	FSavedMove_Character::SetMoveFor(C, InDeltaTime, NewAccel, ClientData);

	ATGOR_MovementCharacter* CreatureCharacter = Cast<ATGOR_MovementCharacter>(C);
	UTGOR_ReplicatedMovementComponent* Movement = Cast<UTGOR_ReplicatedMovementComponent>(C->GetCharacterMovement());

	//yourBool = CreatureCharacter->yourBool;

	Submerged = Movement->Submerged;
	FloorAngle = Movement->FloorAngle;
	SpeedFactor = Movement->SpeedFactor;
	WallSurfacePitch = Movement->WallSurfacePitch;
	SubMovement = Movement->SubMovementMode;
	TurnAdditive = Movement->TurnAdditive;
	SurfaceNormal = Movement->SurfaceNormal;
	AimFocus = CreatureCharacter->AimComponent->AimFocus;
	Clamp = Movement->VelocityClamp;

	WalkableAngle = Movement->GetWalkableFloorAngle();
	TurnSpeed = Movement->TurnSpeed;

	HealthState = (uint8)CreatureCharacter->HealthComponent->HealthState;

	Stamina = CreatureCharacter->StaminaComponent->Stat;
	ExhaustStaminaThreshold = Movement->ExhaustStaminaThreshold;
	ExhaustStaminaRate = Movement->ExhaustStaminaRate;
	IsAggressive = CreatureCharacter->IsAggressive;
}

bool FSavedMove_CreatureCharacter::IsImportantMove(const FSavedMovePtr& LastAckedMove) const
{
	if (FSavedMove_Character::IsImportantMove(LastAckedMove))
	{
		return(true);
	}

	FSavedMove_CreatureCharacter* Last = static_cast<FSavedMove_CreatureCharacter*>(LastAckedMove.Get());
	if (Last == nullptr)
	{
		return(false);
	}

	//return(yourBool != Last->yourBool);
	return(IsAggressive != Last->IsAggressive);
}

bool FSavedMove_CreatureCharacter::CanCombineWith(const FSavedMovePtr& NewMove, ACharacter* InPawn, float MaxDelta) const
{
	FSavedMove_CreatureCharacter* Last = static_cast<FSavedMove_CreatureCharacter*>(NewMove.Get());
	if (Last == nullptr)
	{
		return(false);
	}

/*	if (youBool != Last->yourBool)
	{
		return false;
	}*/


	if (!AimFocus.Equals(Last->AimFocus, KINDA_SMALL_NUMBER))
	{
		return false;
	}

	if (FMath::Abs(SpeedFactor - Last->SpeedFactor) > KINDA_SMALL_NUMBER)
	{
		return false;
	}

	if (FMath::Abs(Stamina - Last->Stamina) > KINDA_SMALL_NUMBER)
	{
		return false;
	}

	if (FMath::Abs(ExhaustStaminaThreshold - Last->ExhaustStaminaThreshold) > KINDA_SMALL_NUMBER)
	{
		return false;
	}

	if (FMath::Abs(ExhaustStaminaRate - Last->ExhaustStaminaRate) > KINDA_SMALL_NUMBER)
	{
		return false;
	}
			
	if (IsAggressive != Last->IsAggressive)
	{
		return false;
	}

	return(FSavedMove_Character::CanCombineWith(NewMove, InPawn, MaxDelta));
}

void FSavedMove_CreatureCharacter::InitMoveFor(ACharacter* C)
{
	ATGOR_MovementCharacter* CreatureCharacter = Cast<ATGOR_MovementCharacter>(C);
	UTGOR_ReplicatedMovementComponent* Movement = Cast<UTGOR_ReplicatedMovementComponent>(C->GetCharacterMovement());

	Movement->Submerged = Submerged;
	Movement->FloorAngle = FloorAngle;
	Movement->WallSurfacePitch = WallSurfacePitch;
	Movement->SubMovementMode = SubMovement;
	Movement->TurnAdditive = TurnAdditive;
	Movement->SurfaceNormal = SurfaceNormal;
	Movement->VelocityClamp = Clamp;

	Movement->SetWalkableFloorAngle(WalkableAngle);
	Movement->TurnSpeed = TurnSpeed;
}

void FSavedMove_CreatureCharacter::PrepMoveFor(ACharacter* C)
{
	FSavedMove_Character::PrepMoveFor(C);

	ATGOR_MovementCharacter* CreatureCharacter = Cast<ATGOR_MovementCharacter>(C);
	UTGOR_ReplicatedMovementComponent* Movement = Cast<UTGOR_ReplicatedMovementComponent>(C->GetCharacterMovement());

	CreatureCharacter->AimComponent->AimFocus = AimFocus;
	Movement->SpeedFactor = SpeedFactor;
	CreatureCharacter->StaminaComponent->Stat = Stamina;
	CreatureCharacter->HealthComponent->HealthState = (ETGOR_HealthState)HealthState;
	Movement->ExhaustStaminaThreshold = ExhaustStaminaThreshold;
	Movement->ExhaustStaminaRate = ExhaustStaminaRate;
	CreatureCharacter->IsAggressive = IsAggressive;
}


uint8 FSavedMove_CreatureCharacter::GetCompressedFlags() const
{
	uint8 Result = FSavedMove_Character::GetCompressedFlags();

/*	if (your_bool)
	{
		Result |= FLAG_Custom_0;
	}*/

	return Result;
}


FNetworkPredictionData_Client_CreatureCharacter::FNetworkPredictionData_Client_CreatureCharacter(const UCharacterMovementComponent& ClientMovement)
	: FNetworkPredictionData_Client_Character(ClientMovement)
{
}

FSavedMovePtr FNetworkPredictionData_Client_CreatureCharacter::AllocateNewMove()
{
	return FSavedMovePtr(new FSavedMove_CreatureCharacter());
}


FNetworkPredictionData_Server_CreatureCharacter::FNetworkPredictionData_Server_CreatureCharacter(const UCharacterMovementComponent& ServerMovement)
	: FNetworkPredictionData_Server_Character(ServerMovement)
{
	Additive = 0.0f;
	Yaw = 0.0f;
}