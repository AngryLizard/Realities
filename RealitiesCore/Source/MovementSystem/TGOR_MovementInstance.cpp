// The Gateway of Realities: Planes of Existence.


#include "TGOR_MovementInstance.h"
#include "RealitiesUGC/Mod/TGOR_ContentManager.h"

#include "AnimationSystem/Content/TGOR_Animation.h"
#include "AnimationSystem/Components/TGOR_AttachComponent.h"
#include "AnimationSystem/Components/TGOR_HandleComponent.h"
#include "PhysicsSystem/Components/TGOR_RigidPawnComponent.h"
#include "MovementSystem/Content/TGOR_Movement.h"

#include "CoreSystem/TGOR_Singleton.h"
#include "CoreSystem/Gameplay/TGOR_GameInstance.h"
#include "CoreSystem/Storage/TGOR_Serialisation.h"
#include "CoreSystem/Storage/TGOR_Package.h"

#include "Components/PrimitiveComponent.h"
#include "RealitiesUtility/Utility/TGOR_Math.h"

//SERIALISE_INIT_SOURCE_IMPLEMENT(FTGOR_MovementBase)
SERIALISE_INIT_SOURCE_IMPLEMENT(FTGOR_MovementState);
SERIALISE_INIT_IMPLEMENT(FTGOR_MovementFrame)
SERIALISE_INIT_IMPLEMENT(FTGOR_MovementInput)
SERIALISE_INIT_SOURCE_IMPLEMENT(FTGOR_MovementContent)


FTGOR_MovementState::FTGOR_MovementState()
	: ActiveSlot(INDEX_NONE)
{
}

void FTGOR_MovementState::Send(FTGOR_NetworkWritePackage& Package, UTGOR_Singleton* Context) const
{
	Package.WriteEntry(ActiveSlot);
}

void FTGOR_MovementState::Recv(FTGOR_NetworkReadPackage& Package, UTGOR_Singleton* Context)
{
	Package.ReadEntry(ActiveSlot);
}


//
//FTGOR_MovementBase::FTGOR_MovementBase()
//: FTGOR_MovementBase(nullptr)
//{
//}
//
//FTGOR_MovementBase::FTGOR_MovementBase(UTGOR_MobilityComponent* Component)
//: FTGOR_MovementDynamic(),
//	Index(),
//	Socket(nullptr),
//	Parent(nullptr),
//	Component(Component)
//{
//}
//
//FTGOR_MovementBase FTGOR_MovementBase::Lerp(const FTGOR_MovementBase& Other, float Alpha) const
//{
//	// If component/parent changed we don't lerp at all
//	if (Component != Other.Component || Parent != Other.Parent)
//	{
//		return Alpha < 0.5f ? (*this) : Other;
//	}
//
//	// Lerp dynamic part
//	FTGOR_MovementBase Out = *this;
//	Out.FTGOR_MovementDynamic::operator=(FTGOR_MovementDynamic::Lerp(Other, Alpha));
//	return Out;
//}
//
//float FTGOR_MovementBase::Compare(const FTGOR_MovementBase& Base, const FTGOR_MovementThreshold& Threshold) const
//{
//	if (Parent.Get() != Base.Parent.Get())
//	{
//		return 1.0f;
//	}
//
//	if (Index != Base.Index)
//	{
//		return 1.0f;
//	}
//
//	if (Socket != Base.Socket)
//	{
//		return 1.0f;
//	}
//
//	const float LinearSpeed = Base.LinearVelocity.Size();
//	const float AngularSpeed = Base.AngularVelocity.Size();
//	const float Cost = FTGOR_MovementPosition::Compare(Base, Threshold(LinearSpeed, AngularSpeed));
//	if (Cost >= 1.0f)
//	{
//		return 1.0f;
//	}
//
//	const float Distance = (LinearVelocity - Base.LinearVelocity).SizeSquared() / (Threshold.LinearVelocity * Threshold.LinearVelocity);
//	const float Angle = (AngularVelocity - Base.AngularVelocity).SizeSquared() / (Threshold.AngularVelocity * Threshold.AngularVelocity);
//	return Cost + Distance + Angle;
//}
//
//bool FTGOR_MovementBase::HasCycle() const
//{
//	return Parent.IsValid() && Parent->HasParent(Component.Get());
//}
//
//void FTGOR_MovementBase::Adjust(const FTGOR_MovementBase& Base)
//{
//	Index = Base.Index;
//	Socket = Base.Socket;
//	Parent = Base.Parent;
//	Linear = Base.Linear;
//	Angular = Base.Angular;
//	LinearVelocity = Base.LinearVelocity;
//	AngularVelocity = Base.AngularVelocity;
//}
//
//
//FTGOR_MovementPosition FTGOR_MovementBase::ComputePosition() const
//{
//	return BaseToPosition(ComputeParentPosition());
//}
//
//FTGOR_MovementPosition FTGOR_MovementBase::ComputeParentPosition() const
//{
//	if (Parent.IsValid())
//	{
//		const FTGOR_MovementPosition Position = Parent->ComputePosition();
//		if (Index || IsValid(Socket))
//		{
//			const FTGOR_MovementPosition Local = Parent->GetIndexTransform(Socket, Index.Index);
//			return Local.BaseToPosition(Position);
//		}
//		return Position;
//	}
//	return FTGOR_MovementSpace();
//}
//
//////////////////////////////////////////////////////////////////////////////////////////////////////
//
//FTGOR_MovementSpace FTGOR_MovementBase::ComputeSpace() const
//{
//	return BaseToSpace(ComputeParentSpace());
//}
//
//FTGOR_MovementSpace FTGOR_MovementBase::ComputeParentSpace() const
//{
//	if (Parent.IsValid())
//	{
//		FTGOR_MovementSpace Space = Parent->ComputeSpace();
//		if (Index || IsValid(Socket))
//		{
//			const FTGOR_MovementDynamic Local = Parent->GetIndexTransform(Socket, Index.Index);
//			return Local.BaseToSpace(Space);
//		}
//
//		return Space;
//	}
//	return FTGOR_MovementSpace();
//}
//
//
//
//void FTGOR_MovementBase::Send(FTGOR_NetworkWritePackage& Package, UTGOR_Singleton* Context) const
//{
//	Package.WriteEntry(Index);
//	Package.WriteEntry(Socket);
//	Package.WriteEntry(Parent);
//	Package.WriteEntry(Linear);
//	Package.WriteEntry(LinearVelocity);
//	Package.WriteEntry(Angular);
//	Package.WriteEntry(AngularVelocity);
//}
//
//void FTGOR_MovementBase::Recv(FTGOR_NetworkReadPackage& Package, UTGOR_Singleton* Context)
//{
//	Package.ReadEntry(Index);
//	Package.ReadEntry(Socket);
//	Package.ReadEntry(Parent);
//	Package.ReadEntry(Linear);
//	Package.ReadEntry(LinearVelocity);
//	Package.ReadEntry(Angular);
//	Package.ReadEntry(AngularVelocity);
//}
// 

////////////////////////////////////////////////////////////////////////////////////////////////////

FTGOR_MovementGround::FTGOR_MovementGround()
:	SurfaceNormal(FVector::UpVector),
	SurfaceOffset(FVector::ZeroVector),
	Slope(0.0f),
	GroundRatio(0.0f)
{
}

////////////////////////////////////////////////////////////////////////////////////////////////////

FTGOR_MovementContact::FTGOR_MovementContact()
:	FrameNormal(FVector::UpVector), 
	FrameRight(FVector::RightVector),
	FrameForward(FVector::ForwardVector),
	FrameOrtho(0.0f),
	FrameInput(FVector::ZeroVector),
	FrameVelocity(FVector::ZeroVector),
	FrameAngular(FVector::ZeroVector),
	VerticalSpeed(0.0f)
{
}

////////////////////////////////////////////////////////////////////////////////////////////////////

FTGOR_MovementRepel::FTGOR_MovementRepel()
:	RepelNormal(FVector::ZeroVector), 
	RepelForce(FVector::ZeroVector), 
	RepelHit(false)
{
}

FVector FTGOR_MovementRepel::Project(const FVector& Direction, float Threshold) const
{
	if (RepelHit)
	{
		const float Dot = Direction | RepelNormal;
		if (Dot < 0.0f)
		{
			const FVector Vector = (Direction - Dot * RepelNormal);
			const float Size = Vector.Size();
			if (Size >= Threshold)
			{
				return(Vector / Size);
			}
		}
	}
	return(Direction);
}


////////////////////////////////////////////////////////////////////////////////////////////////////

FTGOR_MovementFrame::FTGOR_MovementFrame()
:	Agility(1.0f),
	Strength(1.0f)
{
}

void FTGOR_MovementFrame::SetLerpFrame(const FTGOR_MovementFrame& Other, float Alpha)
{
	Agility = FMath::Lerp(Agility, Other.Agility, Alpha);
	Strength = FMath::Lerp(Strength, Other.Strength, Alpha);
}

void FTGOR_MovementFrame::Send(FTGOR_NetworkWritePackage& Package, UTGOR_Singleton* Context) const
{
	Package.WriteEntry(Agility);
	Package.WriteEntry(Strength);
}

void FTGOR_MovementFrame::Recv(FTGOR_NetworkReadPackage& Package, UTGOR_Singleton* Context)
{
	Package.ReadEntry(Agility);
	Package.ReadEntry(Strength);
}

/*
bool FTGOR_MovementFrame::NetSerialize(FArchive& Ar, class UPackageMap* Map, bool& bOutSuccess)
{
	bOutSuccess = true;
	uint8 Mask;
	
	Ar << Mass;
	Ar << Radius;
	Ar << Height;
	Ar << Volume;

	SERIALISE_MASK_VECT(Faces);
	SERIALISE_MASK_VECT(Inertia);

	Ar << Autonomy;

	return bOutSuccess;
}
*/

////////////////////////////////////////////////////////////////////////////////////////////////////

FTGOR_MovementInput::FTGOR_MovementInput()
:	Input(FVector::ZeroVector),
	View(FVector::ZeroVector)
{
}

void FTGOR_MovementInput::SetLerpInput(const FTGOR_MovementInput& Other, float Alpha)
{
	/*
	// Pseudo-polar lerp is more accurate to input behaviour (?)
	const float InputSize = Input.Size();
	const float OtherSize = Other.Input.Size();
	const float OutSize = FMath::Lerp(InputSize, OtherSize, Alpha);
	const FVector InputDirection = UTGOR_Math::Normalize(FMath::Lerp(Input, Other.Input, Alpha));
	Out.Input = InputDirection * OutSize;
	*/

	Input = FMath::Lerp(Input, Other.Input, Alpha);
	View = UTGOR_Math::Normalize(FMath::Lerp(View, Other.View, Alpha));
}

float FTGOR_MovementInput::CompareInput(const FTGOR_MovementInput& State, double InputThres) const
{
	return ((Input - State.Input).SizeSquared() / (InputThres*InputThres));
}

void FTGOR_MovementInput::Send(FTGOR_NetworkWritePackage& Package, UTGOR_Singleton* Context) const
{
	Package.WriteEntry(Input);
	Package.WriteEntry(View);
}

void FTGOR_MovementInput::Recv(FTGOR_NetworkReadPackage& Package, UTGOR_Singleton* Context)
{
	Package.ReadEntry(Input);
	Package.ReadEntry(View);
}

/*
bool FTGOR_MovementInput::NetSerialize(FArchive& Ar, class UPackageMap* Map, bool& bOutSuccess)
{
	bOutSuccess = true;
	uint8 Mask;

	SERIALISE_MASK_NORM(Input);
	SERIALISE_MASK_NORM(View);

	return bOutSuccess;
}
*/

////////////////////////////////////////////////////////////////////////////////////////////////////

FTGOR_MovementContent::FTGOR_MovementContent()
:	Movement(nullptr)
{
}

void FTGOR_MovementContent::SetLerpContent(const FTGOR_MovementContent& Other, float Alpha)
{
	Movement = Alpha < 0.5f ? Movement : Other.Movement;
}

float FTGOR_MovementContent::CompareContent(const FTGOR_MovementContent& State) const
{
	if (Movement != State.Movement)
	{
		return 1.0f;
	}
	return 0.0f;
}

void FTGOR_MovementContent::Send(FTGOR_NetworkWritePackage& Package, UTGOR_Singleton* Context) const
{
	Package.WriteEntry(Movement);
}

void FTGOR_MovementContent::Recv(FTGOR_NetworkReadPackage& Package, UTGOR_Singleton* Context)
{
	Package.ReadEntry(Movement);
}

/*
bool FTGOR_MovementContent::NetSerialize(FArchive& Ar, class UPackageMap* Map, bool& bOutSuccess)
{
	bOutSuccess = true;
	uint8 Mask;
	SERIALISE_INIT_SOURCE;
	
	SERIALISE_MASK_SOURCE(Movement, Movement);
	// Not necessary to replicate
	//SERIALISE_MASK_SOURCE(Animation, Animation);

	return bOutSuccess;
}
*/

////////////////////////////////////////////////////////////////////////////////////////////////////

FTGOR_MovementObstacle::FTGOR_MovementObstacle()
	: Extent(FVector::ZeroVector)
{
}