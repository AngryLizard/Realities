// The Gateway of Realities: Planes of Existence.


#include "TGOR_PipelineSocketTask.h"
#include "DimensionSystem/Components/TGOR_PilotComponent.h"
#include "SocketSystem/Components/TGOR_PipelineComponent.h"
#include "SocketSystem/Content/TGOR_PipelineSocket.h"

#include "Net/UnrealNetwork.h"
#include "Engine/ActorChannel.h"

FTGOR_PipelineSocketParenting::FTGOR_PipelineSocketParenting()
: Socket(nullptr)
{
}

bool FTGOR_PipelineSocketParenting::operator==(const FTGOR_PipelineSocketParenting& Other) const
{
	return Component == Other.Component && Socket == Other.Socket;
}

bool FTGOR_PipelineSocketParenting::operator!=(const FTGOR_PipelineSocketParenting& Other) const
{
	return Component != Other.Component || Socket != Other.Socket;
}


FTGOR_PipelineSocketState::FTGOR_PipelineSocketState()
:	SocketDistance(0.0f),
	SocketSpeed(0.0f),
	SocketDirection(1.0f)
{
}

void FTGOR_PipelineSocketState::SetLerpState(const FTGOR_PipelineSocketState& Other, float Alpha)
{
	SocketDistance = FMath::Lerp(SocketDistance, Other.SocketDistance, Alpha);
	SocketSpeed = FMath::Lerp(SocketSpeed, Other.SocketSpeed, Alpha);
	SocketDirection = FMath::Lerp(SocketDirection, Other.SocketDirection, Alpha);
}

float FTGOR_PipelineSocketState::CompareState(const FTGOR_PipelineSocketState& Other, const FTGOR_MovementThreshold& Threshold) const
{
	const float LinearDistance = Threshold.ThresholdDistance(SocketDistance, Other.SocketDistance, FMath::Square(SocketSpeed));
	const float SpeedDistance = Threshold.ThresholdDistance(SocketSpeed, Other.SocketSpeed, 0.0f);
	const float DirectionDistance = FMath::Square(SocketDirection - Other.SocketDirection) / 4;
	return LinearDistance + SpeedDistance + DirectionDistance;
}

////////////////////////////////////////////////////////////////////////////////////////////////////


UTGOR_PipelineSocketTask::UTGOR_PipelineSocketTask()
{
}

void UTGOR_PipelineSocketTask::GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION(UTGOR_PipelineSocketTask, Parenting, COND_None);
	DOREPLIFETIME_CONDITION(UTGOR_PipelineSocketTask, State, COND_None);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

UTGOR_MobilityComponent* UTGOR_PipelineSocketTask::GetParent() const
{
	return Parenting.Component.Get();
}

FTGOR_MovementPosition UTGOR_PipelineSocketTask::ComputePosition() const
{
	// TODO: Add Spline offset
	if (Parenting.Component.IsValid())
	{
		const FTGOR_MovementPosition Position = Parenting.Component->ComputePosition();
		const FTGOR_MovementDynamic Part = GetDynamic();
		return Part.BaseToPosition(Position);
	}
	return FTGOR_MovementPosition();
}

FTGOR_MovementSpace UTGOR_PipelineSocketTask::ComputeSpace() const
{
	// TODO: Add Spline offset
	if (Parenting.Component.IsValid())
	{
		const FTGOR_MovementSpace Space = Parenting.Component->ComputeSpace();
		const FTGOR_MovementDynamic Part = GetDynamic();
		return Part.BaseToSpace(Space);
	}
	return FTGOR_MovementSpace();
}

void UTGOR_PipelineSocketTask::Update(float DeltaTime)
{
	Super::Update(DeltaTime);
}

void UTGOR_PipelineSocketTask::Unparent()
{
	Parent(nullptr, nullptr);
}

void UTGOR_PipelineSocketTask::Register()
{
	Super::Register();
	if (Parenting.Component.IsValid())
	{
		Parenting.Component->RegisterAttachToSocket(this);
	}
}

void UTGOR_PipelineSocketTask::Unregister()
{
	Super::Unregister();
	if (Parenting.Component.IsValid())
	{
		Parenting.Component->RegisterDetachFromSocket(this);
	}
}


UTGOR_Socket* UTGOR_PipelineSocketTask::GetSocket() const
{
	return Parenting.Socket;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

bool UTGOR_PipelineSocketTask::IsForward() const
{
	return State.SocketDirection > 0.0f;
}

float UTGOR_PipelineSocketTask::GetDistance() const
{
	return State.SocketDistance;
}

float UTGOR_PipelineSocketTask::GetSpeed() const
{
	return State.SocketSpeed;
}

float UTGOR_PipelineSocketTask::GetBlend(float Threshold) const
{
	if (Parenting.Component.IsValid() && !Parenting.Component->IsClosedLoop())
	{
		const float Start = Parenting.Component->GetDistanceFromStart(State.SocketDistance, Threshold);
		const float End = Parenting.Component->GetDistanceFromEnd(State.SocketDistance, Threshold);
		return 1.0f - FMath::Min(Start, End) / Threshold;
	}
	return 0.0f;
}


FTGOR_MovementDynamic UTGOR_PipelineSocketTask::GetDynamic() const
{
	if (Parenting.Component.IsValid())
	{
		USplineComponent* SplineComponent = Parenting.Component->GetSpline();
		if (IsValid(SplineComponent))
		{
			const FQuat AngleRight(FVector::RightVector, (PI / 2));
			const FQuat AngleFlip(FVector::ForwardVector, (IsForward() ? 0.0f : PI));

			FTGOR_MovementDynamic Dynamic;
			Dynamic.Linear = SplineComponent->GetLocationAtDistanceAlongSpline(State.SocketDistance, ESplineCoordinateSpace::Local);
			Dynamic.Angular = SplineComponent->GetQuaternionAtDistanceAlongSpline(State.SocketDistance, ESplineCoordinateSpace::Local) * AngleRight * AngleFlip;
			Dynamic.Angular.Normalize();

			const FVector Direction = SplineComponent->GetDirectionAtDistanceAlongSpline(State.SocketDistance, ESplineCoordinateSpace::Local);
			Dynamic.LinearVelocity = Direction * State.SocketSpeed;
			return Dynamic;
		}
	}
	return FTGOR_MovementDynamic();
}

////////////////////////////////////////////////////////////////////////////////////////////////////

bool UTGOR_PipelineSocketTask::Simulate(UTGOR_PilotComponent* Attachee, float Timestep)
{
	if (Parenting.Component.IsValid())
	{
		const int32 Last = Parenting.Component->GetLastSplinePoint();
		const float TotalDistance = Parenting.Component->GetDistanceAt(Last);

		// Apply external forces
		const float Acceleration = Parenting.Component->GetAccelerationAtDistance(State.SocketDistance, State.SocketSpeed, State.SocketDirection);
		State.SocketSpeed += Acceleration * Timestep;

		// Move along spline
		State.SocketDistance += State.SocketSpeed * Timestep;

		// Handle edge cases
		if (Parenting.Component->IsClosedLoop())
		{
			while (State.SocketDistance > TotalDistance) State.SocketDistance -= TotalDistance;
			while (State.SocketDistance < 0.0f) State.SocketDistance += TotalDistance;
		}
		else if (State.SocketDistance > TotalDistance)
		{
			if (Parenting.Component->CollideWithEnd(Attachee))
			{
				State.SocketDistance = TotalDistance;
				State.SocketSpeed = (State.SocketSpeed > 0.0f ? -State.SocketSpeed : State.SocketSpeed);
			}
			else
			{
				return false;
			}
		}
		else if (State.SocketDistance < 0.0f)
		{
			if (Parenting.Component->CollideWithStart(Attachee))
			{
				State.SocketDistance = TotalDistance;
				State.SocketSpeed = (State.SocketSpeed > 0.0f ? -State.SocketSpeed : State.SocketSpeed);
			}
			else
			{
				return false;
			}
		}
		return true;
	}
	return false;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void UTGOR_PipelineSocketTask::Parent(UTGOR_PipelineComponent* Parent, UTGOR_PipelineSocket* Socket)
{
	if (Parenting.Component != Parent || Parenting.Socket != Socket)
	{
		const bool Reregister = IsRegistered();
		if (Reregister)
		{
			Unregister();
		}

		Parenting.Component = Parent;
		Parenting.Socket = Socket;

		if (Reregister)
		{
			Register();
		}
	}
}

bool UTGOR_PipelineSocketTask::CanParent(const UTGOR_PipelineComponent* Parent, UTGOR_PipelineSocket* Socket) const
{
	return IsValid(Parent) && Identifier.Component->CanParent(Parent) && Parent->CanAttachToPipeline(Identifier.Component, Socket);
}
