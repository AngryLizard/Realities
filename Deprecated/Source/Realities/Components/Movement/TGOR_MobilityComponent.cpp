// The Gateway of Realities: Planes of Existence.

#include "TGOR_MobilityComponent.h"
#include "Components/PrimitiveComponent.h"
#include "GameFramework/Actor.h"

#include "Realities/Mod/Targets/Sockets/TGOR_Socket.h"

#include "Net/UnrealNetwork.h"
#include "DrawDebugHelpers.h"

UTGOR_MobilityComponent::UTGOR_MobilityComponent()
:	Super()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UTGOR_MobilityComponent::BeginPlay()
{
	Super::BeginPlay();

	// TODO: This is set via replication, maybe there is a better way to initialise default values like this
	if (Body.Volume < SMALL_NUMBER)
	{
		FVector Origin, Extend;
		AActor* Actor = GetOwner();
		Actor->GetActorBounds(true, Origin, Extend);
		Body.SetFromBox(FVector(0.5f), Extend * 2, 1.0f);
		ResetToComponent();
	}
}

void UTGOR_MobilityComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	/*
	// Poll for inactive handles to be removed
	// TODO: For now we assume there is only one entry
	for (auto It = Handles.CreateIterator(); It; ++It)
	{
		const TSet<UTGOR_Content*> ContentContext = It->Value.Register->Execute_GetActiveContent(It->Value.Register.GetObject());
		if (ContentContext.Contains(It->Key) && It->Value.Parent->IsValidIndex(It->Value.Index))
		{
			AttachTo(It->Value.Parent.Get(), It->Value.Index);
			break;
		}
		else
		{
			It.RemoveCurrent();
		}
	}
	*/

	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void UTGOR_MobilityComponent::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	// Replicate to simulated
	DOREPLIFETIME_CONDITION(UTGOR_MobilityComponent, Base, COND_SimulatedOnly);
	DOREPLIFETIME_CONDITION(UTGOR_MobilityComponent, Body, COND_None);
}

TSet<UTGOR_Content*> UTGOR_MobilityComponent::GetActiveContent_Implementation() const
{
	TSet<UTGOR_Content*> Set;
	if (IsValid(Base.Socket))
	{
		Set.Add(Base.Socket);
	}
	return Set;
}

void UTGOR_MobilityComponent::RepNotifyBase(const FTGOR_MovementBase& Old)
{
	// Make sure base assignment is valid
	FTGOR_MovementBase New = Base;
	Base = Old;

	SetBase(New);

	// Smoothly lerp
	if (!Old.HasCycle())
	{
		MovementAdjust(Old, Base);
	}

	//DrawDebugPoint(GetWorld(), Base.Location, 10.0f, FColor::Red, false, -1.0f, 0);
	//VPORT(Base.Location);
}

void UTGOR_MobilityComponent::RepNotifyBody(const FTGOR_MovementBody& Old)
{

}

void UTGOR_MobilityComponent::MovementAdjust(const FTGOR_MovementBase& Old, const FTGOR_MovementBase& New)
{
}

USceneComponent* UTGOR_MobilityComponent::GetMovementBasis() const
{
	return Base.Component.Get();
}

const FTGOR_MovementBody& UTGOR_MobilityComponent::GetBody() const
{
	return Body;
}

const float UTGOR_MobilityComponent::GetScale() const
{
	return GetComponentScale().GetMin();
}

void UTGOR_MobilityComponent::AttachToMovement(USceneComponent* Component, UTGOR_Socket* Socket, int32 Index)
{
	USceneComponent* Basis = GetMovementBasis();
	check(!Basis->IsAttachedTo(Component) && "Attachment loop detected");
	Component->AttachToComponent(Basis, FAttachmentTransformRules::KeepWorldTransform, FName());
}


void UTGOR_MobilityComponent::DetachFromMovement(USceneComponent* Component)
{
	Component->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);
}

////////////////////////////////////////////////////////////////////////////////////////////////////


void UTGOR_MobilityComponent::SetBody(const FTGOR_MovementBody& NewBody)
{
	Body = NewBody;
}

UTGOR_Socket* UTGOR_MobilityComponent::GetSocket(TSubclassOf<UTGOR_Socket> Type) const
{
	for (UTGOR_Socket* Socket : Sockets)
	{
		if (Socket->IsA(Type))
		{
			return Socket;
		}
	}
	return nullptr;
}

TArray<UTGOR_Socket*> UTGOR_MobilityComponent::GetSocketList(TSubclassOf<UTGOR_Socket> Type) const
{
	TArray<UTGOR_Socket*> Output;
	for (UTGOR_Socket* Socket : Sockets)
	{
		if (Socket->IsA(Type))
		{
			Output.Emplace(Socket);
		}
	}
	return Output;
}

bool UTGOR_MobilityComponent::IsSocketOccupied(UTGOR_Socket* Socket) const
{
	if (!IsValid(Socket)) return true;
	return false;
}

bool UTGOR_MobilityComponent::QuerySocketObjects(UTGOR_PilotComponent* Component, FTGOR_MovementVisuals& Visuals) const
{
	return false;
}

TArray<UTGOR_PilotComponent*> UTGOR_MobilityComponent::GetSocketObjects(TSubclassOf<UTGOR_Socket> Type) const
{
	return TArray<UTGOR_PilotComponent*>();
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void UTGOR_MobilityComponent::InflictPointImpact(const FVector& Point, const FVector& Impulse, int32 Index)
{
}

void UTGOR_MobilityComponent::InflictPointForce(const FVector& Point, const FVector& Force, float DeltaTime, int32 Index)
{
}

float UTGOR_MobilityComponent::ComputeInertial(const FVector& Point, const FVector& Normal) const
{
	return 0.0f;
}

bool UTGOR_MobilityComponent::UpdatesBase() const
{
	return !GetIsReplicated() || GetOwnerRole() == ENetRole::ROLE_Authority;
}


void UTGOR_MobilityComponent::AttachTo(UTGOR_MobilityComponent* Parent, UTGOR_Socket* Socket, int32 Index)
{
	if (GetOwnerRole() == ENetRole::ROLE_Authority)
	{
		// Update parenting
		if (Parent)
		{
			if (Base.Parent != Parent || Base.Index.Index != Index || Base.Socket != Socket)
			{
				FTGOR_MovementBase NewBase;
				NewBase.Parent = Parent;
				NewBase.Index = Index;
				NewBase.Socket = Socket;
				NewBase.Angular = FQuat::Identity;
				NewBase.Linear = FVector::ZeroVector;
				SetBase(NewBase);
			}
		}
		else if (IsValid(Base.Socket))
		{
			Base.Socket = nullptr;
			SetParent(nullptr, nullptr, INDEX_NONE);
		}
	}
}

void UTGOR_MobilityComponent::DetachFrom(UTGOR_MobilityComponent* Parent, UTGOR_Socket* Socket, int32 Index)
{
	if (GetOwnerRole() == ENetRole::ROLE_Authority)
	{
		if (Base.Parent == Parent && Base.Index.Index == Index && Base.Socket == Socket)
		{
			Base.Socket = nullptr;
			SetParent(nullptr, nullptr, INDEX_NONE);
		}
	}
}

bool UTGOR_MobilityComponent::CanAttachTo(const UTGOR_MobilityComponent* Parent) const
{
	return !IsValid(Parent) || (!Parent->HasParent(this));
}

bool UTGOR_MobilityComponent::IsAttached() const
{
	return IsValid(Base.Socket);
}

bool UTGOR_MobilityComponent::IsAttachedTo(const UTGOR_MobilityComponent* Parent, UTGOR_Socket* Socket, int32 Index) const
{
	return IsAttached() && Base.Parent == Parent && Base.Socket == Socket && Base.Index.Index == Index;

}

////////////////////////////////////////////////////////////////////////////////////////////////////

FTGOR_MovementPosition UTGOR_MobilityComponent::GetComponentPosition() const
{
	FTGOR_MovementPosition Position;
	USceneComponent* Component = GetMovementBasis();
	Position.Linear = Component->GetComponentLocation();
	Position.Angular = Component->GetComponentQuat();
	return Position;
}

void UTGOR_MobilityComponent::SetComponentPosition(const FTGOR_MovementPosition& Position)
{
	USceneComponent* Component = GetMovementBasis();
	Component->SetWorldLocationAndRotation(Position.Linear, Position.Angular, false, nullptr, ETeleportType::TeleportPhysics);
	OnPositionChange(Position);
}

void UTGOR_MobilityComponent::OnPositionChange(const FTGOR_MovementPosition& Position)
{

}

void UTGOR_MobilityComponent::OnReparent(const FTGOR_MovementDynamic& Previous)
{

}

////////////////////////////////////////////////////////////////////////////////////////////////////


void UTGOR_MobilityComponent::ResetToComponent()
{
	if (!Base.Component.IsValid())
	{
		Base.Component = this;
	}

	const FTGOR_MovementPosition ParentPosition = Base.ComputeParentPosition();
	Base.PositionToBase(ParentPosition, GetComponentPosition());
}

int32 UTGOR_MobilityComponent::GetIndexFromName(const FName& Name) const
{
	return INDEX_NONE;
}

FName UTGOR_MobilityComponent::GetNameFromIndex(int32 Index) const
{
	return FName();
}

FTGOR_MovementDynamic UTGOR_MobilityComponent::GetIndexTransform(UTGOR_Socket* Socket, int32 Index) const
{
	return FTGOR_MovementDynamic();
}

const FTGOR_MovementSpace UTGOR_MobilityComponent::ComputeSpace() const
{
	check(!Base.HasCycle() && "Parent loop detected!");

	return Base.ComputeSpace();
}

const FTGOR_MovementPosition UTGOR_MobilityComponent::ComputePosition() const
{
	check(!Base.HasCycle() && "Parent loop detected!");

	return Base.ComputePosition();
}


const FTGOR_MovementBase& UTGOR_MobilityComponent::GetBase() const
{
	return Base;
}

bool UTGOR_MobilityComponent::SetBase(const FTGOR_MovementBase& NewBase)
{
	bool Success = true;

	// Ensure owner
	Base.Component = this;
	Base.Socket = NewBase.Socket;

	// Set parent if changed
	Success = SetParent(NewBase.Parent.Get(), NewBase.Socket, NewBase.Index.Index);
	if (!Success)
	{
		Base.Socket = nullptr;
	}

	// Set movement properties
	Base.FTGOR_MovementDynamic::operator=(NewBase);

	// Move components
	const FTGOR_MovementPosition Position = ComputePosition();
	SetComponentPosition(Position);

	return Success;
}

void UTGOR_MobilityComponent::SimulateDynamic(const FTGOR_MovementDynamic& Dynamic, UTGOR_Socket* Socket, int32 Index)
{
	// Only simulate if not attached
	if (!IsValid(Base.Socket))
	{
		FTGOR_MovementDynamic ParentDynamic = Base.ComputeParentSpace();

		if (Index != INDEX_NONE)
		{
			const FTGOR_MovementDynamic Local = GetIndexTransform(Socket, Index);
			FTGOR_MovementDynamic Relative;
			Relative.DynamicToBase(Local, Dynamic);
			Base.DynamicToBase(ParentDynamic, Relative);
			SetComponentPosition(Relative);
		}
		else
		{
			Base.DynamicToBase(ParentDynamic, Dynamic);
			SetComponentPosition(Dynamic);
		}
	}
	else
	{
		// Refresh base otherwise
		SetBase(Base);
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////

UTGOR_MobilityComponent* UTGOR_MobilityComponent::GetParent() const
{
	return Base.Parent.Get();
}

bool UTGOR_MobilityComponent::SetParent(UTGOR_MobilityComponent* Parent, UTGOR_Socket* Socket, int32 Index)
{
	if (Base.Parent != Parent || Base.Socket != Socket || Base.Index.Index != Index)
	{
		USceneComponent* Component = GetMovementBasis();
		if (Base.Parent.IsValid())
		{
			DetachFromMovement(Component);
		}
		
		// Remember current dynamic in world space to reset after parent swap

		const FTGOR_MovementSpace ParentSpace = Base.ComputeParentSpace();
		const FTGOR_MovementDynamic Dynamic = Base.BaseToSpace(ParentSpace);
		if (CanAttachTo(Parent))
		{
			Base.Parent = Parent;
			Base.Socket = Socket;
			Base.Index = Index;

			if (IsValid(Parent))
			{
				Parent->AttachToMovement(Component, Socket, Index);
			}
		}
		else
		{
			Base.Parent = nullptr;
			Base.Index = FTGOR_Index();
			return false;
		}
		SimulateDynamic(Dynamic);
		OnReparent(ParentSpace);
	}
	return true;
}

bool UTGOR_MobilityComponent::HasParent(const UTGOR_MobilityComponent* Component) const
{
	if (Base.Parent == Component || Component == this)
	{
		return true;
	}
	return Base.Parent.IsValid() ? Base.Parent->HasParent(Component) : false;
}


FTGOR_MovementPosition UTGOR_MobilityComponent::Conv_DynamicToPosition(const FTGOR_MovementDynamic& Dynamic)
{
	return Dynamic;
}

FTGOR_MovementDynamic UTGOR_MobilityComponent::Conv_SpaceToDynamic(const FTGOR_MovementSpace& Space)
{
	return Space;
}

FTGOR_MovementBody UTGOR_MobilityComponent::Conv_BodyFromBox(UBoxComponent* Box, const FVector& SurfaceSparsity, float Density)
{
	FTGOR_MovementBody Body;
	Body.SetFromBox(SurfaceSparsity, Box->GetScaledBoxExtent() * 2, Density);
	return Body;
}

FTGOR_MovementBody UTGOR_MobilityComponent::Conv_BodyFromSphere(USphereComponent* Sphere, const FVector& SurfaceSparsity, float Density)
{
	FTGOR_MovementBody Body;
	Body.SetFromSphere(SurfaceSparsity, Sphere->GetScaledSphereRadius(), Density);
	return Body;
}

FTGOR_MovementBody UTGOR_MobilityComponent::Conv_BodyFromCapsule(UCapsuleComponent* Capsule, const FVector& SurfaceSparsity, float Density)
{
	FTGOR_MovementBody Body;
	Body.SetFromCapsule(SurfaceSparsity, Capsule->GetScaledCapsuleRadius(), Capsule->GetScaledCapsuleHalfHeight_WithoutHemisphere(), Density);
	return Body;
}