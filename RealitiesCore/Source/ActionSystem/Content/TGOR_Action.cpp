// The Gateway of Realities: Planes of Existence.


#include "TGOR_Action.h"

#include "TargetSystem/Content/TGOR_Target.h"

#include "InventorySystem/Content/TGOR_Item.h"

#include "MovementSystem/Content/TGOR_Movement.h"

#include "ActionSystem/Content/TGOR_Input.h"
#include "ActionSystem/Content/Events/TGOR_Event.h"

#include "../Components/TGOR_ActionComponent.h"
#include "../Tasks/TGOR_ActionTask.h"

void UTGOR_Action::LogActionMessage(const FString& Message, UTGOR_ActionComponent* Component) const
{
	if (Verbose)
	{
		const FString Name = GetDefaultName();
		if (IsValid(Component) && Component->Verbose)
		{
			AActor* Owner = Component->GetOwner();
			const FString OwnerName = Owner->GetName();
			const FString OnwerRole = 
				Component->GetOwnerRole() == ENetRole::ROLE_Authority ? "Server" : 
				Component->GetOwnerRole() == ENetRole::ROLE_AutonomousProxy ? "Client" : "Simulated";
			UE_LOG(TGOR_ActionLogs, Log, TEXT("%s / %s - %s: %s"), *OnwerRole, *OwnerName, *Name, *Message);
		}
		else
		{
			//UE_LOG(TGOR_ActionLogs, Log, TEXT("None - %s: %s"), *Name, *Message);
		}
	}
}


UTGOR_Action::UTGOR_Action()
	: Verbose(false),
	ScheduleCooldown(0.0f),
	ClientAuthority(ETGOR_ClientAuthority::NoAuthority),
	NetworkNotify(ETGOR_NotifyReplication::NoValidation),
	SupportedItem(),
	AimCheck(ETGOR_AimCheckEnumeration::None),
	AimTarget(ETGOR_AimIgnoreEnumeration::None),
	AimRange(ETGOR_AimDistanceEnumeration::NoRange),
	AutoTrigger(false),
	MaxPrepareTime(0.0f),
	MaxFinishTime(0.0f)
{
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

UTGOR_ActionTask* UTGOR_Action::CreateActionTask(UTGOR_ActionComponent* Component, int32 SlotIdentifier)
{
	return UTGOR_Task::CreateTask(this, TaskType, Component, SlotIdentifier);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool UTGOR_Action::HasClientAuthority() const
{
	return ClientAuthority > ETGOR_ClientAuthority::NoAuthority;
}

bool UTGOR_Action::HasTargetCondition() const
{
	return AimTarget != ETGOR_AimIgnoreEnumeration::None;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void UTGOR_Action::TaskInitialise(UTGOR_ActionTask* ActionTask)
{
	OnTaskInitialise(ActionTask);
}


////////////////////////////////////////////////////////////////////////////////////////////////////

void UTGOR_Action::MoveInsertion(UTGOR_Content* Insertion, ETGOR_InsertionActionEnumeration Action, bool& Success)
{
	Super::MoveInsertion(Insertion, Action, Success);

	MOV_INSERTION(InputInsertions);
	MOV_INSERTION(EventInsertions);
	MOV_INSERTION(ChildInsertions);
	MOV_INSERTION(TargetInsertions);
	MOV_INSERTION(MovementInsertions);
	MOV_INSERTION(PrepareMovementInsertion);
	MOV_INSERTION(OperateMovementInsertion);
	MOV_INSERTION(FinishMovementInsertion);
}