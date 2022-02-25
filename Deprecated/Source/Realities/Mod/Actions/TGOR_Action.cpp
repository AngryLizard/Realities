// The Gateway of Realities: Planes of Existence.


#include "TGOR_Action.h"

#include "Realities/Mod/Animations/TGOR_Animation.h"
#include "Realities/Mod/Spawner/Items/TGOR_Item.h"
#include "Realities/Base/Inventory/TGOR_ItemStorage.h"
#include "Realities/Base/Inventory/TGOR_ActionStorage.h"
#include "Realities/Components/System/Actions/TGOR_ActionComponent.h"
#include "Realities/Actors/Pawns/TGOR_Pawn.h"
#include "Realities/Base/TGOR_Singleton.h"

UTGOR_Action::UTGOR_Action()
	: ScheduleCooldown(0.0f),
	Importance(ETGOR_ReplicationType::Reliable),
	Notify(ETGOR_NotifyReplication::NoValidation),
	SupportedItem()
{
}

void UTGOR_Action::PostBuildResource()
{
	Super::PostBuildResource();
}

UTGOR_ActionStorage* UTGOR_Action::CreateStorage()
{
	UTGOR_ActionStorage* Storage = NewObject<UTGOR_ActionStorage>(this);
	Storage->ActionRef = this;
	BuildStorage(Storage);
	return Storage;
}

void UTGOR_Action::BuildStorage(UTGOR_ActionStorage* Storage)
{
	Storage->BuildModules(Modules);
	InitialiseStorage(Storage);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool UTGOR_Action::IsReliable() const
{
	return Importance == ETGOR_ReplicationType::Reliable;
}

bool UTGOR_Action::SupportsItem(UTGOR_Item* Item) const
{
	if (Item)
	{
		return Item->IsSupportedBy(SupportedItem);
	}
	return false;
}

bool UTGOR_Action::CanCall(UTGOR_ActionComponent* Component, const FTGOR_SlotIndex& SlotIndex)
{
	if (!Component->Execute_SupportsContent(Component, this)) return false;

	SINGLETON_RETCHK(false);

	// Check cooldown
	const float Cooldown = Component->GetSlotCooldown(SlotIndex);
	if (Cooldown >= ScheduleCooldown)
	{
		// Check invariant on empty params
		bool Valid = Prevariant(Component);
		if (Component->Verbose && !Valid)
		{
			EOUT(DefaultName + "'s invariant returned false")
		}
		return Valid;
	}

	return false;
}


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool UTGOR_Action::Prevariant(UTGOR_ActionComponent* Component) const
{
	return true;
}

bool UTGOR_Action::Invariant(UTGOR_ActionStorage* Storage, UTGOR_ActionComponent* Component) const
{
	return Prevariant(Component);
}

void UTGOR_Action::Activate(UTGOR_ActionStorage* Storage, UTGOR_ActionComponent* Component)
{
	if (*MainAnimation && IsValid(Component))
	{
		Component->PlayAnimation(MainAnimation);
	}
}

bool UTGOR_Action::Equip(UTGOR_ActionStorage* Storage, UTGOR_ActionComponent* Component, float Deltatime)
{
	return true;
}

void UTGOR_Action::ForceEquip(UTGOR_ActionStorage* Storage, UTGOR_ActionComponent* Component)
{
}

bool UTGOR_Action::Operate(UTGOR_ActionStorage* Storage, UTGOR_ActionComponent* Component, float Deltatime)
{
	return true;
}

bool UTGOR_Action::Unequip(UTGOR_ActionStorage* Storage, UTGOR_ActionComponent* Component, float Deltatime)
{
	return true;
}

void UTGOR_Action::ForceUnequip(UTGOR_ActionStorage* Storage, UTGOR_ActionComponent* Component)
{
	if (IsValid(Component))
	{
		Component->PlayAnimation(nullptr);
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////
