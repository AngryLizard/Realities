// The Gateway of Realities: Planes of Existence. By Salireths.

#include "TGOR_Item.h"

#include "RealitiesUtility/Utility/TGOR_Math.h"

#include "InventorySystem/Components/TGOR_ItemComponent.h"
#include "InventorySystem/Storage/TGOR_ItemStorage.h"
#include "InventorySystem/Storage/Modules/TGOR_Module.h"
#include "InventorySystem/Actors/TGOR_ItemDropActor.h"

#include "CoreSystem/TGOR_Singleton.h"
#include "InventorySystem/Content/TGOR_Matter.h"
#include "CoreSystem/Content/TGOR_CoreContent.h"


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
	const TMap<UTGOR_Matter*, int32>& Capacity = Instanced_MatterInsertions.Collection;
	for (const auto& Pair : Capacity)
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
	const TMap<UTGOR_Matter*, int32>& Capacity = Instanced_MatterInsertions.Collection;
	for (const auto& Pair : Capacity)
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
	const TMap<UTGOR_Matter*, int32>& Capacity = Instanced_MatterInsertions.Collection;
	for (const auto& Pair : Capacity)
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
	const TMap<UTGOR_Matter*, int32>& Capacity = Instanced_MatterInsertions.Collection;
	return Capacity.Contains(Matter);
}

int32 UTGOR_Item::GetCapacityBits(UTGOR_Matter* Matter) const
{
	const TMap<UTGOR_Matter*, int32>& Capacity = Instanced_MatterInsertions.Collection;
	const int32 Quantity = Capacity[Matter];
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

const TMap<UTGOR_Matter*, int32>& UTGOR_Item::GetComponentCapacity() const
{
	return Instanced_MatterInsertions.Collection;
}

void UTGOR_Item::MoveInsertion(UTGOR_Content* Insertion, ETGOR_InsertionActionEnumeration Action, bool& Success)
{
	Super::MoveInsertion(Insertion, Action, Success);

	MOV_INSERTION(MatterInsertions);
}

