// The Gateway of Realities: Planes of Existence.
#include "TGOR_EffectComponent.h"

#include "Realities/Mod/Modifiers/Effects/TGOR_Effect.h"
#include "Realities/Components/System/TGOR_AttributeComponent.h"

#include "Realities/Utility/Storage/TGOR_Package.h"
#include "Realities/Base/TGOR_Singleton.h"

#include "Engine/NetConnection.h"
#include "Net/UnrealNetwork.h"

SERIALISE_INIT_SOURCE_IMPLEMENT(FTGOR_EffectState)
SERIALISE_INIT_SOURCE_IMPLEMENT(FTGOR_EffectStates)

void FTGOR_EffectState::Write(FTGOR_GroupWritePackage& Package, UTGOR_Singleton* Context) const
{
	Package.WriteEntry("Time", Time);
	Package.WriteEntry("Level", Level);
}

void FTGOR_EffectState::Read(FTGOR_GroupReadPackage& Package, UTGOR_Singleton* Context)
{
	Package.ReadEntry("Time", Time);
	Package.ReadEntry("Level", Level);
}

void FTGOR_EffectState::Send(FTGOR_NetworkWritePackage& Package, UTGOR_Singleton* Context) const
{
	Package.WriteEntry(Time);
	Package.WriteEntry(Level);
}

void FTGOR_EffectState::Recv(FTGOR_NetworkReadPackage& Package, UTGOR_Singleton* Context)
{
	Package.ReadEntry(Time);
	Package.ReadEntry(Level);
}


void FTGOR_EffectStates::Write(FTGOR_GroupWritePackage& Package, UTGOR_Singleton* Context) const
{
	Package.WriteEntry("Effects", States);
}

void FTGOR_EffectStates::Read(FTGOR_GroupReadPackage& Package, UTGOR_Singleton* Context)
{
	Package.ReadEntry("Effects", States);
}

void FTGOR_EffectStates::Send(FTGOR_NetworkWritePackage& Package, UTGOR_Singleton* Context) const
{
	Package.WriteEntry(States);
}

void FTGOR_EffectStates::Recv(FTGOR_NetworkReadPackage& Package, UTGOR_Singleton* Context)
{
	Package.ReadEntry(States);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

UTGOR_EffectComponent::UTGOR_EffectComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	SetIsReplicatedByDefault(true);
}

void UTGOR_EffectComponent::BeginPlay()
{
	Super::BeginPlay();

	OnEffectChanged.Broadcast();
}

void UTGOR_EffectComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	SINGLETON_CHK;

	bool HasChanged = false;

	const FTGOR_Time Timestamp = Singleton->GetGameTimestamp();
	for (auto It = EffectStates.States.CreateIterator(); It; ++It)
	{
		if (It->Value.Time < Timestamp)
		{
			HasChanged = true;
			It.RemoveCurrent();
		}
	}

	if (HasChanged)
	{
		UpdateAttributes();
		OnEffectChanged.Broadcast();
	}
}

void UTGOR_EffectComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION(UTGOR_EffectComponent, EffectStates, COND_OwnerOnly);
}

TSet<UTGOR_Content*> UTGOR_EffectComponent::GetActiveContent_Implementation() const
{
	TSet<UTGOR_Content*> Output;
	for (auto Pair : EffectStates.States)
	{
		Output.Add(Pair.Key);
	}
	return Output;
}


void UTGOR_EffectComponent::OnReplicateEffects()
{
	UpdateAttributes();
	OnEffectChanged.Broadcast();
}

void UTGOR_EffectComponent::AddEffect(UTGOR_Effect* Effect, int32 Level, float Duration)
{
	SINGLETON_CHK;
	if (IsValid(Effect) && Level > 0 && Duration >= SMALL_NUMBER)
	{
		const FTGOR_Time Timestamp = Singleton->GetGameTimestamp() + Duration;

		FTGOR_EffectState& State = EffectStates.States.FindOrAdd(Effect);
		if (State.Time < Timestamp)
		{
			State.Time = Timestamp;
			State.Level = FMath::Max(State.Level, Level);
		}

		UpdateAttributes();
		OnEffectChanged.Broadcast();
	}
}

TArray<UTGOR_Effect*> UTGOR_EffectComponent::GetCurrentEffects() const
{
	TArray<UTGOR_Effect*> Output;
	EffectStates.States.GetKeys(Output);
	return Output;
}

bool UTGOR_EffectComponent::IsEffectActive(UTGOR_Effect* Effect) const
{
	return EffectStates.States.Contains(Effect);
}

float UTGOR_EffectComponent::GetEffectDuration(UTGOR_Effect* Effect) const
{
	const FTGOR_EffectState* Ptr = EffectStates.States.Find(Effect);
	if (Ptr && Singleton)
	{
		const FTGOR_Time Timestamp = Singleton->GetGameTimestamp();
		return FMath::Max(Ptr->Time - Timestamp, 0.0f);
	}
	return 0.0f;
}

int32 UTGOR_EffectComponent::GetEffectLevel(UTGOR_Effect* Effect) const
{
	const FTGOR_EffectState* Ptr = EffectStates.States.Find(Effect);
	if (Ptr)
	{
		return Ptr->Level;
	}
	return 0;
}

void UTGOR_EffectComponent::UpdateAttributes()
{
	UTGOR_AttributeComponent* Attributes = GetOwnerComponent<UTGOR_AttributeComponent>();
	if (IsValid(Attributes))
	{
		for (auto Pair : EffectStates.States)
		{
			Attributes->RegisterHandle(this, Pair.Key, (float)Pair.Value.Level);
		}
	}
}
