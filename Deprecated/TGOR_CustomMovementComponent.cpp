// The Gateway of Realities: Planes of Existence.

#include "Realities.h"
#include "TGOR_CustomMovementComponent.h"

#include "TGOR_CollisionSphereComponent.h"
#include "Components/Movement/TGOR_CollisionSpheresComponent.h"

const float VERTICAL_SLOPE_NORMAL_Z = 0.001f;


UTGOR_CustomMovementComponent::UTGOR_CustomMovementComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	TurnSpeed = 360.0f;
	TurnAdditive = FRotator(0.0f, 0.0f, 0.0f);
	TurnDelta = FRotator(0.0f, 0.0f, 0.0f);

	LastSpeedLimit = 0.0f;
	MovementBrakeFactor = 1.0f;

	VelocityClamp = 0.0f;
	CollisionSphereCorrectionPitch = 0.0f;
	CollisionSphereCorrectionSpeed = 50.0f;
	CollisionSphereCorrectionEnabled = true;

	_movementDirection = FVector(0.0f, 0.0f, 0.0f);

	CollisionSpheres = nullptr;
}


void UTGOR_CustomMovementComponent::BeginPlay()
{
	Super::BeginPlay();

	// Find collision spheres
	AActor* Owner = GetOwner();
	if (IsValid(Owner))
	{
		UActorComponent* Component = Owner->GetComponentByClass(UTGOR_CollisionSpheresComponent::StaticClass());
		CollisionSpheres = Cast<UTGOR_CollisionSpheresComponent>(Component);
		if (IsValid(CollisionSpheres))
		{
			CollisionSpheres->Build();
		}
	}
}

void UTGOR_CustomMovementComponent::TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction)
{
	// Clamp Deltatime to not spazz out character for low framerates
	Super::TickComponent(FMath::Min(0.05f, DeltaTime), TickType, ThisTickFunction);
}

void UTGOR_CustomMovementComponent::CalcCustomVelocity(float DeltaTime, float Friction, bool bFluid, float BrakingDeceleration, InputAccelerationFunc func)
{
	// Do not update velocity when using root motion or when SimulatedProxy - SimulatedProxy are repped their Velocity
	if (!HasValidData() || HasAnimRootMotion() || DeltaTime < MIN_TICK_TIME || (CharacterOwner && CharacterOwner->Role == ROLE_SimulatedProxy))
	{ return; }

	Friction = FMath::Max(0.f, Friction * MovementBrakeFactor);
	BrakingDeceleration = BrakingDeceleration * MovementBrakeFactor;
	const float MaxAccel = GetMaxAcceleration();
	float MaxSpeed = GetMaxSpeed();

	// Check if path following requested movement
	bool bZeroRequestedAcceleration = true;
	FVector RequestedAcceleration = FVector::ZeroVector;
	float RequestedSpeed = 0.0f;
	if (ApplyRequestedMove(DeltaTime, MaxAccel, MaxSpeed, Friction, BrakingDeceleration, RequestedAcceleration, RequestedSpeed))
	{
		RequestedAcceleration = RequestedAcceleration.GetClampedToMaxSize(MaxAccel);
		bZeroRequestedAcceleration = false;
	}

	if (bForceMaxAccel)
	{
		// Force acceleration at full speed.
		// In consideration order for direction: Acceleration, then Velocity, then Pawn's rotation.
		if (Acceleration.SizeSquared() > SMALL_NUMBER)
		{
			Acceleration = Acceleration.GetSafeNormal() * MaxAccel;
		}
		else
		{
			Acceleration = MaxAccel * (Velocity.SizeSquared() < SMALL_NUMBER ? UpdatedComponent->GetForwardVector() : Velocity.GetSafeNormal());
		}

		AnalogInputModifier = 1.f;
	}
	


	// Apply braking or deceleration
	const bool bZeroAcceleration = Acceleration.IsZero();
	const bool bVelocityOverClamp = IsExceedingMaxSpeed(MaxSpeed);
	const bool bVelocityOverMax = IsExceedingMaxSpeed(VelocityClamp);
	
	// Only apply braking if there is no acceleration or we are over our max speed and need to slow down to it.
	if (((bZeroAcceleration && bZeroRequestedAcceleration) || bVelocityOverMax))
	{
		const FVector OldVelocity = Velocity;
		const float ActualBrakingFriction = ((bUseSeparateBrakingFriction && !bVelocityOverClamp) ? BrakingFriction : Friction);
		ApplyVelocityBraking(DeltaTime, ActualBrakingFriction, BrakingDeceleration);

		// Don't allow braking to lower us below max speed if we started above it.
		if (bVelocityOverMax && Velocity.SizeSquared() < FMath::Square(VelocityClamp) && FVector::DotProduct(Acceleration, OldVelocity) > 0.0f) // TODO
		{
			Velocity = OldVelocity.GetSafeNormal() * VelocityClamp;
		}
	}

	// Set to input if not changed later
	_movementDirection = Acceleration;

	// Apply fluid friction
	if (bFluid)
	{
		Velocity = Velocity * (1.f - FMath::Min(Friction * DeltaTime, 1.f));
	}


	// Apply acceleration
	const FVector Old = Velocity;
	LastSpeedLimit = (this->*func)(DeltaTime, RequestedAcceleration + Acceleration);

	// Correct curves (needs to happen after func to ensure simulation)
	if (!bZeroAcceleration && !bVelocityOverMax)
	{
		// Correct movement for curves
		const FVector AccelDir = _movementDirection.GetSafeNormal();
		const float VelSize = Velocity.Size();
		Velocity = Velocity - (Velocity - AccelDir * VelSize) * FMath::Min(DeltaTime * Friction, 1.f);
	}

	// Get measured acceleration
	const FVector Delta = Velocity - Old;


	// Compute actual max value
	MaxSpeed = FMath::Max(RequestedSpeed, MaxSpeed);
	VelocityClamp = FMath::Min(MaxSpeed, LastSpeedLimit);

	// Clamp max speed
	Velocity = UTGOR_Math::AddLimited(Old, Delta, VelocityClamp);

	if (bUseRVOAvoidance)
	{
		CalcAvoidanceVelocity(DeltaTime);
	}
}

void UTGOR_CustomMovementComponent::SecureCustomVelocity(float DeltaTime, int32 Iterations)
{
	Iterations++;
	bJustTeleported = false;

	FVector Correction;
	FVector OldLocation = UpdatedComponent->GetComponentLocation();
	const FVector Adjusted = Velocity * DeltaTime;
	FHitResult Hit(1.f);
	SafeMoveUpdatedComponent(Adjusted, UpdatedComponent->GetComponentQuat(), true, Hit);

	if (Hit.Time < 1.f)
	{
		const FVector GravDir = FVector(0.f, 0.f, -1.f);
		const FVector VelDir = Velocity.GetSafeNormal();
		const float UpDown = GravDir | VelDir;

		bool bSteppedUp = false;
		if ((FMath::Abs(Hit.ImpactNormal.Z) < 0.2f) && (UpDown < 0.5f) && (UpDown > -0.2f) && CanStepUp(Hit))
		{
			float stepZ = UpdatedComponent->GetComponentLocation().Z;
			bSteppedUp = StepUp(GravDir, Adjusted * (1.f - Hit.Time), Hit);
			if (bSteppedUp)
			{
				OldLocation.Z = UpdatedComponent->GetComponentLocation().Z + (OldLocation.Z - stepZ);
			}
		}

		if (!bSteppedUp)
		{
			//adjust and try again
			HandleImpact(Hit, DeltaTime, Adjusted);
			SlideAlongSurface(Adjusted, (1.f - Hit.Time), Hit.Normal, Hit, true);
		}
	}

	if (!bJustTeleported && !HasAnimRootMotion() && !CurrentRootMotion.HasOverrideVelocity())
	{
		Velocity = (UpdatedComponent->GetComponentLocation() - OldLocation) / DeltaTime;
	}
}


float UTGOR_CustomMovementComponent::CalcCustomControlledFallingVelocity(float DeltaTime, const FVector& InputAcceleration)
{
	Velocity += InputAcceleration * DeltaTime;
	return(FLT_MAX);
}

float UTGOR_CustomMovementComponent::CalcCustomFreeFallingVelocity(float DeltaTime, const FVector& InputAcceleration)
{
	Velocity += InputAcceleration * DeltaTime;
	return(FLT_MAX);
}

float UTGOR_CustomMovementComponent::CalcCustomWalkingVelocity(float DeltaTime, const FVector& InputAcceleration)
{
	Velocity += InputAcceleration * DeltaTime;
	return(FLT_MAX);
}

float UTGOR_CustomMovementComponent::CalcCustomFlyingVelocity(float DeltaTime, const FVector& InputAcceleration)
{
	Velocity += InputAcceleration * DeltaTime;
	return(FLT_MAX);
}

float UTGOR_CustomMovementComponent::CalcCustomSkiingVelocity(float DeltaTime, const FVector& InputAcceleration)
{
	Velocity += InputAcceleration * DeltaTime;
	return(FLT_MAX);
}

float UTGOR_CustomMovementComponent::CalcCustomSwimmingVelocity(float DeltaTime, const FVector& InputAcceleration)
{
	Velocity += InputAcceleration * DeltaTime;
	return(FLT_MAX);
}

float UTGOR_CustomMovementComponent::CalcCustomClimbingVelocity(float DeltaTime, const FVector& InputAcceleration)
{
	Velocity += InputAcceleration * DeltaTime;
	return(FLT_MAX);
}

void UTGOR_CustomMovementComponent::CalcCustomMontageVelocity(float DeltaTime)
{
}

void UTGOR_CustomMovementComponent::PhysFalling(float DeltaTime, int32 Iterations)
{
	if (DeltaTime < MIN_TICK_TIME)
	{
		return;
	}

	FVector FallAcceleration = GetFallingLateralAcceleration(DeltaTime);
	FallAcceleration.Z = 0.f;
	const bool bHasAirControl = (FallAcceleration.SizeSquared2D() > 0.f);

	float remainingTime = DeltaTime;
	while ((remainingTime >= MIN_TICK_TIME) && (Iterations < MaxSimulationIterations))
	{
		Iterations++;
		const float timeTick = GetSimulationTimeStep(remainingTime, Iterations);
		remainingTime -= timeTick;

		const FVector OldLocation = UpdatedComponent->GetComponentLocation();
		const FQuat PawnRotation = UpdatedComponent->GetComponentQuat();
		bJustTeleported = false;

		RestorePreAdditiveRootMotionVelocity();

		FVector OldVelocity = Velocity;
		FVector VelocityNoAirControl = Velocity;

		// Apply input
		if (!HasAnimRootMotion() && !CurrentRootMotion.HasOverrideVelocity())
		{
			// Compute VelocityNoAirControl
			if (bHasAirControl)
			{
				// Find velocity *without* acceleration.
				TGuardValue<FVector> RestoreAcceleration(Acceleration, FVector::ZeroVector);
				TGuardValue<FVector> RestoreVelocity(Velocity, Velocity);

				// CalcVelocity(timeTick, FallingLateralFriction, false, BrakingDecelerationFalling);
				CalcCustomVelocity(timeTick, FallingLateralFriction, false, BrakingDecelerationFalling, &UTGOR_CustomMovementComponent::CalcCustomControlledFallingVelocity);
				VelocityNoAirControl = FVector(Velocity.X, Velocity.Y, OldVelocity.Z);
			}

			// Compute Velocity
			{
				// Acceleration = FallAcceleration for CalcVelocity(), but we restore it after using it.
				TGuardValue<FVector> RestoreAcceleration(Acceleration, FallAcceleration);

				// CalcVelocity(timeTick, FallingLateralFriction, false, BrakingDecelerationFalling);
				CalcCustomVelocity(timeTick, FallingLateralFriction, false, BrakingDecelerationFalling, &UTGOR_CustomMovementComponent::CalcCustomFreeFallingVelocity);
				Velocity.Z = OldVelocity.Z;
			}

			// Just copy Velocity to VelocityNoAirControl if they are the same (ie no acceleration).
			if (!bHasAirControl)
			{
				VelocityNoAirControl = Velocity;
			}
		}

		// Apply gravity
		const FVector Gravity(0.f, 0.f, GetGravityZ());
		Velocity = NewFallVelocity(Velocity, Gravity, timeTick);
		VelocityNoAirControl = NewFallVelocity(VelocityNoAirControl, Gravity, timeTick);
		const FVector AirControlAccel = (Velocity - VelocityNoAirControl) / timeTick;

		ApplyRootMotionToVelocity(timeTick);

		if (bNotifyApex && CharacterOwner->Controller && (Velocity.Z <= 0.f))
		{
			// Just passed jump apex since now going down
			bNotifyApex = false;
			NotifyJumpApex();
		}


		// Move
		FHitResult Hit(1.f);
		FVector Adjusted = 0.5f*(OldVelocity + Velocity) * timeTick;
		SafeMoveUpdatedComponent(Adjusted, PawnRotation, true, Hit);

		if (!HasValidData())
		{
			return;
		}

		float LastMoveTimeSlice = timeTick;
		float subTimeTickRemaining = timeTick * (1.f - Hit.Time);

		if (IsSwimming()) //just entered water
		{
			remainingTime += subTimeTickRemaining;
			StartSwimming(OldLocation, OldVelocity, timeTick, remainingTime, Iterations);
			return;
		}
		else if (Hit.bBlockingHit)
		{
			if (IsValidLandingSpot(UpdatedComponent->GetComponentLocation(), Hit))
			{
				remainingTime += subTimeTickRemaining;
				ProcessLanded(Hit, remainingTime, Iterations);
				return;
			}
			else
			{
				// Compute impact deflection based on final velocity, not integration step.
				// This allows us to compute a new velocity from the deflected vector, and ensures the full gravity effect is included in the slide result.
				Adjusted = Velocity * timeTick;

				// See if we can convert a normally invalid landing spot (based on the hit result) to a usable one.
				if (!Hit.bStartPenetrating && ShouldCheckForValidLandingSpot(timeTick, Adjusted, Hit))
				{
					const FVector PawnLocation = UpdatedComponent->GetComponentLocation();
					FFindFloorResult FloorResult;
					FindFloor(PawnLocation, FloorResult, false);
					if (FloorResult.IsWalkableFloor() && IsValidLandingSpot(PawnLocation, FloorResult.HitResult))
					{
						remainingTime += subTimeTickRemaining;
						ProcessLanded(FloorResult.HitResult, remainingTime, Iterations);
						return;
					}
				}

				HandleImpact(Hit, LastMoveTimeSlice, Adjusted);

				// If we've changed physics mode, abort.
				if (!HasValidData() || !IsFalling())
				{
					return;
				}

				// Limit air control based on what we hit.
				// We moved to the impact point using air control, but may want to deflect from there based on a limited air control acceleration.
				if (bHasAirControl)
				{
					const bool bCheckLandingSpot = false; // we already checked above.
					const FVector AirControlDeltaV = LimitAirControl(LastMoveTimeSlice, AirControlAccel, Hit, bCheckLandingSpot) * LastMoveTimeSlice;
					Adjusted = (VelocityNoAirControl + AirControlDeltaV) * LastMoveTimeSlice;
				}

				const FVector OldHitNormal = Hit.Normal;
				const FVector OldHitImpactNormal = Hit.ImpactNormal;
				FVector Delta = ComputeSlideVector(Adjusted, 1.f - Hit.Time, OldHitNormal, Hit);

				// Compute velocity after deflection (only gravity component for RootMotion)
				if (subTimeTickRemaining > KINDA_SMALL_NUMBER && !bJustTeleported)
				{
					const FVector NewVelocity = (Delta / subTimeTickRemaining);
					Velocity = HasAnimRootMotion() && !CurrentRootMotion.HasOverrideVelocity() ? FVector(Velocity.X, Velocity.Y, NewVelocity.Z) : NewVelocity;
				}

				if (subTimeTickRemaining > KINDA_SMALL_NUMBER && (Delta | Adjusted) > 0.f)
				{
					// Move in deflected direction.
					SafeMoveUpdatedComponent(Delta, PawnRotation, true, Hit);

					if (Hit.bBlockingHit)
					{
						// hit second wall
						LastMoveTimeSlice = subTimeTickRemaining;
						subTimeTickRemaining = subTimeTickRemaining * (1.f - Hit.Time);

						if (IsValidLandingSpot(UpdatedComponent->GetComponentLocation(), Hit))
						{
							remainingTime += subTimeTickRemaining;
							ProcessLanded(Hit, remainingTime, Iterations);
							return;
						}

						HandleImpact(Hit, LastMoveTimeSlice, Delta);

						// If we've changed physics mode, abort.
						if (!HasValidData() || !IsFalling())
						{
							return;
						}

						// Act as if there was no air control on the last move when computing new deflection.
						if (bHasAirControl && Hit.Normal.Z > VERTICAL_SLOPE_NORMAL_Z)
						{
							const FVector LastMoveNoAirControl = VelocityNoAirControl * LastMoveTimeSlice;
							Delta = ComputeSlideVector(LastMoveNoAirControl, 1.f, OldHitNormal, Hit);
						}

						FVector PreTwoWallDelta = Delta;
						TwoWallAdjust(Delta, Hit, OldHitNormal);

						// Limit air control, but allow a slide along the second wall.
						if (bHasAirControl)
						{
							const bool bCheckLandingSpot = false; // we already checked above.
							const FVector AirControlDeltaV = LimitAirControl(subTimeTickRemaining, AirControlAccel, Hit, bCheckLandingSpot) * subTimeTickRemaining;

							// Only allow if not back in to first wall
							if (FVector::DotProduct(AirControlDeltaV, OldHitNormal) > 0.f)
							{
								Delta += (AirControlDeltaV * subTimeTickRemaining);
							}
						}

						// Compute velocity after deflection (only gravity component for RootMotion)
						if (subTimeTickRemaining > KINDA_SMALL_NUMBER && !bJustTeleported)
						{
							const FVector NewVelocity = (Delta / subTimeTickRemaining);
							Velocity = HasAnimRootMotion() && !CurrentRootMotion.HasOverrideVelocity() ? FVector(Velocity.X, Velocity.Y, NewVelocity.Z) : NewVelocity;
						}

						// bDitch=true means that pawn is straddling two slopes, neither of which he can stand on
						bool bDitch = ((OldHitImpactNormal.Z > 0.f) && (Hit.ImpactNormal.Z > 0.f) && (FMath::Abs(Delta.Z) <= KINDA_SMALL_NUMBER) && ((Hit.ImpactNormal | OldHitImpactNormal) < 0.f));
						SafeMoveUpdatedComponent(Delta, PawnRotation, true, Hit);
						if (Hit.Time == 0.f)
						{
							// if we are stuck then try to side step
							FVector SideDelta = (OldHitNormal + Hit.ImpactNormal).GetSafeNormal2D();
							if (SideDelta.IsNearlyZero())
							{
								SideDelta = FVector(OldHitNormal.Y, -OldHitNormal.X, 0).GetSafeNormal();
							}
							SafeMoveUpdatedComponent(SideDelta, PawnRotation, true, Hit);
						}

						if (bDitch || IsValidLandingSpot(UpdatedComponent->GetComponentLocation(), Hit) || Hit.Time == 0.f)
						{
							remainingTime = 0.f;
							ProcessLanded(Hit, remainingTime, Iterations);
							return;
						}
						else if (GetPerchRadiusThreshold() > 0.f && Hit.Time == 1.f && OldHitImpactNormal.Z >= GetWalkableFloorZ())
						{
							// We might be in a virtual 'ditch' within our perch radius. This is rare.
							const FVector PawnLocation = UpdatedComponent->GetComponentLocation();
							const float ZMovedDist = FMath::Abs(PawnLocation.Z - OldLocation.Z);
							const float MovedDist2DSq = (PawnLocation - OldLocation).SizeSquared2D();
							if (ZMovedDist <= 0.2f * timeTick && MovedDist2DSq <= 4.f * timeTick)
							{
								Velocity.X += 0.25f * GetMaxSpeed() * (FMath::FRand() - 0.5f);
								Velocity.Y += 0.25f * GetMaxSpeed() * (FMath::FRand() - 0.5f);
								Velocity.Z = FMath::Max<float>(JumpZVelocity * 0.25f, 1.f);
								Delta = Velocity * timeTick;
								SafeMoveUpdatedComponent(Delta, PawnRotation, true, Hit);
							}
						}
					}
				}
			}
		}

		if (Velocity.SizeSquared2D() <= KINDA_SMALL_NUMBER * 10.f)
		{
			Velocity.X = 0.f;
			Velocity.Y = 0.f;
		}
	}
}

void UTGOR_CustomMovementComponent::PhysWalking(float DeltaTime, int32 Iterations)
{
	if (DeltaTime < MIN_TICK_TIME)
	{ return; }

	if (!CharacterOwner || (!CharacterOwner->Controller && !bRunPhysicsWithNoController && !HasAnimRootMotion() && !CurrentRootMotion.HasOverrideVelocity() && (CharacterOwner->Role != ROLE_SimulatedProxy)))
	{
		Acceleration = FVector::ZeroVector;
		Velocity = FVector::ZeroVector;
		return;
	}
	
	if (!UpdatedComponent->IsQueryCollisionEnabled())
	{
		SetMovementMode(MOVE_Walking);
		return;
	}

	checkCode(ensureMsgf(!Velocity.ContainsNaN(), TEXT("PhysWalking: Velocity contains NaN before Iteration (%s)\n%s"), *GetPathNameSafe(this), *Velocity.ToString()));
	
	bJustTeleported = false;
	bool bCheckedFall = false;
	bool bTriedLedgeMove = false;
	float remainingTime = DeltaTime;

	// Perform the move
	while ( (remainingTime >= MIN_TICK_TIME) && (Iterations < MaxSimulationIterations) && CharacterOwner && (CharacterOwner->Controller || bRunPhysicsWithNoController || HasAnimRootMotion() || CurrentRootMotion.HasOverrideVelocity() || (CharacterOwner->Role == ROLE_SimulatedProxy)) )
	{
		Iterations++;
		bJustTeleported = false;
		const float timeTick = GetSimulationTimeStep(remainingTime, Iterations);
		remainingTime -= timeTick;
		
		// Save current values
		UPrimitiveComponent * const OldBase = GetMovementBase();
		const FVector PreviousBaseLocation = (OldBase != NULL) ? OldBase->GetComponentLocation() : FVector::ZeroVector;
		const FVector OldLocation = UpdatedComponent->GetComponentLocation();
		const FFindFloorResult OldFloor = CurrentFloor;

		RestorePreAdditiveRootMotionVelocity();

		// Ensure velocity is horizontal.
		MaintainHorizontalGroundVelocity();
		const FVector OldVelocity = Velocity;

		// Apply acceleration
		if( !HasAnimRootMotion() && !CurrentRootMotion.HasOverrideVelocity() )
		{
			//CalcVelocity(timeTick, GroundFriction, false, BrakingDecelerationWalking);
			CalcCustomVelocity(timeTick, GroundFriction, false, BrakingDecelerationWalking, &UTGOR_CustomMovementComponent::CalcCustomWalkingVelocity);
			checkCode(ensureMsgf(!Velocity.ContainsNaN(), TEXT("PhysWalking: Velocity contains NaN after CalcVelocity (%s)\n%s"), *GetPathNameSafe(this), *Velocity.ToString()));
		}
		
		ApplyRootMotionToVelocity(timeTick);
		checkCode(ensureMsgf(!Velocity.ContainsNaN(), TEXT("PhysWalking: Velocity contains NaN after Root Motion application (%s)\n%s"), *GetPathNameSafe(this), *Velocity.ToString()));

		if( IsFalling() )
		{
			// Root motion could have put us into Falling.
			// No movement has taken place this movement tick so we pass on full time/past iteration count
			StartNewPhysics(remainingTime+timeTick, Iterations-1);
			return;
		}

		// Compute move parameters
		const FVector MoveVelocity = Velocity;
		const FVector Delta = timeTick * MoveVelocity;
		const bool bZeroDelta = Delta.IsNearlyZero();
		FStepDownResult StepDownResult;

		if ( bZeroDelta )
		{
			remainingTime = 0.f;
		}
		else
		{
			// try to move forward
			MoveAlongFloor(MoveVelocity, timeTick, &StepDownResult);

			if ( IsFalling() )
			{
				// pawn decided to jump up
				const float DesiredDist = Delta.Size();
				if (DesiredDist > KINDA_SMALL_NUMBER)
				{
					const float ActualDist = (UpdatedComponent->GetComponentLocation() - OldLocation).Size2D();
					remainingTime += timeTick * (1.f - FMath::Min(1.f,ActualDist/DesiredDist));
				}
				StartNewPhysics(remainingTime,Iterations);
				return;
			}
			else if ( IsSwimming() ) //just entered water
			{
				StartSwimming(OldLocation, OldVelocity, timeTick, remainingTime, Iterations);
				return;
			}
		}

		// Update floor.
		// StepUp might have already done it for us.
		if (StepDownResult.bComputedFloor)
		{
			CurrentFloor = StepDownResult.FloorResult;
		}
		else
		{
			FindFloor(UpdatedComponent->GetComponentLocation(), CurrentFloor, bZeroDelta, NULL);
		}

		// check for ledges here
		const bool bCheckLedges = !CanWalkOffLedges();
		if ( bCheckLedges && !CurrentFloor.IsWalkableFloor() )
		{
			// calculate possible alternate movement
			const FVector GravDir = FVector(0.f,0.f,-1.f);
			const FVector NewDelta = bTriedLedgeMove ? FVector::ZeroVector : GetLedgeMove(OldLocation, Delta, GravDir);
			if ( !NewDelta.IsZero() )
			{
				// first revert this move
				RevertMove(OldLocation, OldBase, PreviousBaseLocation, OldFloor, false);

				// avoid repeated ledge moves if the first one fails
				bTriedLedgeMove = true;

				// Try new movement direction
				Velocity = NewDelta/timeTick;
				remainingTime += timeTick;
				continue;
			}
			else
			{
				// see if it is OK to jump
				// @todo collision : only thing that can be problem is that oldbase has world collision on
				bool bMustJump = bZeroDelta || (OldBase == NULL || (!OldBase->IsQueryCollisionEnabled() && MovementBaseUtility::IsDynamicBase(OldBase)));
				if ( (bMustJump || !bCheckedFall) && CheckFall(OldFloor, CurrentFloor.HitResult, Delta, OldLocation, remainingTime, timeTick, Iterations, bMustJump) )
				{
					return;
				}
				bCheckedFall = true;

				// revert this move
				RevertMove(OldLocation, OldBase, PreviousBaseLocation, OldFloor, true);
				remainingTime = 0.f;
				break;
			}
		}
		else
		{
			// Validate the floor check
			if (CurrentFloor.IsWalkableFloor())
			{
				if (ShouldCatchAir(OldFloor, CurrentFloor))
				{
					CharacterOwner->OnWalkingOffLedge(OldFloor.HitResult.ImpactNormal, OldFloor.HitResult.Normal, OldLocation, timeTick);
					if (IsMovingOnGround())
					{
						// If still walking, then fall. If not, assume the user set a different mode they want to keep.
						StartFalling(Iterations, remainingTime, timeTick, Delta, OldLocation);
					}
					return;
				}

				AdjustFloorHeight();
				SetBase(CurrentFloor.HitResult.Component.Get(), CurrentFloor.HitResult.BoneName);
			}
			else if (CurrentFloor.HitResult.bStartPenetrating && remainingTime <= 0.f)
			{
				// The floor check failed because it started in penetration
				// We do not want to try to move downward because the downward sweep failed, rather we'd like to try to pop out of the floor.
				FHitResult Hit(CurrentFloor.HitResult);
				Hit.TraceEnd = Hit.TraceStart + FVector(0.f, 0.f, MAX_FLOOR_DIST);
				const FVector RequestedAdjustment = GetPenetrationAdjustment(Hit);
				ResolvePenetration(RequestedAdjustment, Hit, UpdatedComponent->GetComponentQuat());
			}

			// check if just entered water
			if ( IsSwimming() )
			{
				StartSwimming(OldLocation, Velocity, timeTick, remainingTime, Iterations);
				return;
			}

			// See if we need to start falling.
			if (!CurrentFloor.IsWalkableFloor() && !CurrentFloor.HitResult.bStartPenetrating)
			{
				const bool bMustJump = bJustTeleported || bZeroDelta || (OldBase == NULL || (!OldBase->IsQueryCollisionEnabled() && MovementBaseUtility::IsDynamicBase(OldBase)));
				if ((bMustJump || !bCheckedFall) && CheckFall(OldFloor, CurrentFloor.HitResult, Delta, OldLocation, remainingTime, timeTick, Iterations, bMustJump) )
				{
					return;
				}
				bCheckedFall = true;
			}
		}


		// Allow overlap events and such to change physics state and velocity
		if (IsMovingOnGround())
		{
			// Make velocity reflect actual move
			if( !bJustTeleported && !HasAnimRootMotion() && !CurrentRootMotion.HasOverrideVelocity() && timeTick >= MIN_TICK_TIME)
			{
				// TODO-RootMotionSource: Allow this to happen during partial override Velocity, but only set allowed axes?
				Velocity = (UpdatedComponent->GetComponentLocation() - OldLocation) / timeTick;
			}
		}
		
		// If we didn't move at all this iteration then abort (since future iterations will also be stuck).
		if (UpdatedComponent->GetComponentLocation() == OldLocation)
		{
			remainingTime = 0.f;
			break;
		}	
	}

	if (IsMovingOnGround())
	{
		MaintainHorizontalGroundVelocity();
	}
}

void UTGOR_CustomMovementComponent::PhysSwimming(float DeltaTime, int32 Iterations)
{
	if (DeltaTime < MIN_TICK_TIME)
	{
		return;
	}

	float Depth = ImmersionDepth();
	RestorePreAdditiveRootMotionVelocity();

	if (!HasAnimRootMotion() && !CurrentRootMotion.HasOverrideVelocity())
	{
		const float Friction = 0.5f * GetPhysicsVolume()->FluidFriction * Depth;

		//CalcVelocity(deltaTime, Friction, false, BrakingDecelerationSwimming);
		CalcCustomVelocity(DeltaTime, Friction, false, BrakingDecelerationSwimming, &UTGOR_CustomMovementComponent::CalcCustomSwimmingVelocity);
	}

	ApplyRootMotionToVelocity(DeltaTime);
	SecureCustomVelocity(DeltaTime, Iterations);
}


float UTGOR_CustomMovementComponent::SwimCustom(FVector Delta, FHitResult& Hit)
{
	float airTime = 0.f;
	FVector Start = UpdatedComponent->GetComponentLocation();
	SafeMoveUpdatedComponent(Delta, UpdatedComponent->GetComponentQuat(), true, Hit);

	if (!GetPhysicsVolume()->bWaterVolume) //then left water
	{
		const FVector End = FindWaterLine(Start, UpdatedComponent->GetComponentLocation());
		const float DesiredDist = Delta.Size();
		if (End != UpdatedComponent->GetComponentLocation() && DesiredDist > KINDA_SMALL_NUMBER)
		{
			airTime = (End - UpdatedComponent->GetComponentLocation()).Size() / DesiredDist;
			if (((UpdatedComponent->GetComponentLocation() - Start) | (End - UpdatedComponent->GetComponentLocation())) > 0.f)
			{
				airTime = 0.f;
			}
			SafeMoveUpdatedComponent(End - UpdatedComponent->GetComponentLocation(), UpdatedComponent->GetComponentQuat(), true, Hit);
		}
	}
	return airTime;
}


void UTGOR_CustomMovementComponent::PhysCustom(float DeltaTime, int32 Iterations)
{
	switch (CustomMovementMode)
	{
	case ETGOR_CustomMovementMode::Skiing:
		PhysSkiing(DeltaTime, Iterations);
		break;
	case ETGOR_CustomMovementMode::Montage:
		PhysMontage(DeltaTime, Iterations);
		break;
	case ETGOR_CustomMovementMode::Climb:
		PhysClimb(DeltaTime, Iterations);
		break;
	case ETGOR_CustomMovementMode::Waypoint:
		PhysMontage(DeltaTime, Iterations);
		break;
	default: break;
	}
}

void UTGOR_CustomMovementComponent::PhysFlying(float DeltaTime, int32 Iterations)
{
	if (DeltaTime < MIN_TICK_TIME)
	{
		return;
	}

	RestorePreAdditiveRootMotionVelocity();

	if (!HasAnimRootMotion() && !CurrentRootMotion.HasOverrideVelocity())
	{
		const float Friction = 0.5f * GetPhysicsVolume()->FluidFriction;
		//CalcVelocity(deltaTime, Friction, true, BrakingDecelerationFlying);
		CalcCustomVelocity(DeltaTime, Friction, true, BrakingDecelerationFlying, &UTGOR_CustomMovementComponent::CalcCustomFlyingVelocity);
	}

	ApplyRootMotionToVelocity(DeltaTime);

	SecureCustomVelocity(DeltaTime, Iterations);
}

void UTGOR_CustomMovementComponent::PhysSkiing(float DeltaTime, int32 Iterations)
{
	if (DeltaTime < MIN_TICK_TIME)
	{
		return;
	}

	RestorePreAdditiveRootMotionVelocity();

	if (!HasAnimRootMotion() && !CurrentRootMotion.HasOverrideVelocity())
	{
		const float Friction = 0.5f * GetPhysicsVolume()->FluidFriction;
		// CalcVelocity(deltaTime, Friction, true, BrakingDecelerationFlying);
		CalcCustomVelocity(DeltaTime, Friction, true, BrakingDecelerationFlying, &UTGOR_CustomMovementComponent::CalcCustomSkiingVelocity);
	}

	ApplyRootMotionToVelocity(DeltaTime);

	SecureCustomVelocity(DeltaTime, Iterations);
}


void UTGOR_CustomMovementComponent::PhysMontage(float DeltaTime, int32 Iterations)
{
	if (DeltaTime < MIN_TICK_TIME)
	{ return; }

	RestorePreAdditiveRootMotionVelocity();
	
	PhysicsRotation(DeltaTime);

	const float Friction = 0.5f * GetPhysicsVolume()->FluidFriction;
	CalcCustomMontageVelocity(DeltaTime);

	ApplyRootMotionToVelocity(DeltaTime);

	SecureCustomVelocity(DeltaTime, Iterations);
}

void UTGOR_CustomMovementComponent::PhysClimb(float DeltaTime, int32 Iterations)
{
	if (DeltaTime < MIN_TICK_TIME)
	{
		return;
	}
	RestorePreAdditiveRootMotionVelocity();

	if (!HasAnimRootMotion() && !CurrentRootMotion.HasOverrideVelocity())
	{
		const float Friction = 0.5f * GetPhysicsVolume()->FluidFriction;
		// CalcVelocity(deltaTime, Friction, true, BrakingDecelerationFlying);
		CalcCustomVelocity(DeltaTime, Friction, true, BrakingDecelerationFlying, &UTGOR_CustomMovementComponent::CalcCustomClimbingVelocity);
	}

	ApplyRootMotionToVelocity(DeltaTime);

	SecureCustomVelocity(DeltaTime, Iterations);
}



float UTGOR_CustomMovementComponent::SlideAlongSurface(const FVector& Delta, float Time, const FVector& InNormal, FHitResult& Hit, bool bHandleImpact)
{
	if (!Hit.bBlockingHit)
	{
		return 0.f;
	}

	FVector Normal(InNormal);
	if (IsMovingOnGround())
	{
		// We don't want to be pushed up an unwalkable surface.
		if (Normal.Z > 0.f)
		{
			if (!IsHardlyWalkable(Hit))
			{
				Normal = Normal.GetSafeNormal2D();
			}
		}
		else if (Normal.Z < -KINDA_SMALL_NUMBER)
		{
			// Don't push down into the floor when the impact is on the upper portion of the capsule.
			if (CurrentFloor.FloorDist < MIN_FLOOR_DIST && CurrentFloor.bBlockingHit)
			{
				const FVector FloorNormal = CurrentFloor.HitResult.Normal;
				const bool bFloorOpposedToMovement = (Delta | FloorNormal) < 0.f && (FloorNormal.Z < 1.f - DELTA);
				if (bFloorOpposedToMovement)
				{
					Normal = FloorNormal;
				}

				Normal = Normal.GetSafeNormal2D();
			}
		}
	}

	return UPawnMovementComponent::SlideAlongSurface(Delta, Time, Normal, Hit, bHandleImpact);
}

bool UTGOR_CustomMovementComponent::IsHardlyWalkable(const FHitResult& Hit) const
{
	if (!Hit.IsValidBlockingHit())
	{
		// No hit, or starting in penetration
		return false;
	}

	// Never walk up vertical surfaces.
	if (Hit.ImpactNormal.Z < KINDA_SMALL_NUMBER)
	{
		return false;
	}

	float TestWalkableZ = GetWalkableFloorZ() - 0.2f;

	// See if this component overrides the walkable floor z.
	const UPrimitiveComponent* HitComponent = Hit.Component.Get();
	if (HitComponent)
	{
		const FWalkableSlopeOverride& SlopeOverride = HitComponent->GetWalkableSlopeOverride();
		TestWalkableZ = SlopeOverride.ModifyWalkableFloorZ(TestWalkableZ);
	}

	// Can't walk on this surface if it is too steep.
	if (Hit.ImpactNormal.Z < TestWalkableZ)
	{
		return false;
	}

	return true;
}



void UTGOR_CustomMovementComponent::PhysicsRotation(float DeltaTime)
{
	if (!(bOrientRotationToMovement || bUseControllerDesiredRotation))
	{
		return;
	}

	if (!HasValidData() || (!CharacterOwner->Controller && !bRunPhysicsWithNoController))
	{
		return;
	}

	//if (GetOwner()->Role == ROLE_AutonomousProxy)	RPTCK(FString::SanitizeFloat(DeltaTime) + " rotation client")
	//if (GetOwner()->Role == ROLE_Authority)	RPTCK(FString::SanitizeFloat(DeltaTime) + " rotation server")

	CalcTurnDelta(DeltaTime);
	CustomRotation(DeltaTime, TurnDelta);
}

void UTGOR_CustomMovementComponent::CustomRotation(float DeltaTime, FRotator Delta)
{
	AActor* Actor = GetOwner();
	//UpdatedComponent->AddWorldRotation(TurnDelta, true);

	const FQuat Rotation = UpdatedComponent->GetComponentQuat();
	const FQuat NewWorldRotation = TurnDelta.Quaternion() * Rotation;
	RotateCustomComponent(DeltaTime, CollisionSphereCorrectionSpeed, NewWorldRotation, true);
}

bool UTGOR_CustomMovementComponent::CanCrouchInCurrentState() const
{
	return CanEverCrouch();
}


void UTGOR_CustomMovementComponent::MoveDirection(float DeltaTime, float Input, const FVector& Direction)
{
	if (CurveCorrection)
	{
		_movementDirection = Direction.GetSafeNormal();
	}

	Velocity += Direction * Input * DeltaTime;
}


void UTGOR_CustomMovementComponent::MoveDirection(float DeltaTime, float Input, const FVector2D& Direction)
{
	MoveDirection(DeltaTime, Input, FVector(Direction, 0.0f));
}


void UTGOR_CustomMovementComponent::CalcTurnDelta(float DeltaTime)
{
	TurnDelta.Yaw =		FMath::FInterpConstantTo(0.0f, TurnAdditive.Yaw, DeltaTime, TurnSpeed);
	TurnDelta.Pitch =	FMath::FInterpConstantTo(0.0f, TurnAdditive.Pitch, DeltaTime, TurnSpeed);
	
	TurnAdditive -= TurnDelta;
}


bool UTGOR_CustomMovementComponent::MoveUpdatedComponentImpl(const FVector& Delta, const FQuat& NewRotation, bool bSweep, FHitResult* OutHit, ETeleportType Teleport)
{
	const bool CapsuleSweep = true;

	if (UpdatedComponent)
	{
		FVector NewDelta = ConstrainDirectionToPlane(Delta);

		// Only probe with collision spheres if enabled and sweeping
		if (bSweep && CollisionSphereCorrectionEnabled && IsValid(CollisionSpheres))
		{
			// Probe with collision spheres to stop movement and slide along wall
			FHitResult Hit;
			if (CollisionSpheres->Probe(NewDelta, NewRotation, true, Hit, MoveComponentFlags, Teleport))
			{
				if (Hit.bBlockingHit)
				{
					// Move rest vector and reflect off wall
					FVector Reflect = FVector::VectorPlaneProject(NewDelta * (1.0f - Hit.Time), Hit.Normal);
					NewDelta = NewDelta * Hit.Time + Reflect;
				}

				// move actual capsule
				return UpdatedComponent->MoveComponent(NewDelta, NewRotation, true, OutHit, MoveComponentFlags, Teleport);
			}
		}

		return UpdatedComponent->MoveComponent(NewDelta, NewRotation, bSweep, OutHit, MoveComponentFlags, Teleport);
	}

	return(false);
}


bool UTGOR_CustomMovementComponent::RotateCustomComponent(float DeltaSeconds, float Multiplier, const FQuat& NewRotation, bool bSweep, ETeleportType Teleport)
{
	if (CollisionSphereCorrectionEnabled && IsValid(CollisionSpheres))
	{
		// Pitch collision spheres
		CollisionSpheres->RelativeRotation.Pitch = CollisionSphereCorrectionPitch;

		// Correction multiplier
		const float Factor = FMath::Min(Multiplier * DeltaSeconds, 1.0f);

		// Correct for rotation
		const FVector Correction = CollisionSpheres->ProbeCenter();
		return(UpdatedComponent->MoveComponent(Correction * Factor, NewRotation, bSweep, NULL, MoveComponentFlags, Teleport));
	}
	else
	{
		return(UpdatedComponent->MoveComponent(FVector::ZeroVector, NewRotation, bSweep, NULL, MoveComponentFlags, Teleport));
	}
}
