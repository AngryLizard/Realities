// The Gateway of Realities: Planes of Existence.


#include "TGOR_EquipableAction.h"
#include "Realities/Base/Inventory/TGOR_ActionStorage.h"
#include "Realities/Components/Combat/TGOR_AimComponent.h"
#include "Realities/Components/System/Actions/TGOR_ActionComponent.h"
#include "Realities/Components/Movement/TGOR_MovementComponent.h"
#include "Realities/Mod/Movements/TGOR_Movement.h"
#include "Realities/Mod/Targets/TGOR_Target.h"

UTGOR_EquipableAction::UTGOR_EquipableAction()
	: Super()
{
	ScheduleCooldown = 0.0f;
	AimFilter = UTGOR_Target::StaticClass();
	FixesAim = true;

	MaxEquipTime = 0.0f;
	MaxUnequipTime = 0.0f;
}

bool UTGOR_EquipableAction::Prevariant(UTGOR_ActionComponent* Component) const
{
	return(Invariant(nullptr, Component));
}

bool UTGOR_EquipableAction::Invariant(UTGOR_ActionStorage* Storage, UTGOR_ActionComponent* Component) const
{
	ETGOR_InvarianceEnumeration Invariance = ETGOR_InvarianceEnumeration::Valid;
	if (IsValid(Component))
	{
		CanTick(Storage, Component, Invariance);
	}

	if (Invariance == ETGOR_InvarianceEnumeration::Valid)
	{
		return(true);
	}
	else if (Invariance == ETGOR_InvarianceEnumeration::Uncallable)
	{
		// Check whether equipable has already been called
		return(IsValid(Storage));
	}
	return(false);
}

void UTGOR_EquipableAction::Activate(UTGOR_ActionStorage* Storage, UTGOR_ActionComponent* Component)
{
	if (IsValid(Component) && IsValid(Storage))
	{
		if (FixesAim)
		{
			FixAim(Component);
		}
		else if (AimFilter != UTGOR_Target::StaticClass())
		{
			FilterAim(Component, AimFilter);
		}

		PrepareTick(Storage, Component);
	}
	Super::Activate(Storage, Component);
}

bool UTGOR_EquipableAction::Equip(UTGOR_ActionStorage* Storage, UTGOR_ActionComponent* Component, float Deltatime)
{
	ETGOR_WaitEnumeration Wait = ETGOR_WaitEnumeration::Continue;
	if (IsValid(Component) && IsValid(Storage))
	{
		AwaitStartTick(Storage, Component, Deltatime, Wait);
	}
	return(Wait == ETGOR_WaitEnumeration::Wait);
}

void UTGOR_EquipableAction::ForceEquip(UTGOR_ActionStorage* Storage, UTGOR_ActionComponent* Component)
{
	if (IsValid(Component) && IsValid(Storage))
	{
		StartTick(Storage, Component);
	}
	Super::ForceEquip(Storage, Component);
}

bool UTGOR_EquipableAction::Operate(UTGOR_ActionStorage* Storage, UTGOR_ActionComponent* Component, float Deltatime)
{
	ETGOR_TerminationEnumeration Termination = ETGOR_TerminationEnumeration::Terminate;
	if (IsValid(Component))
	{
		Tick(Storage, Component, Deltatime, Termination);
	}
	return(Termination == ETGOR_TerminationEnumeration::Continue);
}

bool UTGOR_EquipableAction::Unequip(UTGOR_ActionStorage* Storage, UTGOR_ActionComponent* Component, float Deltatime)
{
	ETGOR_WaitEnumeration Wait = ETGOR_WaitEnumeration::Continue;
	if (IsValid(Component) && IsValid(Storage))
	{
		AwaitEndTick(Storage, Component, Deltatime, Wait);
	}
	return(Wait == ETGOR_WaitEnumeration::Wait);
}

void UTGOR_EquipableAction::ForceUnequip(UTGOR_ActionStorage* Storage, UTGOR_ActionComponent* Component)
{
	if (IsValid(Component))
	{
		if (FixesAim || AimFilter != UTGOR_Target::StaticClass())
		{
			FreeAim(Component);
		}

		EndTick(Storage, Component);
	}
	Super::ForceUnequip(Storage, Component);
}

////////////////////////////////////////////////////////////////////////////////////////////////////


void UTGOR_EquipableAction::CheckAim(UTGOR_Component* Component, ETGOR_AimIgnoreEnumeration Ignore, bool& Valid, FTGOR_AimInstance& Instance, FVector& Location, UTGOR_AimComponent*& Aim) const
{
	Valid = false;

	// Unsuspend aiming
	Aim = Component->GetOwnerComponent<UTGOR_AimComponent>();
	if (IsValid(Aim))
	{
		Instance = Aim->GetCurrentAim();
		Location = Aim->ComputeAimLocation(Instance, true);

		if (ContainsI(Instance.Target) && Instance.Component.IsValid())
		{
			if (Ignore == ETGOR_AimIgnoreEnumeration::IgnoreSelf)
			{
				Valid = (Instance.Component->GetOwner() != Component->GetOwner());
			}
			else if (Ignore == ETGOR_AimIgnoreEnumeration::OnlySelf)
			{
				Valid = (Instance.Component->GetOwner() == Component->GetOwner());
			}
			else
			{
				Valid = true;
			}
		}
	}
}

void UTGOR_EquipableAction::CheckMovement(UTGOR_Component* Component, bool& Valid, UTGOR_MovementComponent*& Movement) const
{
	Valid = false;

	// Unsuspend aiming
	Movement = Component->GetOwnerComponent<UTGOR_MovementComponent>();
	if (IsValid(Movement))
	{
		const FTGOR_MovementContent& MovementContent = Movement->GetContent();
		if (ContainsI(MovementContent.Movement))
		{
			Valid = true;
		}
	}
}

void UTGOR_EquipableAction::FilterAim(UTGOR_ActionComponent* Component, TSubclassOf<UTGOR_Target> Filter)
{
	// Unsuspend aiming
	UTGOR_AimComponent* AimComponent = Component->GetOwnerComponent<UTGOR_AimComponent>();
	if (IsValid(AimComponent))
	{
		AimComponent->RegisterFilter(Component, this, Filter);
	}
}

void UTGOR_EquipableAction::FixAim(UTGOR_ActionComponent* Component)
{
	UTGOR_AimComponent* AimComponent = Component->GetOwnerComponent<UTGOR_AimComponent>();
	if (IsValid(AimComponent))
	{
		AimComponent->RegisterHandle(Component, this);
	}
}

void UTGOR_EquipableAction::FreeAim(UTGOR_ActionComponent* Component)
{
	UTGOR_AimComponent* AimComponent = Component->GetOwnerComponent<UTGOR_AimComponent>();
	if (IsValid(AimComponent))
	{
		AimComponent->UnregisterHandle(this);
	}
}