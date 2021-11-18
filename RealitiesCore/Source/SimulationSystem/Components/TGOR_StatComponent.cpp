///////////////////////////////////////////////////
// The Gateway of Realities: Planes of Existence //
// Copyright 2016-2017                           //
///////////////////////////////////////////////////


#include "TGOR_StatComponent.h"
#include "Net/UnrealNetwork.h"

#include "RealitiesUGC/Mod/TGOR_ContentManager.h"

#include "AttributeSystem/Components/TGOR_AttributeComponent.h"
#include "AttributeSystem/Content/TGOR_Attribute.h"
#include "SimulationSystem/Content/TGOR_Stat.h"
#include "SimulationSystem/Content/TGOR_Effect.h"
#include "SimulationSystem/Content/TGOR_Simulation.h"
#include "SimulationSystem/Content/TGOR_Response.h"
#include "InventorySystem/Content/TGOR_Matter.h"
#include "DimensionSystem/Content/TGOR_Spawner.h"

#include "CoreSystem/Gameplay/TGOR_GameInstance.h"
#include "CoreSystem/TGOR_Singleton.h"

#include "CoreSystem/Storage/TGOR_Package.h"

SERIALISE_INIT_SOURCE_IMPLEMENT(FTGOR_StatState)

void FTGOR_StatState::Write(FTGOR_GroupWritePackage& Package, UTGOR_Singleton* Context) const
{
	Package.WriteEntry("Stats", Stats);
}

void FTGOR_StatState::Read(FTGOR_GroupReadPackage& Package, UTGOR_Singleton* Context)
{
	Package.ReadEntry("Stats", Stats);
}

void FTGOR_StatState::Send(FTGOR_NetworkWritePackage& Package, UTGOR_Singleton* Context) const
{
	Package.WriteEntry(Stats);
}

void FTGOR_StatState::Recv(FTGOR_NetworkReadPackage& Package, UTGOR_Singleton* Context)
{
	Package.ReadEntry(Stats);
}


UTGOR_StatComponent::UTGOR_StatComponent()
	: Super()
{
	PrimaryComponentTick.bCanEverTick = true;
	SetIsReplicatedByDefault(true);
	
}

void UTGOR_StatComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UTGOR_StatComponent::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION(UTGOR_StatComponent, State, COND_None);
}

void UTGOR_StatComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (GetOwnerRole() >= ENetRole::ROLE_AutonomousProxy)
	{
		// Regenerate/Degenerate stat values after a given duration
		SINGLETON_CHK;
		const FTGOR_Time Timestamp = Singleton->GetGameTimestamp();

		bool HasUpdate = false;

		// Deal with each stat individually
		for (auto& Pair : StatTable)
		{
			while (Timestamp > Pair.Value.LastUpdate + Pair.Key->Duration)
			{
				Pair.Value.LastUpdate += Pair.Key->Duration;

				FTGOR_Percentage* Ptr = State.Stats.Find(Pair.Key);
				if (Ptr)
				{
					// Compute with stat tick
					float& Buffer = StatBuffer.FindOrAdd(Pair.Key, 0.0f);
					Buffer += Pair.Key->Compute(this, Ptr->Value + Buffer, Pair.Key->Duration);
					if (FMath::Square(Buffer) > SMALL_NUMBER)
					{
						HasUpdate = true;
					}
				}
			}
		}

		if (HasUpdate)
		{
			UpdateStatValues();
		}
	}
}


bool UTGOR_StatComponent::IsContentActive_Implementation(UTGOR_CoreContent* Content) const
{
	return ActiveEffects.Contains(Content);
}

TSet<UTGOR_CoreContent*> UTGOR_StatComponent::GetActiveContent_Implementation() const
{
	TSet<UTGOR_CoreContent*> Objects;
	for (UTGOR_CoreContent* ActiveEffect : ActiveEffects)
	{
		Objects.Emplace(ActiveEffect);
	}
	return Objects;
}

void UTGOR_StatComponent::UpdateContent_Implementation(FTGOR_SpawnerDependencies& Dependencies)
{
	FTGOR_StatState Old = State;
	State.Stats.Empty();
	StatTable.Empty();

	SINGLETON_CHK;
	const FTGOR_Time Timestamp = Singleton->GetGameTimestamp();

	TArray<UTGOR_Stat*> StatsQueue = Dependencies.Spawner->GetMListFromType<UTGOR_Stat>(SpawnStats);
	TArray<UTGOR_Effect*> EffectsQueue = Dependencies.Spawner->GetMListFromType<UTGOR_Effect>(SpawnEffects);
	for (UTGOR_Stat* Stat : StatsQueue)
	{
		// Add with previous value or max if not existing yet
		FTGOR_Percentage* Ptr = Old.Stats.Find(Stat);
		if (Ptr)
		{
			State.Stats.Emplace(Stat, *Ptr);
		}
		else
		{
			State.Stats.Emplace(Stat, Stat->Initial);
		}

		// Set stat properties
		FTGOR_StatProperties& Properties = StatTable.FindOrAdd(Stat);
		Properties.LastUpdate = Timestamp;

		// Build effects table
		Properties.EffectTable.Empty();
		for (UTGOR_Effect* Effect : EffectsQueue)
		{
			// Add effects
			if (*Effect->Stat && Stat->IsA(Effect->Stat))
			{
				Properties.EffectTable.Emplace(Effect);
			}
		}
		Properties.EffectTable.Sort([](const UTGOR_Effect& A, const UTGOR_Effect& B) -> bool { return (A.Min == B.Min) ? (A.Max < B.Max) : (A.Min < B.Min); });

		// Build responses table
		Properties.ResponseTable.Empty();
		const TArray<UTGOR_Response*>& ResponseQueue = Stat->Instanced_ResponseInsertions.Collection; //GetIListFromType<UTGOR_Response>();
		for (UTGOR_Response* Response : ResponseQueue)
		{
			// Add responses
			Properties.ResponseTable.Emplace(Response);
		}
	}

	UpdateEffects();
	OnStatChanged.Broadcast();
}

TMap<int32, UTGOR_SpawnModule*> UTGOR_StatComponent::GetModuleType_Implementation() const
{
	TMap<int32, UTGOR_SpawnModule*> Modules;

	const int32 Num = ActiveEffects.Num();
	for (int32 Index = 0; Index < Num; Index++)
	{
		Modules.Emplace(Index, ActiveEffects[Index]);
	}
	return Modules;
}

TArray<UTGOR_Modifier*> UTGOR_StatComponent::QueryActiveModifiers_Implementation() const
{
	TArray<UTGOR_Modifier*> Modifiers;
	for(UTGOR_Effect* ActiveEffect : ActiveEffects)
	{
		Modifiers.Emplace(ActiveEffect);
	}
	return Modifiers;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void UTGOR_StatComponent::ReplicatedStat(const FTGOR_StatState& Old)
{
	bool HasChanged = false;

	// Check replication difference
	for (const auto& Pair : State.Stats)
	{
		const FTGOR_Percentage* Ptr = Old.Stats.Find(Pair.Key);
		if (Ptr)
		{
			// Remember changes
			const float Delta = Pair.Value.Value - Ptr->Value;
			if (FMath::Square(Delta) > SMALL_NUMBER)
			{
				HasChanged = true;
			}
		}
	}

	// Reset buffer
	StatBuffer.Empty();
	if (HasChanged)
	{
		UpdateEffects();
		OnStatChanged.Broadcast();
	}
}


void UTGOR_StatComponent::UpdateStatValues()
{
	bool HasChanged = false;

	// Deal with each stat individually
	for (auto& Pair : State.Stats)
	{
		// Use buffered values
		const float* Ptr = StatBuffer.Find(Pair.Key);
		if (Ptr)
		{
			Pair.Value.Value += (*Ptr);

			// Remember changes
			if (FMath::Square(*Ptr) > SMALL_NUMBER)
			{
				HasChanged = true;
			}

			// Make sure we stay in bounds
			Pair.Value.Value = FMath::Clamp(Pair.Value.Value, 0.0f, 1.0f);
		}
	}

	// Reset buffer
	StatBuffer.Empty();
	if (HasChanged)
	{
		UpdateEffects();
		OnStatChanged.Broadcast();
	}
}


void UTGOR_StatComponent::UpdateEffects()
{
	ActiveEffects.Empty();

	UTGOR_AttributeComponent* AttributeComponent = GetOwnerComponent<UTGOR_AttributeComponent>();
	if (IsValid(AttributeComponent))
	{
		const TArray<FTGOR_StatOutput> Currents = GetStats();
		for (const FTGOR_StatOutput& Current : Currents)
		{
			for (UTGOR_Effect* ActiveEffect : Current.Active)
			{
				ActiveEffects.Emplace(ActiveEffect);
			}
		}
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////

TArray<FTGOR_StatOutput> UTGOR_StatComponent::GetStats() const
{
	TArray<FTGOR_StatOutput> Outputs;

	for (const auto& Pair : State.Stats)
	{
		FTGOR_StatOutput Output;
		Output.Stat = Pair.Key;
		Output.Current = Pair.Value.Value;

		const float* Buf = StatBuffer.Find(Pair.Key);
		if (Buf)
		{
			Output.Buffered = FMath::Clamp(Pair.Value.Value + (*Buf), 0.0f, 1.0f);
		}
		else
		{
			Output.Buffered = Output.Current;
		}

		const FTGOR_StatProperties* Ptr = StatTable.Find(Pair.Key);
		if (Ptr)
		{
			Output.HadBufferUpdate = Ptr->HadBufferUpdate;

			Output.Active.Empty();
			Output.Inactive.Empty();
			for (UTGOR_Effect* Effect : Ptr->EffectTable)
			{
				if (Effect->Min <= Output.Current && Output.Current <= Effect->Max)
				{
					Output.Active.Emplace(Effect);
				}
				else
				{
					Output.Inactive.Emplace(Effect);
				}
			}
		}

		Outputs.Emplace(Output);
	}

	return Outputs;
}

float UTGOR_StatComponent::GetStatValue(UTGOR_Stat* Stat) const
{
	const FTGOR_Percentage* Ptr = State.Stats.Find(Stat);
	if(Ptr)
	{
		return Ptr->Value;
	}
	return 0.0f;
}

void UTGOR_StatComponent::AddStatValue(UTGOR_Stat* Stat, float Value)
{
	float& Buffer = StatBuffer.FindOrAdd(Stat, 0.0f);
	Buffer += Value;

	// Keep track of change
	FTGOR_StatProperties* Ptr = StatTable.Find(Stat);
	if (Ptr && FMath::Square(Value) > SMALL_NUMBER)
	{
		Ptr->HadBufferUpdate = true;
	}
}

int32 UTGOR_StatComponent::ProcessEnergy(UTGOR_Segment* Segment, UTGOR_Energy* Energy, int32 Quantity)
{
	for (const auto& Pair : StatTable)
	{
		float& Buffer = StatBuffer.FindOrAdd(Pair.Key, 0.0f);
		float Value = GetStatValue(Pair.Key);

		for (UTGOR_Response* Response : Pair.Value.ResponseTable)
		{
			float Delta = 0.0f;
			Quantity = Response->Convert(this, Segment, Energy, Quantity, Value + Buffer, Delta);
			Buffer += Delta;
		}
	}
	return Quantity;
}

TMap<UTGOR_Matter*, int32> UTGOR_StatComponent::ProcessMatter(UTGOR_Segment* Segment, const TMap<UTGOR_Matter*, int32>& Composition)
{
	TMap<UTGOR_Matter*, int32> Residual;
	for (const auto& Pair : Composition)
	{
		const float Quantity = ProcessEnergy(Segment, Pair.Key, Pair.Value);
		Residual.Emplace(Pair.Key, Quantity);
	}
	return Residual;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
