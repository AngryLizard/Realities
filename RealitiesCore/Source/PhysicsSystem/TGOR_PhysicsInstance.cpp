// The Gateway of Realities: Planes of Existence.


#include "TGOR_PhysicsInstance.h"
#include "RealitiesUGC/Mod/TGOR_ContentManager.h"

#include "CoreSystem/Storage/TGOR_Serialisation.h"
#include "CoreSystem/Storage/TGOR_Package.h"

SERIALISE_INIT_IMPLEMENT(FTGOR_MovementCapture)


////////////////////////////////////////////////////////////////////////////////////////////////////

FTGOR_MovementCapture::FTGOR_MovementCapture()
:	UpVector(FVector::UpVector)
{
}

void FTGOR_MovementCapture::SetLerpCapture(const FTGOR_MovementCapture& Other, float Alpha)
{
	UpVector = UTGOR_Math::Normalize(FMath::Lerp(UpVector, Other.UpVector, Alpha));
}

float FTGOR_MovementCapture::CompareCapture(const FTGOR_MovementCapture& State, float DotThres) const
{
	return (1.0 - (State.UpVector | UpVector)) / DotThres;
}

void FTGOR_MovementCapture::Send(FTGOR_NetworkWritePackage& Package, UTGOR_Singleton* Context) const
{
	Package.WriteEntry(UpVector);
}

void FTGOR_MovementCapture::Recv(FTGOR_NetworkReadPackage& Package, UTGOR_Singleton* Context)
{
	Package.ReadEntry(UpVector);
}

/*
bool FTGOR_MovementCapture::NetSerialize(FArchive& Ar, class UPackageMap* Map, bool& bOutSuccess)
{
	bOutSuccess = true;
	uint8 Mask;

	SERIALISE_MASK_NORM(Orientation);
	SERIALISE_MASK_NORM(UpVector);

	return bOutSuccess;
}
*/

////////////////////////////////////////////////////////////////////////////////////////////////////

FTGOR_MovementTick::FTGOR_MovementTick()
	: Deltatime(0.0f),
	MaxLinearDamping(0.0f),
	MaxAngularDamping(0.0f)
{
}

FTGOR_MovementOutput::FTGOR_MovementOutput()
	: FTGOR_MovementForce(),
	Orientation(FVector::UpVector),
	MaxLinearDamping(0.0f),
	MaxAngularDamping(0.0f)
{
}

/*
void FTGOR_MovementOutput::SetBaseFrom(UActorComponent* Component, const FName& Name)
{
	Index = INDEX_NONE;
	Base = nullptr;
	if (IsValid(Component))
	{
		SetBaseFrom(Component->GetOwner(), Name);
	}
}

void FTGOR_MovementOutput::SetBaseFromVolume(ATGOR_PhysicsVolume* Volume, const FName& Name)
{
	Index = INDEX_NONE;
	Base = nullptr;
	if (IsValid(Volume))
	{
		Base = Volume->GetMovement();
		Index = Base->GetIndexFromName(Name);
	}
}
*/

////////////////////////////////////////////////////////////////////////////////////////////////////
