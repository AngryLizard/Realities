// The Gateway of Realities: Planes of Existence.


#include "TGOR_RegisterComponent.h"
#include "Realities/Actors/Pawns/TGOR_Pawn.h"
#include "Realities/Base/Controller/TGOR_OnlineController.h"

#include "Realities/Utility/Storage/TGOR_Package.h"
#include "Realities/Base/Content/TGOR_ContentManager.h"
#include "Realities/Base/TGOR_Singleton.h"

UTGOR_RegisterComponent::UTGOR_RegisterComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void UTGOR_RegisterComponent::BeginPlay()
{
	Super::BeginPlay();

}

void UTGOR_RegisterComponent::Interact(UTGOR_ActionStorage* Storage, UTGOR_ActionComponent* Component, const FTGOR_SlotIndex& Slot)
{
	Super::Interact(Storage, Component, Slot);

	// Server only
	if (IsValid(Component) && Component->IsServer())
	{
		ATGOR_Pawn* Pawn = Component->GetOwnerPawn();
		ATGOR_OnlineController* Controller = Cast<ATGOR_OnlineController>(Pawn->GetController());
		if (IsValid(Controller))
		{
			const int32 UserKey = Controller->GetActiveUserKey();
			RegisteredPlayerKeys.Emplace(UserKey);
		}
	}
}

bool UTGOR_RegisterComponent::IsSupported(UTGOR_ActionComponent* Component) const
{
	if (IsValid(Component))
	{
		ATGOR_Pawn* Pawn = Component->GetOwnerPawn();
		ATGOR_OnlineController* Controller = Cast<ATGOR_OnlineController>(Pawn->GetController());
		if (IsValid(Controller))
		{
			const int32 UserKey = Controller->GetActiveUserKey();
			return !RegisteredPlayerKeys.Contains(Controller->GetActiveUserKey());
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
