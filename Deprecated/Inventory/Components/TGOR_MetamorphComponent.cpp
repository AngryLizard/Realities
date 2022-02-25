// The Gateway of Realities: Planes of Existence.

#include "TGOR_MetamorphComponent.h"
#include "Realities.h"

#include "Base/Instances/Inventory/TGOR_MetamorphInstance.h"
#include "Components/Dimension/TGOR_RegionComponent.h"
#include "Actors/Inventory/TGOR_ItemDropActor.h"

#include "Components/Creature/TGOR_ItemDropSimulationComponent.h"
#include "Components/Inventory/TGOR_SingleItemComponent.h"
#include "Mod/Metamorphs/TGOR_Metamorph.h"
#include "Mod/Items/TGOR_Item.h"

void UTGOR_MetamorphComponent::MaintainTimer()
{
	// Get resource table
	SINGLETON_CHK

	float Timestamp = Singleton->GameTimestamp;

	// Update all entries in queue
	for (auto& Pair : MetamorphQueue)
	{
		if (Pair.Value.LastUpdate + Pair.Value.MetamorphTime < Timestamp)
		{
			MetamorphQueue.Remove(Pair.Key);
		}
	}
}

bool UTGOR_MetamorphComponent::IncreaseTimer(const FTGOR_MetamorphRecipe& Recipe)
{
	// Clear the queue if metamorph class changed
	if (Recipe.Metamorph != _cache)
	{
		MetamorphQueue.Empty();
		_cache = Recipe.Metamorph;
	}

	// Get resource table
	SINGLETON_RETCHK(false);

	float Timestamp = Singleton->GameTimestamp;

	// Find proper metamorph if available or create if not
	FTGOR_MetamorphTimer* Ref = MetamorphQueue.Find(Recipe.Recipe);
	if (Ref == nullptr)
	{
		// Create new timer
		FTGOR_MetamorphTimer MetamorphTimer;
		MetamorphTimer.Progress = 0.0f;
		MetamorphTimer.LastUpdate = Timestamp;
		MetamorphTimer.MetamorphTime = GetMetamorphTime(Recipe);

		if (MetamorphTimer.MetamorphTime >= 0.0f)
		{
			MetamorphQueue.Add(Recipe.Recipe, MetamorphTimer);
		}
	}
	else
	{
		// Count up progress
		Ref->Progress += Timestamp - Ref->LastUpdate;
		Ref->LastUpdate = Timestamp;
		// If goal has been achieved, return true and remove timer
		if (Ref->Progress > Ref->MetamorphTime)
		{
			MetamorphQueue.Remove(Recipe.Recipe);
			return(true);
		}
	}

	return(false);
}

TArray<FTGOR_ItemInstance_OLD> UTGOR_MetamorphComponent::MetamorphItem(FTGOR_ItemInstance_OLD& ItemInstance, const FTGOR_ElementInstance& ElementInstance)
{
	TArray<FTGOR_ItemInstance_OLD> Output;
	SINGLETON_RETCHK(Output)

	// Get item class
	// TAKEN OUT BY LHYKOS FOR COMPATIBILITY
	/*UTGOR_Item* Item = ItemInstance.Content;
	if (IsValid(Item))
	{
		// Iterate through all recipes
		for (const FTGOR_MetamorphRecipe& Recipe : Item->MetamorphRecipeInputList)
		{
			// Get metamorph
			UTGOR_Metamorph* Metamorph = Recipe.Metamorph;
			if (IsValid(Metamorph))
			{
				// Make sure recipe is in range
				if (Metamorph->MetamorphRecipes.IsValidIndex(Recipe.Recipe))
				{
					// Check if metamorph is satisfied
					const FTGOR_MetamorphInstance& Instance = Metamorph->MetamorphRecipes[Recipe.Recipe];
					
					if (Instance.Satisfies(ElementInstance) &&
						Metamorph->InputFilter(Recipe.Recipe, this, ItemInstance))
					{
						// Increase timer and apply metamorph if applies
						if (IncreaseTimer(Recipe))
						{
							// Decrement input
							float Quantity = Instance.Input.Instance.Quantity;
							Item->Split(ItemInstance, Quantity);

							// Fill output
							for (const FTGOR_MetamorphEntryInstance& Entry : Instance.Output)
							{
								FTGOR_ItemInstance_OLD New = Entry.Instance;
								Metamorph->OutputPayload(Recipe.Recipe, this, New);
								Output.Add(New);
							}

							Recipe.Metamorph->Output(Recipe.Recipe, this, nullptr);
						}
					}
				}
			}
		}
	}*/

	return(Output);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

float UTGOR_MetamorphComponent::GetProgress(FTGOR_ElementInstance& ElementInstance) const
{
	if (_cache)
	{
		// Get recipe with highest progress
		int32 Recipe = -1;
		float Progress = 0.0f;
		for (const auto& Pair : MetamorphQueue)
		{
			float Current = Pair.Value.Progress / Pair.Value.MetamorphTime;
			if (Current > Progress)
			{
				Progress = Current;
				Recipe = Pair.Key;
			}
		}

		// Get element requirements of recipe
		if (_cache->MetamorphRecipes.IsValidIndex(Recipe))
		{
			const FTGOR_MetamorphInstance& Instance = _cache->MetamorphRecipes[Recipe];
			ElementInstance = (Instance.Min + Instance.Max) / 2;
			return(Progress);
		}
	}
	return(0.0f);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

TArray<UTGOR_ContainerComponent*> UTGOR_MetamorphComponent::GetContainersAround(float Radius)
{
	TArray<UTGOR_ContainerComponent*> Containers;

	// Find Region component
	AActor* Actor = GetOwner();
	UTGOR_RegionComponent* RegionComponent = Cast<UTGOR_RegionComponent>(Actor->GetComponentByClass(UTGOR_RegionComponent::StaticClass()));
	if (!IsValid(RegionComponent))
	{
		return(Containers);
	}


	// Get nearby itemdrops
	RegionComponent->CallNearby<UTGOR_ItemDropSimulationComponent>([this, &Containers](UTGOR_ItemDropSimulationComponent* Component, float Ratio)
	{
		if (IsValid(Component->ItemDrop) && IsValid(Component->ItemDrop->Container))
		{
			Containers.Add(Component->ItemDrop->Container);
		}
	}, Radius, false);

	return(Containers);
}


TArray<FTGOR_ItemInstance_OLD> UTGOR_MetamorphComponent::CombineItemsInside(
	const TArray<UTGOR_ContainerComponent*>& Containers,
	const FTGOR_MetamorphRecipe& Recipe,
	AActor* Instigator)
{
	TArray<FTGOR_ItemInstance_OLD> Output;
	
	// Check and get available
	TArray<FTGOR_ContainerList> Available;
	TMap<UTGOR_ContainerComponent*, FTGOR_ContainerItemPair> Register;
	if (!CanCombineItemsInside(Containers, Available, Register, Recipe))
	{
		return (Output);
	}

	SINGLETON_RETCHK(Output);

	// Get metamorph
	const FTGOR_CombinationInstance& Instance = Recipe.Metamorph->CombinationRecipes[Recipe.Recipe];

	// Iterate through all found registers and adapz item quantities
	for (auto& ContainerPair : Register)
	{
		for (auto& ItemPair : ContainerPair.Value.Items)
		{
			// Get item instance from container
			ETGOR_FetchEnumeration State;
			FTGOR_ItemInstance_OLD& ItemInstance = ContainerPair.Key->GetSlot(ItemPair.Key, State);
			if (State == ETGOR_FetchEnumeration::Found)
			{
				// Split off correct quantity
				float Quantity = ItemInstance.Quantity - ItemPair.Value;
				if (Quantity > SMALL_NUMBER)
				{
					// Get item class to perform split
					UTGOR_Item* Item = ItemInstance.Content;
					if (IsValid(Item))
					{
						// TAKEN OUT BY LHYKOS FOR COMPATIBILITY
						//Item->Split(ItemInstance, Quantity);
					}
				}
			}
		}
	}
	
	// Fill output
	for (const FTGOR_MetamorphEntryInstance& Entry : Instance.Output)
	{
		FTGOR_ItemInstance_OLD New = Entry.Instance;
		Recipe.Metamorph->OutputPayload(Recipe.Recipe, this, New);
		Output.Add(New);
	}

	Recipe.Metamorph->Output(Recipe.Recipe, this, Instigator);
	return(Output);
}


bool UTGOR_MetamorphComponent::CanCombineItemsInside(
	const TArray<UTGOR_ContainerComponent*>& Containers,
	TArray<FTGOR_ContainerList>& Available,
	TMap<UTGOR_ContainerComponent*, FTGOR_ContainerItemPair>& Register,
	const FTGOR_MetamorphRecipe& Recipe)
{
	SINGLETON_RETCHK(false);

	// Reset register
	Register.Empty();
	for (UTGOR_ContainerComponent* Container : Containers)
	{
		if (IsValid(Container))
		{
			Register.Add(Container, FTGOR_ContainerItemPair());
		}
	}

	// Get metamorph
	const UTGOR_Metamorph* Metamorph = Recipe.Metamorph;
	if (IsValid(Metamorph))
	{
		// Get all available elements that allow children
		const FTGOR_CombinationInstance& Instance = Metamorph->CombinationRecipes[Recipe.Recipe];

		// Set up available array
		const TArray<FTGOR_MetamorphEntryInstance>& InputArray = Instance.Input;
		int32 InputNum = InputArray.Num();
		Available.SetNum(InputNum);
		
		// Get available itemdrops for each entry
		if (!GetAvailableItemsInside(Containers, Available, Register, InputArray))
		{
			return(false);
		}
		
		// Go through all inputs and find which items to remove from what drops
		for (int i = 0; i < InputNum; i++)
		{
			const FTGOR_MetamorphEntryInstance& Entry = InputArray[i];
			const TArray<FTGOR_ContainerPair>& Pairs = Available[i].Containers;

			// Get amount to be removed
			float Pending = Entry.Instance.Quantity;

			// Reduce quantity until reached
			for (const FTGOR_ContainerPair& Pair : Pairs)
			{
				float& Quantity = Register[Pair.Container].Items[Pair.Index];

				// Decrease Quantity and Pending
				Quantity -= Pending;
				Pending = -FMath::Min(Quantity, 0.0f);
				Quantity = FMath::Max(Quantity, 0.0f);
			}

			// if not all resources have been spent then transaction isn't possible
			if (Pending > SMALL_NUMBER)
			{
				return(false);
			}
		}

		return(true);
	}

	return(false);
}


bool UTGOR_MetamorphComponent::GetAvailableItemsInside(
	const TArray<UTGOR_ContainerComponent*>& Containers,
	TArray<FTGOR_ContainerList>& Available,
	TMap<UTGOR_ContainerComponent*, FTGOR_ContainerItemPair>& Register,
	const TArray<FTGOR_MetamorphEntryInstance>& Input)
{
	SINGLETON_RETCHK(false);

	for (UTGOR_ContainerComponent* Container : Containers)
	{
		if (IsValid(Container))
		{
			Container->ForEachSlot([this, Container, &Available, &Register, &Input](FTGOR_SlotIndex Index, FTGOR_ItemInstance_OLD& ItemInstance)
			{
				UTGOR_Item* Item = ItemInstance.Content;

				if (IsValid(Item))
				{
					int32 Num = Input.Num();
					for (int i = 0; i < Num; i++)
					{
						const FTGOR_MetamorphEntryInstance& Entry = Input[i];
						if ((Item->IsA(Entry.Item) && Entry.AllowChildren) ||
							(Item == Entry.Instance.Content && !Entry.AllowChildren))
						{
							// Add available pair (Duplicates possible, but don't matter)
							FTGOR_ContainerPair Pair;
							Pair.Container = Container;
							Pair.Index = Index;
							Pair.Class = Item->GetClass();
							Available[i].Containers.Add(Pair);

							// Add register pair
							FTGOR_ContainerItemPair& ItemPair = Register[Container];
							ItemPair.Items.Add(Index, ItemInstance.Quantity);
						}
					}
				}

				return(true);
			});
		}
	}

	for (FTGOR_ContainerList& Pairs : Available)
	{
		// Sort inputs to put more specific inputs first
		Pairs.Containers.Sort([](const FTGOR_ContainerPair& a, const FTGOR_ContainerPair& b)->bool
		{
			int32 LeftDistance = UTGOR_Math::GetClassDistance(a.Class, UTGOR_Item::StaticClass());
			int32 RightDistance = UTGOR_Math::GetClassDistance(b.Class, UTGOR_Item::StaticClass());

			return(LeftDistance < RightDistance);
		});
	}

	return(true);
}


////////////////////////////////////////////////////////////////////////////////////////////////////

float UTGOR_MetamorphComponent::GetMetamorphTime(const FTGOR_MetamorphRecipe& Recipe)
{
	// Find if influence expired
	if (IsValid(Recipe.Metamorph))
	{
		// Get Metamorph instance
		int32 Index = Recipe.Recipe;
		TArray<FTGOR_MetamorphInstance>& Instances = Recipe.Metamorph->MetamorphRecipes;

		if (0 <= Index && Index < Instances.Num())
		{
			return(Instances[Index].Time);
		}
	}
	return(-1.0f);
}