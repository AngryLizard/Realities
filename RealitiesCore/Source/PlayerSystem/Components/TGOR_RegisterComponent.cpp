// The Gateway of Realities: Planes of Existence.


#include "TGOR_RegisterComponent.h"
#include "PlayerSystem/Gameplay/TGOR_OnlineController.h"
#include "ActionSystem/Tasks/TGOR_ActionTask.h"
#include "ActionSystem/Components/TGOR_ActionComponent.h"

#include "CoreSystem/Storage/TGOR_Package.h"
#include "CoreSystem/TGOR_Singleton.h"

UTGOR_RegisterComponent::UTGOR_RegisterComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void UTGOR_RegisterComponent::BeginPlay()
{
	Super::BeginPlay();

}

void UTGOR_RegisterComponent::RegisterInteract(UTGOR_ActionTask* ActionSlot, int32 Index)
{
	Super::RegisterInteract(ActionSlot, Index);

	// Server only
	if (IsValid(ActionSlot) && ActionSlot->Identifier.Component->IsServer())
	{
		APawn* Pawn = ActionSlot->Identifier.Component->GetOuterAPawn();
		if (IsValid(Pawn))
		{
			ATGOR_OnlineController* Controller = Cast<ATGOR_OnlineController>(Pawn->GetController());
			if (IsValid(Controller))
			{
				const int32 UserKey = Controller->GetActiveUserKey();
				RegisteredPlayerKeys.Emplace(UserKey);
			}
		}
	}
}

bool UTGOR_RegisterComponent::IsSupported(UTGOR_ActionComponent* Component) const
{
	if (IsValid(Component))
	{
		APawn* Pawn = Component->GetOuterAPawn();
		if (IsValid(Pawn))
		{
			ATGOR_OnlineController* Controller = Cast<ATGOR_OnlineController>(Pawn->GetController());
			if (IsValid(Controller))
			{
				const int32 UserKey = Controller->GetActiveUserKey();
				return !RegisteredPlayerKeys.Contains(Controller->GetActiveUserKey());
			}
		}
	}
	return false;
}


void UTGOR_RegisterComponent::Write_Implementation(FTGOR_GroupWritePackage& Package, UTGOR_Singleton* Context) const
{
	Package.WriteEntry("Register", RegisteredPlayerKeys);
}

bool UTGOR_RegisterComponent::Read_Implementation(FTGOR_GroupReadPackage& Package, UTGOR_Singleton* Context)
{
	return Package.ReadEntry("Register", RegisteredPlayerKeys);
}
