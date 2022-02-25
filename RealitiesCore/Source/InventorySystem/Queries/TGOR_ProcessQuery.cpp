// The Gateway of Realities: Planes of Existence.


#include "TGOR_ProcessQuery.h"

#include "InventorySystem/Content/TGOR_Item.h"
#include "InventorySystem/Content/TGOR_Process.h"
#include "InventorySystem/Components/TGOR_ProcessComponent.h"

#include "CoreSystem/TGOR_Singleton.h"
#include "RealitiesUGC/Mod/TGOR_ContentManager.h"

UTGOR_ProcessQuery::UTGOR_ProcessQuery()
	: Super(),
	OwnerPawn(nullptr),
	HostComponent(nullptr)
{
}

TArray<UTGOR_CoreContent*> UTGOR_ProcessQuery::QueryContent() const
{
	return MigrateContentArray(Processes);
}


void UTGOR_ProcessQuery::AssignComponent(APawn* Pawn, UTGOR_ProcessComponent* ProcessComponent, TSubclassOf<UTGOR_Process> Type, bool Match)
{
	OwnerPawn = Pawn;
	HostComponent = ProcessComponent;
	RecipeMatch = Match;


	SINGLETON_CHK;
	UTGOR_ContentManager* ContentManager = Singleton->GetContentManager();

	// Clear state
	Processes.Reset();

	// Get inventory of provided ActionComponent
	if (IsValid(OwnerPawn) && IsValid(HostComponent))
	{
		// Get unlocked items
		const TArray<UTGOR_Process*> Unlocks = ContentManager->GetTListFromType<UTGOR_Process>(Type); // TODO: OnlineController->GetUnlock()->GetUListFromType<UTGOR_Process>(Type)
		for (UTGOR_Process* Process : Unlocks)
		{
			/*
			// Split into categories
			if (Process->CanProcess(HostComponent, nullptr))
			{
				if (Match) Processes.Emplace(Process);
			}
			else
			{
				if(!Match) Processes.Emplace(Process);
			}
			*/
		}
	}
}
