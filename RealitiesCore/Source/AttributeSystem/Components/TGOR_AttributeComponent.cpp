// The Gateway of Realities: Planes of Existence.
#include "TGOR_AttributeComponent.h"

#include "AttributeSystem/Interfaces/TGOR_AttributeInterface.h"
#include "AttributeSystem/Content/TGOR_Attribute.h"
#include "AttributeSystem/Content/TGOR_Modifier.h"
#include "GameFramework/Actor.h"


////////////////////////////////////////////////////////////////////////////////////////////////////

UTGOR_AttributeComponent::UTGOR_AttributeComponent()
	: Super()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UTGOR_AttributeComponent::BeginPlay()
{
	Super::BeginPlay();
	
}

void UTGOR_AttributeComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	bool Changed = false;

	// Poll for inactive handles to be removed
	for (auto It = Handles.CreateIterator(); It; ++It)
	{
		const TSet<UTGOR_CoreContent*> ContentContext = It->Value.Register->Execute_GetActiveContent(It->Value.Register.GetObject());
		if (!ContentContext.Contains(It->Key))
		{
			Changed = true;
			It.RemoveCurrent();
		}
	}

	if (Changed)
	{
		RecomputeValues();
		AttributeUpdate(GetOwner());
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void UTGOR_AttributeComponent::RecomputeValues()
{
	AttributeValues.Empty();
	for (const auto& Pair : Handles)
	{
		AddValues(Pair.Key->AttributeValues);
	}
	AttributeUpdate(GetOwner());
}

void UTGOR_AttributeComponent::AddValues(const TMap<UTGOR_Attribute*, float>& Values)
{
	for (const auto& Pair : Values)
	{
		float& Value = AttributeValues.FindOrAdd(Pair.Key, Pair.Key->DefaultValue);
		switch (Pair.Key->Accumulation)
		{
		case ETGOR_AttributeAccumulation::Add: Value += Pair.Value; break;
		case ETGOR_AttributeAccumulation::Mul: Value *= Pair.Value; break;
		case ETGOR_AttributeAccumulation::Max: Value = FMath::Max(Value, Pair.Value); break;
		}
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void UTGOR_AttributeComponent::RegisterHandle(TScriptInterface<ITGOR_RegisterInterface> Register, UTGOR_Modifier* Modifier)
{
	// Add handle
	FTGOR_AttributeHandle* Ptr = Handles.Find(Modifier);
	if (Ptr)
	{
		Ptr->Register = Register;
	}
	else
	{
		FTGOR_AttributeHandle Handle;
		Handle.Register = Register;
		Handles.Emplace(Modifier, Handle);

		AddValues(Modifier->AttributeValues);
		AttributeUpdate(GetOwner());
	}
}

void UTGOR_AttributeComponent::UnregisterHandle(UTGOR_Modifier* Modifier)
{
	// Remove handle
	Handles.Remove(Modifier);

	RecomputeValues();
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void UTGOR_AttributeComponent::UpdateAttributes(TMap<UTGOR_Attribute*, float>& Attributes) const
{
	for (auto& Pair : Attributes)
	{
		const float* Ptr = AttributeValues.Find(Pair.Key);
		if (Ptr)
		{
			Pair.Value = (*Ptr);
		}
		else
		{
			Pair.Value = Pair.Key->DefaultValue;
		}
	}
}


void UTGOR_AttributeComponent::AttributeUpdate(AActor* Actor)
{
	if (IsValid(Actor))
	{
		if (Actor->Implements<UTGOR_AttributeInterface>())
		{
			ITGOR_AttributeInterface::Execute_UpdateAttributes(Actor, this);
		}

		const TSet<UActorComponent*>& Components = Actor->GetComponents();
		for (UActorComponent* Component : Components)
		{
			if (Component->Implements<UTGOR_AttributeInterface>())
			{
				ITGOR_AttributeInterface::Execute_UpdateAttributes(Component, this);
			}
		}
	}
}

TArray<FTGOR_AttributeOutput> UTGOR_AttributeComponent::GetAttributes() const
{
	TMap<UTGOR_Attribute*, TMap<UTGOR_Modifier*, float>> Contributions;

	// Figure out contributions for each attribute
	for (const auto& HandlePair : Handles)
	{
		UTGOR_Modifier* Modifier = HandlePair.Key;
		for (const auto& AttributePair : Modifier->AttributeValues)
		{
			Contributions.FindOrAdd(AttributePair.Key).Emplace(Modifier, AttributePair.Value);
		}
	}

	// Build output
	TArray<FTGOR_AttributeOutput> Output;
	for (const auto& AttributePair : AttributeValues)
	{
		FTGOR_AttributeOutput Sample;
		Sample.Attribute = AttributePair.Key;
		Sample.Value = AttributePair.Value;
		Sample.Contributions = Contributions.FindOrAdd(Sample.Attribute);

		Output.Emplace(Sample);
	}
	return Output;
}