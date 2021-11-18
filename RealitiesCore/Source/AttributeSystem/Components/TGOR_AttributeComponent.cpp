// The Gateway of Realities: Planes of Existence.
#include "TGOR_AttributeComponent.h"

#include "RealitiesUGC/Mod/TGOR_ContentManager.h"
#include "CoreSystem/TGOR_Singleton.h"

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

void UTGOR_AttributeComponent::UpdateContent_Implementation(FTGOR_SpawnerDependencies& Dependencies)
{
	ITGOR_SpawnerInterface::UpdateContent_Implementation(Dependencies);
}

TMap<int32, UTGOR_SpawnModule*> UTGOR_AttributeComponent::GetModuleType_Implementation() const
{
	return TMap<int32, UTGOR_SpawnModule*>();
}

////////////////////////////////////////////////////////////////////////////////////////////////////

float UTGOR_AttributeComponent::ComputeAttributeValue(TSubclassOf<UTGOR_Attribute> AttributeType, float DefaultValue) const
{
	TOptional<float> Value;

	// Grab active modifiers, use default value of first matching entry
	const TArray<UTGOR_Modifier*> Modifiers = QueryActiveModifiers();
	for (UTGOR_Modifier* Modifier : Modifiers)
	{
		const auto Pair = Modifier->Instanced_AttributeInsertions.GetOfType<UTGOR_Attribute>(AttributeType);
		if (IsValid(Pair.Key))
		{
			Value = Pair.Key->ModifyValue(Value.Get(Pair.Key->DefaultValue), Pair.Value);
		}
	}
	return Value.Get(DefaultValue);
}

TArray<FTGOR_AttributeOutput> UTGOR_AttributeComponent::ComputeActiveAttributes() const
{
	TMap<UTGOR_Attribute*, float> AttributeValues;
	TMap<UTGOR_Attribute*, TMap<UTGOR_Modifier*, float>> Contributions;

	// Grab active modifiers
	const TArray<UTGOR_Modifier*> Modifiers = QueryActiveModifiers();
	for (UTGOR_Modifier* Modifier : Modifiers)
	{
		for (auto Pair : Modifier->Instanced_AttributeInsertions.Collection)
		{
			float& Value = AttributeValues.FindOrAdd(Pair.Key, Pair.Key->DefaultValue);
			Value = Pair.Key->ModifyValue(Value, Pair.Value);

			Contributions.FindOrAdd(Pair.Key).Emplace(Modifier, Pair.Value);
		}
	}

	// Compile outputs
	TArray<FTGOR_AttributeOutput> AttributeOutputs;
	for (auto Pair : AttributeValues)
	{
		FTGOR_AttributeOutput Output;
		Output.Attribute = Pair.Key;
		Output.Value = Pair.Value;
		Output.Contributions = Contributions[Pair.Key];
		AttributeOutputs.Emplace(Output);
	}
	return AttributeOutputs;
}

float UTGOR_AttributeComponent::GetAttributeValue(AActor* Actor, TSubclassOf<UTGOR_Attribute> AttributeType, float DefaultValue)
{
	UTGOR_AttributeComponent* AttributeComponent = Actor->FindComponentByClass<UTGOR_AttributeComponent>();
	if (IsValid(AttributeComponent))
	{
		return AttributeComponent->ComputeAttributeValue(AttributeType, DefaultValue);
	}
	return DefaultValue;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

TArray<UTGOR_Modifier*> UTGOR_AttributeComponent::QueryActiveModifiers() const
{
	TArray<UTGOR_Modifier*> Modifiers;

	AActor* Actor = GetOwner();
	if (IsValid(Actor))
	{
		if (Actor->Implements<UTGOR_AttributeInterface>())
		{
			Modifiers.Append(ITGOR_AttributeInterface::Execute_QueryActiveModifiers(Actor));
		}

		const TSet<UActorComponent*>& Components = Actor->GetComponents();
		for (UActorComponent* Component : Components)
		{
			if (Component->Implements<UTGOR_AttributeInterface>())
			{
				Modifiers.Append(ITGOR_AttributeInterface::Execute_QueryActiveModifiers(Component));
			}
		}
	}

	return Modifiers;
}
