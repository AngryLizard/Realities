// The Gateway of Realities: Planes of Existence.

#include "TGOR_MobilityComponent.h"
#include "TGOR_PilotComponent.h"
#include "GameFramework/Actor.h"

#include "DimensionSystem/Content/TGOR_Spawner.h"
#include "DimensionSystem/Tasks/TGOR_LinearPilotTask.h"

#include "CoreSystem/TGOR_Singleton.h"

#include "Net/UnrealNetwork.h"
#include "DrawDebugHelpers.h"

UTGOR_MobilityComponent::UTGOR_MobilityComponent()
:	Super()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UTGOR_MobilityComponent::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION(UTGOR_MobilityComponent, Body, COND_None);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void UTGOR_MobilityComponent::RepNotifyBody(const FTGOR_MovementBody& Old)
{
	SetBody(Body);
}

void UTGOR_MobilityComponent::SetBody(const FTGOR_MovementBody& NewBody)
{
	Body = NewBody;

	SetCapsuleHalfHeight(Body.Height * 0.5f);
	SetCapsuleRadius(Body.Radius);
}

const FTGOR_MovementBody& UTGOR_MobilityComponent::GetBody() const
{
	return Body;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

const float UTGOR_MobilityComponent::GetScale() const
{
	return GetComponentScale().GetMin();
}

void UTGOR_MobilityComponent::AttachToMovement(USceneComponent* Component, int32 Index)
{
	check(!IsAttachedTo(Component) && "Attachment loop detected");
	Component->AttachToComponent(this, FAttachmentTransformRules::KeepWorldTransform, FName());
}


void UTGOR_MobilityComponent::DetachFromMovement(USceneComponent* Component)
{
	Component->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void UTGOR_MobilityComponent::RegisterAttach(UTGOR_LinearPilotTask* Attachee)
{
	LinearAttachments.Add(Attachee);
}

void UTGOR_MobilityComponent::RegisterDetach(UTGOR_LinearPilotTask* Attachee)
{
	LinearAttachments.Remove(Attachee);
}

bool UTGOR_MobilityComponent::UpdatesBase() const
{
	return !GetIsReplicated() || GetOwnerRole() == ENetRole::ROLE_Authority;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

FTGOR_MovementPosition UTGOR_MobilityComponent::GetComponentPosition() const
{
	FTGOR_MovementPosition Position;
	Position.Linear = GetComponentLocation();
	Position.Angular = GetComponentQuat();
	return Position;
}

void UTGOR_MobilityComponent::OnPositionChange(const FTGOR_MovementPosition& Position)
{
	SetWorldLocationAndRotation(Position.Linear, Position.Angular, false, nullptr, ETeleportType::TeleportPhysics);
}

bool UTGOR_MobilityComponent::HasParent(const UTGOR_MobilityComponent* Component) const
{
	if (Component == this)
	{
		return true;
	}
	return false;
}

bool UTGOR_MobilityComponent::CanParent(const UTGOR_MobilityComponent* Component) const
{
	return !IsValid(Component) || (!Component->HasParent(this));
}

////////////////////////////////////////////////////////////////////////////////////////////////////

bool UTGOR_MobilityComponent::ParentLinear(UTGOR_PilotComponent* Attachee, int32 Index, const FTGOR_MovementDynamic& Dynamic)
{
	if (IsValid(Attachee))
	{
		TArray<UTGOR_LinearPilotTask*> Tasks = Attachee->GetPListOfType<UTGOR_LinearPilotTask>();
		for (UTGOR_LinearPilotTask* Task : Tasks)
		{
			if (IsValid(Task) && Task->CanParent(this, Index))
			{
				Task->Parent(this, Index);

				// Set location without triggering any movement events (prevent infinite loops when parent volume changes)
				Task->InitDynamic(Dynamic);

				Attachee->AttachWith(Task->Identifier.Slot);

				// Trigger movement events now
				Attachee->OnPositionChange(Dynamic);
				return true;
			}
		}
	}
	return false;
}

bool UTGOR_MobilityComponent::CanParentLinear(UTGOR_PilotComponent* Attachee, int32 Index) const
{
	if (IsValid(Attachee))
	{
		TArray<UTGOR_LinearPilotTask*> Tasks = Attachee->GetPListOfType<UTGOR_LinearPilotTask>();
		for (UTGOR_LinearPilotTask* Task : Tasks)
		{
			if (IsValid(Task) && Task->CanParent(this, Index))
			{
				return true;
			}
		}
	}
	return false;
}

////////////////////////////////////////////////////////////////////////////////////////////////////


int32 UTGOR_MobilityComponent::GetIndexFromName(const FName& Name) const
{
	return INDEX_NONE;
}

FName UTGOR_MobilityComponent::GetNameFromIndex(int32 Index) const
{
	return FName();
}

FTGOR_MovementDynamic UTGOR_MobilityComponent::GetIndexTransform(int32 Index) const
{
	return FTGOR_MovementDynamic();
}

bool UTGOR_MobilityComponent::IsValidIndex(int32 Index) const
{
	// By default we have no index, so only -1 is permissable
	return Index == INDEX_NONE;
}


const FTGOR_MovementSpace UTGOR_MobilityComponent::ComputeSpace() const
{
	return FTGOR_MovementSpace(GetComponentPosition());
}

const FTGOR_MovementPosition UTGOR_MobilityComponent::ComputePosition() const
{
	return GetComponentPosition();
}

////////////////////////////////////////////////////////////////////////////////////////////////////


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