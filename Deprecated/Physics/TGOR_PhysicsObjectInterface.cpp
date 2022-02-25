// The Gateway of Realities: Planes of Existence.


#include "TGOR_PhysicsObjectInterface.h"
FTGOR_InertiaTensor::FTGOR_InertiaTensor()
:	X(FVector::ZeroVector),
	Y(FVector::ZeroVector),
	Z(FVector::ZeroVector)
{
}

FTGOR_InertiaTensor::FTGOR_InertiaTensor(const FVector& Diag)
:	X(FVector(Diag.X, 0.0f, 0.0f)),
	Y(FVector(0.0f, Diag.Y, 0.0f)),
	Z(FVector(0.0f, 0.0f, Diag.Z))
{
}

FTGOR_InertiaTensor::FTGOR_InertiaTensor(const FVector& InX, const FVector& InY, const FVector& InZ)
	: X(InX), Y(InY), Z(InZ)
{
}

FTGOR_InertiaTensor::FTGOR_InertiaTensor(const FTGOR_InertiaTensor& In)
	: X(In.X), Y(In.Y), Z(In.Z)
{
}

FVector FTGOR_InertiaTensor::GetColumnX() const
{
	return(FVector(X.X, Y.X, Z.X));
}

FVector FTGOR_InertiaTensor::GetColumnY() const
{
	return(FVector(X.Y, Y.Y, Z.Y));
}

FVector FTGOR_InertiaTensor::GetColumnZ() const
{
	return(FVector(X.Z, Y.Z, Z.Z));
}

FVector FTGOR_InertiaTensor::operator*(const FVector& Vector) const
{
	const float OutX = FVector::DotProduct(Vector, X);
	const float OutY = FVector::DotProduct(Vector, Y);
	const float OutZ = FVector::DotProduct(Vector, Z);
	return(FVector(OutX, OutY, OutZ));
}

FVector FTGOR_InertiaTensor::operator^(const FVector& Vector) const
{
	const float OutX = FVector::DotProduct(Vector, FVector(X.X, Y.X, Z.X));
	const float OutY = FVector::DotProduct(Vector, FVector(X.Y, Y.Y, Z.Y));
	const float OutZ = FVector::DotProduct(Vector, FVector(X.Z, Y.Z, Z.Z));
	return(FVector(OutX, OutY, OutZ));
}

FTGOR_InertiaTensor FTGOR_InertiaTensor::Scale(const FVector& Scale) const
{
	const FVector OutX = FVector(X.X * Scale.X, X.Y * Scale.Y, X.Z * Scale.Z);
	const FVector OutY = FVector(Y.X * Scale.X, Y.Y * Scale.Y, Y.Z * Scale.Z);
	const FVector OutZ = FVector(Z.X * Scale.X, Z.Y * Scale.Y, Z.Z * Scale.Z);
	return(FTGOR_InertiaTensor(OutX, OutY, OutZ));
}

FTGOR_InertiaTensor FTGOR_InertiaTensor::InvScale(const FVector& Scale) const
{
	const FVector OutX = FVector(X.X / Scale.X, X.Y / Scale.Y, X.Z / Scale.Z);
	const FVector OutY = FVector(Y.X / Scale.X, Y.Y / Scale.Y, Y.Z / Scale.Z);
	const FVector OutZ = FVector(Z.X / Scale.X, Z.Y / Scale.Y, Z.Z / Scale.Z);
	return(FTGOR_InertiaTensor(OutX, OutY, OutZ));
}

FTGOR_InertiaTensor FTGOR_InertiaTensor::operator*(const FTGOR_InertiaTensor& Tensor) const
{
	FTGOR_InertiaTensor Result;
	Result.X.X = X.X * Tensor.X.X + X.Y * Tensor.Y.X + X.Z * Tensor.Z.X;
	Result.X.Y = X.X * Tensor.X.Y + X.Y * Tensor.Y.Y + X.Z * Tensor.Z.Y;
	Result.X.Z = X.X * Tensor.X.Z + X.Y * Tensor.Y.Z + X.Z * Tensor.Z.Z;

	Result.Y.X = Y.X * Tensor.X.X + Y.Y * Tensor.Y.X + Y.Z * Tensor.Z.X;
	Result.Y.Y = Y.X * Tensor.X.Y + Y.Y * Tensor.Y.Y + Y.Z * Tensor.Z.Y;
	Result.Y.Z = Y.X * Tensor.X.Z + Y.Y * Tensor.Y.Z + Y.Z * Tensor.Z.Z;

	Result.Z.X = Y.X * Tensor.X.X + Z.Y * Tensor.Y.X + Z.Z * Tensor.Z.X;
	Result.Z.Y = Y.X * Tensor.X.Y + Z.Y * Tensor.Y.Y + Z.Z * Tensor.Z.Y;
	Result.Z.Z = Y.X * Tensor.X.Z + Z.Y * Tensor.Y.Z + Z.Z * Tensor.Z.Z;
	return(Result);
}

FTGOR_InertiaTensor FTGOR_InertiaTensor::operator^(const FTGOR_InertiaTensor& Tensor) const
{
	FTGOR_InertiaTensor Result;
	Result.X.X = X.X * Tensor.X.X + Y.X * Tensor.Y.X + Z.X * Tensor.Z.X;
	Result.X.Y = X.X * Tensor.X.Y + Y.X * Tensor.Y.Y + Z.X * Tensor.Z.Y;
	Result.X.Z = X.X * Tensor.X.Z + Y.X * Tensor.Y.Z + Z.X * Tensor.Z.Z;

	Result.Y.X = X.Y * Tensor.X.X + Y.Y * Tensor.Y.X + Z.Y * Tensor.Z.X;
	Result.Y.Y = X.Y * Tensor.X.Y + Y.Y * Tensor.Y.Y + Z.Y * Tensor.Z.Y;
	Result.Y.Z = X.Y * Tensor.X.Z + Y.Y * Tensor.Y.Z + Z.Y * Tensor.Z.Z;

	Result.Z.X = X.Y * Tensor.X.X + Y.Z * Tensor.Y.X + Z.Z * Tensor.Z.X;
	Result.Z.Y = X.Y * Tensor.X.Y + Y.Z * Tensor.Y.Y + Z.Z * Tensor.Z.Y;
	Result.Z.Z = X.Y * Tensor.X.Z + Y.Z * Tensor.Y.Z + Z.Z * Tensor.Z.Z;
	return(Result);
}

FTGOR_InertiaTensor FTGOR_InertiaTensor::operator&(const FTGOR_InertiaTensor& Tensor) const
{
	FTGOR_InertiaTensor Result;
	Result.X.X = X.X * Tensor.X.X + X.Y * Tensor.X.Y + X.Z * Tensor.X.Z;
	Result.X.Y = X.X * Tensor.Y.X + X.Y * Tensor.Y.Y + X.Z * Tensor.Y.Z;
	Result.X.Z = X.X * Tensor.Z.X + X.Y * Tensor.Z.Y + X.Z * Tensor.Z.Z;

	Result.Y.X = Y.X * Tensor.X.X + Y.Y * Tensor.X.Y + Y.Z * Tensor.X.Z;
	Result.Y.Y = Y.X * Tensor.Y.X + Y.Y * Tensor.Y.Y + Y.Z * Tensor.Y.Z;
	Result.Y.Z = Y.X * Tensor.Z.X + Y.Y * Tensor.Z.Y + Y.Z * Tensor.Z.Z;

	Result.Z.X = Y.X * Tensor.X.X + Z.Y * Tensor.X.Y + Z.Z * Tensor.X.Z;
	Result.Z.Y = Y.X * Tensor.Y.X + Z.Y * Tensor.Y.Y + Z.Z * Tensor.Y.Z;
	Result.Z.Z = Y.X * Tensor.Z.X + Z.Y * Tensor.Z.Y + Z.Z * Tensor.Z.Z;
	return(Result);
}

FTGOR_InertiaTensor FTGOR_InertiaTensor::operator|(const FTGOR_InertiaTensor& Tensor) const
{
	FTGOR_InertiaTensor Result;
	Result.X.X = X.X * Tensor.X.X + Y.X * Tensor.X.Y + Z.X * Tensor.X.Z;
	Result.X.Y = X.X * Tensor.Y.X + Y.X * Tensor.Y.Y + Z.X * Tensor.Y.Z;
	Result.X.Z = X.X * Tensor.Z.X + Y.X * Tensor.Z.Y + Z.X * Tensor.Z.Z;

	Result.Y.X = X.Y * Tensor.X.X + Y.Y * Tensor.X.Y + Z.Y * Tensor.X.Z;
	Result.Y.Y = X.Y * Tensor.Y.X + Y.Y * Tensor.Y.Y + Z.Y * Tensor.Y.Z;
	Result.Y.Z = X.Y * Tensor.Z.X + Y.Y * Tensor.Z.Y + Z.Y * Tensor.Z.Z;

	Result.Z.X = X.Y * Tensor.X.X + Y.Z * Tensor.X.Y + Z.Z * Tensor.X.Z;
	Result.Z.Y = X.Y * Tensor.Y.X + Y.Z * Tensor.Y.Y + Z.Z * Tensor.Y.Z;
	Result.Z.Z = X.Y * Tensor.Z.X + Y.Z * Tensor.Z.Y + Z.Z * Tensor.Z.Z;
	return(Result);
}

FTGOR_InertiaTensor FTGOR_InertiaTensor::operator+(const FTGOR_InertiaTensor& Tensor) const
{
	return(FTGOR_InertiaTensor(X + Tensor.X, Y + Tensor.Y, Z + Tensor.Z));
}

FTGOR_InertiaTensor FTGOR_InertiaTensor::operator+=(const FTGOR_InertiaTensor& Tensor)
{
	X += Tensor.X;
	Y += Tensor.Y;
	Z += Tensor.Z;
	return(*this);
}

bool FTGOR_InertiaTensor::operator==(const FTGOR_InertiaTensor& Tensor) const
{
	return(Tensor.X.Equals(X, 0.001f) && Tensor.Y.Equals(Y, 0.001f) && Tensor.Z.Equals(Z, 0.001f));
}

FTGOR_InertiaTensor FTGOR_InertiaTensor::Inverse() const
{
	const float DetX = Y.Y * Z.Z - Y.Z * Z.Y;
	const float DetY = Z.X * Y.Z - Y.X * Z.Z;
	const float DetZ = Y.X * Z.Y - Z.X * Y.Y;

	const float Det = X.X * DetX + X.Y * DetY + X.Z * DetZ;

	if (FMath::IsNearlyEqual(Det, 0.0f))
	{
		return(FTGOR_InertiaTensor(FVector::OneVector));
	}

	FTGOR_InertiaTensor Tensor;
	Tensor.X.X = DetX / Det;
	Tensor.Y.X = DetY / Det;
	Tensor.Z.X = DetZ / Det;

	Tensor.X.Y = (Z.Y * X.Z - X.Y * Z.Z) / Det;
	Tensor.Y.Y = (X.X * Z.Z - Z.X * X.Z) / Det;
	Tensor.Z.Y = (Z.X * X.Y - X.X * Z.Y) / Det;

	Tensor.X.Z = (X.Y * Y.Z - Y.Y * X.Z) / Det;
	Tensor.Y.Z = (Y.X * X.Z - X.X * Y.Z) / Det;
	Tensor.Z.Z = (X.X * Y.Y - Y.X * X.Y) / Det;

	return(Tensor);
}

FString FTGOR_InertiaTensor::ToString() const
{
	return(X.ToString() + " | " + Y.ToString() + " | " + Z.ToString());
}



// Add default functionality here for any ITGOR_PhysicsInterface functions that are not pure virtual.


FVector ITGOR_PhysicsObjectInterface::ApplyMove(float DeltaTime, const FVector& External, const TArray<AActor*>& Ignore)
{
	return(FVector::ZeroVector);
}


void ITGOR_PhysicsObjectInterface::SetParent(TScriptInterface<ITGOR_PhysicsObjectInterface> ParentInterface)
{

}


FTGOR_InertiaTensor ITGOR_PhysicsObjectInterface::ComputeInertiaTensor()
{
	return(FTGOR_InertiaTensor(FVector::OneVector));
}

void ITGOR_PhysicsObjectInterface::InvalidateInertiaTensor()
{

}


FVector ITGOR_PhysicsObjectInterface::GetOffset() const
{
	return(FVector::ZeroVector);
}

void ITGOR_PhysicsObjectInterface::MoveOffset(const FVector& Delta)
{

}

FVector ITGOR_PhysicsObjectInterface::GetLocation() const
{
	return(FVector::ZeroVector);
}

void ITGOR_PhysicsObjectInterface::Move(const FVector& Delta)
{

}
