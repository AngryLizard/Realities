// The Gateway of Realities: Planes of Existence. By Salireths.

#include "TGOR_Item.h"

#include "Realities/Components/Inventory/TGOR_ItemComponent.h"
#include "Realities/Components/Dimension/Interactable/TGOR_WorldInteractableComponent.h"
#include "Realities/Base/Inventory/TGOR_ItemStorage.h"
#include "Realities/Base/Inventory/Modules/TGOR_Module.h"
#include "Realities/Actors/Inventory/TGOR_ItemDropActor.h"

#include "Realities/Base/TGOR_Singleton.h"
#include "Realities/Mod/Matters/TGOR_Matter.h"
#include "Realities/Base/Content/TGOR_ContentManager.h"
#include "Realities/Base/Content/TGOR_Content.h"

const float UTGOR_Item::DEFAULT_ITEM_LIFETIME = 300.0f;

UTGOR_Item::UTGOR_Item()
	: Super(),
	Radius(64.0f),
	HalfHeight(0.0f)
{
}

TSubclassOf<AActor> UTGOR_Item::GetSpawnClass_Implementation() const
{
	return DefaultItemActor;
}

void UTGOR_Item::PostBuildResource()
{
	Super::PostBuildResource();

	SINGLETON_CHK;
	UTGOR_ContentManager* ContentManager = Singleton->GetContentManager();

	// Generate composition
	CompositionCapacity.Reset();
	for (auto& Pair : MatterComposition)
	{
		UTGOR_Matter* Content = ContentManager->GetTFromType<UTGOR_Matter>(Pair.Key); // *this breaks if Matter isn't included in item.h, see header for more.
		if (IsValid(Content))
		{
			CompositionCapacity.Add(Content, Pair.Value);
		}
	}
}

UTGOR_ItemStorage* UTGOR_Item::CreateStorage()
{
	UTGOR_ItemStorage* Storage = NewObject<UTGOR_ItemStorage>(this);
	Storage->ItemRef = this;
	BuildStorage(Storage);
	return Storage;
}

void UTGOR_Item::BuildStorage(UTGOR_ItemStorage* Storage)
{
	Storage->BuildModules(Modules);
	InitialiseStorage(Storage);
}

void UTGOR_Item::SetCapsuleSize(UCapsuleComponent* Capsule)
{
	if (IsValid(Capsule))
	{
		Capsule->SetCapsuleSize(Radius, HalfHeight + Radius);
	}
}

float UTGOR_Item::GetRadius() const
{
	return Radius;
}

float UTGOR_Item::GetHeight() const
{
	return (HalfHeight + Radius) * 2;
}

FTGOR_MovementShape UTGOR_Item::GetShape() const
{
	FTGOR_MovementShape Shape;
	Shape.Height = GetHeight();
	Shape.Radius = GetRadius();
	return Shape;
}

bool UTGOR_Item::CanAssembleWith(const TMap<UTGOR_Matter*, int32>& Matter) const
{
	// Check for each matter type whether entry exists and is sufficient
	for (const auto& Pair : CompositionCapacity)
	{
		if (Pair.Value > 0)
		{
			const int32* Ptr = Matter.Find(Pair.Key);
			if (!Ptr || (*Ptr) < Pair.Value)
			{
				return false;
			}
		}
	}
	return true;
}

bool UTGOR_Item::IsCompatibleWith(const TMap<UTGOR_Matter*, int32>& Matter) const
{
	// Check for each matter type whether entry exists and is sufficient
	for (const auto& Pair : CompositionCapacity)
	{
		if (Pair.Value > 0)
		{
			const int32* Ptr = Matter.Find(Pair.Key);
			if (Ptr && (*Ptr) > 0)
			{
				return true;
			}
		}
	}
	return false;
}

bool UTGOR_Item::CanDisassembleInto(const TSet<UTGOR_Matter*>& Matter) const
{
	// Check for each matter type whether entry exists
	for (const auto& Pair : CompositionCapacity)
	{
		if (!Matter.Contains(Pair.Key))
		{
			return false;
		}
	}
	return true;
}

bool UTGOR_Item::HasComposition(UTGOR_Matter* Matter) const
{
	return CompositionCapacity.Contains(Matter);
}

int32 UTGOR_Item::GetCapacityBits(UTGOR_Matter* Matter) const
{
	const int32 Quantity = CompositionCapacity[Matter];
	return UTGOR_Math::Log2(Quantity) + 1;
}

float UTGOR_Item::GetDefaultItemLifeTime()
{
	return UTGOR_Item::DEFAULT_ITEM_LIFETIME;
}


bool UTGOR_Item::IsSupportedBy(const FTGOR_ItemRestriction& Restriction) const
{
	if (*Restriction.ItemType && IsA(Restriction.ItemType))
	{
		if (!*Restriction.ModuleType)
		{
			return true;
		}

		for (const auto& Pair : Modules.Modules)
		{
			if (Pair.Value->IsChildOf(Restriction.ModuleType))
			{
				return true;
			}
		}
	}
	return false;
}