///////////////////////////////////////////////////
// The Gateway of Realities: Planes of Existence //
// Copyright 2016-2017                           //
///////////////////////////////////////////////////


#include "TGOR_StatComponent.h"
#include "Net/UnrealNetwork.h"
#include "Realities/Base/TGOR_Singleton.h"

UTGOR_StatComponent::UTGOR_StatComponent()
	: Super(),
	RegenTimeout(3.0f),
	StatMax(100.0f)
{
	bWantsInitializeComponent = true;
	SetIsReplicatedByDefault(true);
	
}

void UTGOR_StatComponent::BeginPlay()
{
	Super::BeginPlay();

	State.Stat = StatMax;

	SINGLETON_CHK;
	State.RegenTime = Singleton->GetGameTimestamp() + RegenTimeout;
}

void UTGOR_StatComponent::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION(UTGOR_StatComponent, State, COND_SimulatedOnly);
}

void UTGOR_StatComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	SINGLETON_CHK;

	// Poll for inactive handles to be removed
	for (auto It = Handles.CreateIterator(); It; ++It)
	{
		const TSet<UTGOR_Content*> ContentContext = It->Value.Register->Execute_GetActiveContent(It->Value.Register.GetObject());
		if (!ContentContext.Contains(It->Key))
		{
			It.RemoveCurrent();
		}
	}

	if (CanRegen())
	{
		// Regenerate according to current handles
		const FTGOR_Time Timestamp = Singleton->GetGameTimestamp();
		if (Timestamp > State.RegenTime)
		{
			const float Regen = GetTotalRegen();
			State.Stat = FMath::Min(State.Stat + Regen, StatMax);
		}
	}
}

void UTGOR_StatComponent::ReplicatedStat(const FTGOR_StatState& Old)
{
	// Usually client should predict state changes before server update.
	// If we are currently regenerating but the server tells us we shouldn't, chances are something happened on the server that didn't for client.
	// (TODO: But it can also mean the update came in too quickly and said event will be client-predicted soon, aka, fire the stat-change event twice, producing visual artifacts. Oh well.)
	SINGLETON_CHK;
	const FTGOR_Time Timestamp = Singleton->GetGameTimestamp();
	if (Old.RegenTime < Timestamp && Timestamp < State.RegenTime)
	{
		OnStatChanged.Broadcast(State.Stat - Old.Stat);
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////

float UTGOR_StatComponent::GetTotalRegen() const
{
	float Regen = 0.0f;
	for (auto Pair : Handles)
	{
		Regen += Pair.Value.Regen;
	}

	// Make sure debuffs don't make regen go negative
	return FMath::Max(Regen, 0.0f);
}

void UTGOR_StatComponent::RegisterHandle(TScriptInterface<ITGOR_RegisterInterface> Register, UTGOR_Content* Content, float Regen)
{
	// If regen is too small, remove handle instead
	if (Regen * Regen < SMALL_NUMBER)
	{
		UnRegisterHandle(Content);
		return;
	}

	// Add handle
	FTGOR_RegenerationHandle& Handle = Handles.FindOrAdd(Content);
	Handle.Register = Register;
	Handle.Regen = Regen;
}

void UTGOR_StatComponent::UnRegisterHandle(UTGOR_Content* Content)
{
	// Remove handle
	//FTGOR_RegenerationHandle Handle;
	//Handles.RemoveAndCopyValue(Content, Handle);
	Handles.Remove(Content);
}


////////////////////////////////////////////////////////////////////////////////////////////////////


float UTGOR_StatComponent::GetStatRatio()
{
	if (StatMax < SMALL_NUMBER)
	{
		return(1.0f);
	}

	return(State.Stat / StatMax);
}

void UTGOR_StatComponent::SetStatMax(float Amount)
{
	if (Amount <= 0.0f)
	{
		ERROR("Attempted to set Zero or Negative StatMax", Warning);
	}

	StatMax = Amount;
	State.Stat = FMath::Min(State.Stat, StatMax);
}

void UTGOR_StatComponent::ChangeStat(float Amount)
{
	SINGLETON_CHK;

	// Change stat
	State.Stat = FMath::Clamp(State.Stat + Amount, 0.0f, StatMax);
	State.RegenTime = Singleton->GetGameTimestamp() + RegenTimeout;

	// Notify
	OnStatChanged.Broadcast(Amount);
}

bool UTGOR_StatComponent::CanRegen() const
{
	return true;
}
