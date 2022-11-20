// The Gateway of Realities: Planes of Existence.

#include "TGOR_ActivatorComponent.h"
#include "RealitiesUtility/Utility/TGOR_Error.h"
#include "RealitiesUtility/Utility/TGOR_Math.h"
#include "DimensionSystem/Content/TGOR_Spawner.h"

#include "DialogueSystem/Tasks/TGOR_ActivatorTask.h"
#include "DialogueSystem/Content/TGOR_Activator.h"
#include "DialogueSystem/Components/TGOR_DialogueComponent.h"
#include "DialogueSystem/Components/TGOR_InstigatorComponent.h"
#include "DialogueSystem/Interfaces/TGOR_ParticipantInterface.h"

#include "DrawDebugHelpers.h"
#include "Net/UnrealNetwork.h"

UTGOR_ActivatorComponent::UTGOR_ActivatorComponent()
	: Super()
{
	PrimaryComponentTick.bCanEverTick = true;
	SetIsReplicatedByDefault(true);
}

void UTGOR_ActivatorComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UTGOR_ActivatorComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction * ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	UTGOR_ActivatorTask* Activator = GetActivatorTask();
	if (IsValid(Activator))
	{
		Activator->Update(ActivatorTaskState, DeltaTime);
	}
}

void UTGOR_ActivatorComponent::GetLifetimeReplicatedProps(TArray< FLifetimeProperty >&OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	// Replicate to everyone
	DOREPLIFETIME_CONDITION(UTGOR_ActivatorComponent, ActivatorSlots, COND_None); // This MUST be replicated before setup
	DOREPLIFETIME_CONDITION(UTGOR_ActivatorComponent, ActivatorTaskState, COND_None);
}

TSet<UTGOR_CoreContent*> UTGOR_ActivatorComponent::GetActiveContent_Implementation() const
{
	TSet<UTGOR_CoreContent*> ContentContext = ITGOR_RegisterInterface::GetActiveContent_Implementation();

	UTGOR_ActivatorTask* CurrentTask = GetActivatorTask();
	if (IsValid(CurrentTask))
	{
		ContentContext.Emplace(CurrentTask->GetActivator());
	}
	return ContentContext;
}

void UTGOR_ActivatorComponent::UpdateContent_Implementation(FTGOR_SpawnerDependencies& Dependencies)
{
	Super::UpdateContent_Implementation(Dependencies);

	Dependencies.Process<UTGOR_ParticipantInterface>();
	Dependencies.Process<UTGOR_DialogueComponent>();

	UTGOR_ActivatorTask* CurrentTask = GetActivatorTask();
	FTGOR_SpectacleConfiguration PreviousConfiguration = ActivatorTaskState.Configuration;

	TMap<UTGOR_Activator*, TArray<UTGOR_ActivatorTask*>> Previous;

	// Remove slots but cache both instances and items in case the new loadout can use them
	for (int Slot = 0; Slot < ActivatorSlots.Num(); Slot++)
	{
		UTGOR_ActivatorTask* ActivatorSlot = ActivatorSlots[Slot];
		if (IsValid(ActivatorSlot))
		{
			Previous.FindOrAdd(ActivatorSlot->GetActivator()).Add(ActivatorSlot);
		}
	}

	ActivatorSlots.Empty();

	// Get installed Activator modes
	TArray<UTGOR_Activator*> Activators = Dependencies.Spawner->GetMListFromType<UTGOR_Activator>(SpawnActivators);

	// Get all candidates that are part of the Activator queue
	for (UTGOR_Activator* Activator : Activators)
	{
		// Get slot from cache or create one
		UTGOR_ActivatorTask* ActivatorSlot = nullptr;

		TArray<UTGOR_ActivatorTask*>* Ptr = Previous.Find(Activator);
		if (Ptr && Ptr->Num() > 0)
		{
			ActivatorSlot = Ptr->Pop();
		}
		else
		{
			// No cache was found, create a new one
			ActivatorSlot = Activator->CreateActivatorTask(this, ActivatorSlots.Num());
		}

		// Initialise and add to slots
		if (IsValid(ActivatorSlot))
		{
			ActivatorSlots.Add(ActivatorSlot);
		}
	}

	// Schedule with current if available or reset
	const int32 ActiveSlot = ActivatorSlots.Find(CurrentTask);
	if (ActivatorSlots.IsValidIndex(ActiveSlot))
	{
		StartActivatorWith(ActiveSlot, PreviousConfiguration);
	}
	else
	{
		StartActivatorWith(INDEX_NONE, PreviousConfiguration);
	}

	// Discard tasks that got removed
	for (const auto& Pair : Previous)
	{
		for (UTGOR_ActivatorTask* ActivatorSlot : Pair.Value)
		{
			ActivatorSlot->MarkAsGarbage();
		}
	}
}

TMap<int32, UTGOR_SpawnModule*> UTGOR_ActivatorComponent::GetModuleType_Implementation() const
{
	TMap<int32, UTGOR_SpawnModule*> Modules = Super::GetModuleType_Implementation();

	const int32 Num = ActivatorSlots.Num();
	for (UTGOR_ActivatorTask* ActivatorSlot : ActivatorSlots)
	{
		Modules.Emplace(ActivatorSlot->Identifier.Slot, ActivatorSlot->Identifier.Content);
	}
	return Modules;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

bool UTGOR_ActivatorComponent::ValidateSpectactleFor(UTGOR_InstigatorComponent* Instigator, FTGOR_SpectacleConfiguration& Configuration) const
{
	for (UTGOR_ActivatorTask* ActivatorSlot : ActivatorSlots)
	{
		if (Instigator->ValidateTask(ActivatorSlot, Configuration))
		{
			return true;
		}
	}
	return false;
}

void UTGOR_ActivatorComponent::StartSpectacle(UTGOR_InstigatorComponent* Instigator, FTGOR_SpectacleConfiguration& Configuration)
{
	const int32 Num = ActivatorSlots.Num();
	for (int32 Index = 0; Index < Num; Index++)
	{
		if (Instigator->ValidateTask(ActivatorSlots[Index], Configuration))
		{
			StartActivatorWith(Index, Configuration);
		}
	}
}

void UTGOR_ActivatorComponent::EndSpectacle()
{
	StartActivatorWith(INDEX_NONE, FTGOR_SpectacleConfiguration());
}

////////////////////////////////////////////////////////////////////////////////////////////////////

UTGOR_ActivatorTask* UTGOR_ActivatorComponent::GetActivatorTask() const
{
	if (ActivatorSlots.IsValidIndex(ActivatorTaskState.ActiveSlot))
	{
		return ActivatorSlots[ActivatorTaskState.ActiveSlot];
	}
	return nullptr;
}

bool UTGOR_ActivatorComponent::HasActivatorWith(int32 Identifier) const
{
	return ActivatorTaskState.ActiveSlot == Identifier;
}

void UTGOR_ActivatorComponent::StartActivatorWith(int32 Identifier, const FTGOR_SpectacleConfiguration& Configuration)
{
	if (Identifier != ActivatorTaskState.ActiveSlot)
	{
		if (UTGOR_ActivatorTask* Task = GetActivatorTask())
		{
			Task->Interrupt(ActivatorTaskState);
		}

		ActivatorTaskState.ActiveSlot = Identifier;
		ActivatorTaskState.Configuration = Configuration;

		if (UTGOR_ActivatorTask* Task = GetActivatorTask())
		{
			Task->Prepare(ActivatorTaskState);
		}
	}
}

void UTGOR_ActivatorComponent::SetParticipantDomain(UTGOR_Participant* Participant, const FVector& Local)
{
	for (UTGOR_ActivatorTask* ActivatorSlot : ActivatorSlots)
	{
		ActivatorSlot->SetDomain(Participant, Local);
	}
}

UTGOR_ActivatorTask* UTGOR_ActivatorComponent::GetCurrentActivatorOfType(TSubclassOf<UTGOR_ActivatorTask> Type) const
{
	if (ActivatorSlots.IsValidIndex(ActivatorTaskState.ActiveSlot) && ActivatorSlots[ActivatorTaskState.ActiveSlot]->IsA(Type))
	{
		return ActivatorSlots[ActivatorTaskState.ActiveSlot];
	}
	return nullptr;
}

TArray<UTGOR_ActivatorTask*> UTGOR_ActivatorComponent::GetActivatorListOfType(TSubclassOf<UTGOR_ActivatorTask> Type) const
{
	TArray<UTGOR_ActivatorTask*> Activators;
	if (*Type)
	{
		for (UTGOR_ActivatorTask* ActivatorSlot : ActivatorSlots)
		{
			if (ActivatorSlot->IsA(Type))
			{
				Activators.Emplace(ActivatorSlot);
			}
		}
	}
	return Activators;
}

void UTGOR_ActivatorComponent::RepNotifyActivatorTaskState(const FTGOR_SpectacleState& Old)
{
}

////////////////////////////////////////////////////////////////////////////////////////////////////

UTGOR_ActivatorComponent::FDrawActivatorSceneProxy::FDrawActivatorSceneProxy(const UTGOR_ActivatorComponent* InComponent)
	: UTGOR_AimTargetComponent::FDrawAimTargetSceneProxy(InComponent)
	, PreviewSize(InComponent->PreviewSize)
	, LocalX(InComponent->OffsetRotation.RotateVector(FVector::ForwardVector))
	, LocalY(InComponent->OffsetRotation.RotateVector(FVector::RightVector))
{
}

void UTGOR_ActivatorComponent::FDrawActivatorSceneProxy::DrawTargetSpace(const FMatrix& Transform, const FLinearColor& DrawColor, FPrimitiveDrawInterface* PDI) const
{
	// Draw preview
	const FVector ScaledLocalX = Transform.TransformVector(LocalX).GetSafeNormal() * PreviewSize;
	const FVector ScaledLocalY = Transform.TransformVector(LocalY).GetSafeNormal() * PreviewSize;
	DrawRect(Transform, PDI, DrawColor, DrawColor, ScaledLocalX, ScaledLocalY, 0.75f, 0.9f);

	PDI->DrawLine(
		Transform.GetOrigin() + ScaledLocalX,
		Transform.GetOrigin() + ScaledLocalY,
		DrawColor, SDPG_World, 1.f);

	PDI->DrawLine(
		Transform.GetOrigin() + ScaledLocalX,
		Transform.GetOrigin() - ScaledLocalY,
		DrawColor, SDPG_World, 1.f);
}

void UTGOR_ActivatorComponent::FDrawActivatorSceneProxy::DrawRect(const FMatrix& Transform, FPrimitiveDrawInterface* PDI, const FLinearColor& ColorX, const FLinearColor& ColorY, const FVector& ScaledX, const FVector& ScaledY, const  float thickness, float border) const
{
	PDI->DrawLine(
		Transform.GetOrigin() + ScaledX + ScaledY * border,
		Transform.GetOrigin() + ScaledX - ScaledY * border,
		ColorX, SDPG_World, thickness);

	PDI->DrawLine(
		Transform.GetOrigin() + ScaledX * border + ScaledY,
		Transform.GetOrigin() - ScaledX * border + ScaledY,
		ColorY, SDPG_World, thickness);

	PDI->DrawLine(
		Transform.GetOrigin() - ScaledX + ScaledY * border,
		Transform.GetOrigin() - ScaledX - ScaledY * border,
		ColorX, SDPG_World, thickness);

	PDI->DrawLine(
		Transform.GetOrigin() + ScaledX * border - ScaledY,
		Transform.GetOrigin() - ScaledX * border - ScaledY,
		ColorY, SDPG_World, thickness);
}

FPrimitiveSceneProxy* UTGOR_ActivatorComponent::CreateSceneProxy()
{
	return new FDrawActivatorSceneProxy(this);
}

bool UTGOR_ActivatorComponent::AreSymmetricRotations(const FQuat& A, const FQuat& B, const FVector& Scale3D) const
{
	return false;
}
