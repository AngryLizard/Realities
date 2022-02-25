// The Gateway of Realities: Planes of Existence.


#include "TGOR_ProcessQuery.h"
#include "Realities/Components/Inventory/TGOR_ProcessComponent.h"
#include "Realities/Components/System/Actions/TGOR_ActionComponent.h"
#include "Realities/Base/Controller/TGOR_OnlineController.h"
#include "Realities/Actors/Pawns/TGOR_Pawn.h"

#include "Realities/Components/Player/TGOR_UnlockComponent.h"
#include "Realities/Mod/Spawner/Items/TGOR_Item.h"
#include "Realities/Mod/Processes/TGOR_Science.h"

#include "Realities/Base/TGOR_Singleton.h"
#include "Realities/Base/Content/TGOR_ContentManager.h"

UTGOR_ProcessQuery::UTGOR_ProcessQuery()
	: Super(),
	OwnerComponent(nullptr),
	HostComponent(nullptr)
{
}

TArray<UTGOR_Content*> UTGOR_ProcessQuery::QueryContent() const
{
	return MigrateContentArray(Processes);
}


void UTGOR_ProcessQuery::AssignComponent(UTGOR_ActionComponent* ActionComponent, UTGOR_ProcessComponent* ProcessComponent, TSubclassOf<UTGOR_Process> Type, bool Match)
{
	OwnerComponent = ActionComponent;
	HostComponent = ProcessComponent;
	RecipeMatch = Match;


	SINGLETON_CHK;
	UTGOR_ContentManager* ContentManager = Singleton->GetContentManager();

	// Clear state
	Processes.Reset();

	// Get inventory of provided ActionComponent
	if (IsValid(OwnerComponent) && IsValid(HostComponent))
	{
		// Get unlocked items
		ATGOR_Pawn* Pawn = ActionComponent->GetOwnerPawn();
		ATGOR_OnlineController* OnlineController = Cast<ATGOR_OnlineController>(Pawn->Controller);
		const TArray<UTGOR_Process*> Unlocks = IsValid(OnlineController) ? OnlineController->GetUnlock()->GetUListFromType<UTGOR_Process>(Type) : ContentManager->GetTListFromType<UTGOR_Process>(Type);
		for (UTGOR_Process* Process : Unlocks)
		{
			// Split into categories
			if (Process->CanProcess(HostComponent, nullptr))
			{
				if (Match) Processes.Emplace(Process);
			}
			else
			{
				if(!Match) Processes.Emplace(Process);
			}

		}
	}
}
