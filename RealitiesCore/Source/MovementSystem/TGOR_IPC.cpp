// The Gateway of Realities: Planes of Existence.

#include "TGOR_IPC.h"
#include "RealitiesUtility/Utility/TGOR_Math.h"

FVector4 FTGOR_PendulumProperties::PositionToPendulum(const FTGOR_MovementPosition& Position, const FVector& UpDirection, const FVector& ForwardDirection) const
{
	FVector4 State;

	const FVector Axis = Position.Angular.GetAxisZ();
	const float Dot = ForwardDirection | Axis;
	State[2] = FMath::Acos(Dot);
	State[0] = (ForwardDirection | Position.Linear) - Dot * Length;

	return State;
}

FVector4 FTGOR_PendulumProperties::DynamicToPendulum(const FTGOR_MovementDynamic& Dynamic, const FVector& UpDirection, const FVector& ForwardDirection) const
{
	FVector4 State = PositionToPendulum(Dynamic, UpDirection, ForwardDirection);

	const FVector Axis = Dynamic.Angular.GetAxisZ();
	const FVector Right = ForwardDirection ^ UpDirection;
	State[3] = Right | Dynamic.AngularVelocity;
	State[1] = (ForwardDirection | Dynamic.LinearVelocity) - State[3] * Length;

	return State;
}

FTGOR_PendulumState FTGOR_PendulumProperties::PositionToPendulum(const FTGOR_MovementPosition& Position) const
{
	FTGOR_PendulumState State;
	const FVector Axis = Position.Angular.GetAxisZ();

	State.X[2] = FMath::Acos(Axis | FVector::RightVector);
	State.Y[2] = FMath::Acos(Axis | FVector::ForwardVector);
	State.R[0] = Position.Angular.GetTwistAngle(Axis);

	State.X[0] = Position.Linear.X - Axis.X * Length;
	State.Y[0] = Position.Linear.Y - Axis.Y * Length;
	State.Z[0] = Position.Linear.Z - Axis.Z * Length;
	return State;
}

FTGOR_PendulumState FTGOR_PendulumProperties::DynamicToPendulum(const FTGOR_MovementDynamic& Dynamic) const
{
	FTGOR_PendulumState State = PositionToPendulum(Dynamic);

	State.X[3] = Dynamic.AngularVelocity | FVector::RightVector;
	State.Y[3] = Dynamic.AngularVelocity | FVector::ForwardVector;
	State.R[1] = Dynamic.AngularVelocity | FVector::UpVector;

	const FVector Axis = Dynamic.Angular.GetAxisZ();
	const FVector Radial = (Axis * Length) ^ Dynamic.AngularVelocity;
	State.X[1] = Dynamic.LinearVelocity.X - Radial.X;
	State.Y[1] = Dynamic.LinearVelocity.Y - Radial.Y;
	State.Z[1] = Dynamic.LinearVelocity.Z - Radial.Z;
	return State;
}

FTGOR_MovementPosition FTGOR_PendulumProperties::PendulumToPosition(const FTGOR_PendulumState& State) const
{
	FTGOR_MovementPosition Position;
	Position.Angular = FQuat(FVector::RightVector, State.X[2]) * FQuat(FVector::ForwardVector, State.Y[2]) * FQuat(FVector::UpVector, State.R[0]);

	const FVector Axis = Position.Angular.GetAxisZ();
	Position.Linear = FVector(State.X[0], State.Y[0], State.Z[0]) + Axis * Length;
	return Position;
}

FTGOR_MovementDynamic FTGOR_PendulumProperties::PendulumToDynamic(const FTGOR_PendulumState& State) const
{
	FTGOR_MovementDynamic Dynamic = PendulumToPosition(State);
	Dynamic.AngularVelocity = FVector::RightVector * State.X[3] + FVector::ForwardVector * State.Y[3] + FVector::UpVector * State.R[1];

	const FVector Axis = Dynamic.Angular.GetAxisZ();
	const FVector Radial = (Axis * Length) ^ Dynamic.AngularVelocity;
	Dynamic.LinearVelocity = FVector(State.X[1], State.Y[1], State.Z[1]) + Radial;
	return Dynamic;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void FTGOR_IPCProperties::Generate(const FTGOR_PendulumProperties& PendulumProperties, const FTGOR_RiccatiProperties& RiccatiProperties)
{
	const float L = PendulumProperties.Length;
	const float M = PendulumProperties.Mass;
	const float I = PendulumProperties.Inertia;
	const float G = PendulumProperties.Gravity;

	const float LinvI = L / I;
	const float invM = 1.0f / M;
	const float mgL = M * G * L * LinvI;

	StateCoef = FMatrix();
	StateCoef.M[0][1] = 1.0f;
	StateCoef.M[1][2] = mgL * L;
	StateCoef.M[2][3] = 1.0f;
	StateCoef.M[3][2] = mgL;

	ForceCoef = FVector4();
	ForceCoef[1] = invM + LinvI * L;
	ForceCoef[3] = LinvI;

	const FMatrix A = StateCoef;
	const FMatrix AT = StateCoef.GetTransposed();

	const FVector4 B = ForceCoef;

	FMatrix Q;
	Q.M[0][0] = RiccatiProperties.Q;
	Q.M[1][1] = RiccatiProperties.Q;
	Q.M[2][2] = RiccatiProperties.Q;
	Q.M[3][3] = RiccatiProperties.Q;

	const float Dt = RiccatiProperties.Dt;
	const float Rinv = 1.0f / RiccatiProperties.R;

	FMatrix P = Q;

	// Source from https://github.com/TakaHoribe/Riccati_Solver/blob/master/riccati_solver.cpp
	for (int32 Iteration = 0; Iteration < RiccatiProperties.Iterations; Iteration++)
	{
		P = P + (P * A + AT * P + P * -Dot4(B * Rinv, B) * P + Q) * Dt;
	}

	ForceResponse = P.GetTransposed().TransformFVector4(B) * Rinv;
}

void FTGOR_IPCProperties::SimulateForPosition(FVector4& State, float Position, float DeltaTime) const
{
	const float Force = Dot4(ForceResponse, State - FVector4(Position, State.Y, State.Z, State.W));
	const FVector4 DState = StateCoef.TransformFVector4(State) + ForceCoef * Force;
	State += DState * DeltaTime;
}

void FTGOR_IPCProperties::SimulateForVelocity(FVector4& State, float Velocity, float DeltaTime) const
{
	const float Force = Dot4(ForceResponse, State - FVector4(State.X, Velocity, State.Z, State.W));
	const FVector4 DState = StateCoef.TransformFVector4(State) + ForceCoef * Force;
	State += DState * DeltaTime;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

ATGOR_IPCActor::ATGOR_IPCActor()
	: Super()
{
}