// The Gateway of Realities: Planes of Existence.


#include "TGOR_ScienceQuery.h"
#include "KnowledgeSystem/Components/TGOR_UnlockComponent.h"
#include "InventorySystem/Content/TGOR_Science.h"

UTGOR_ScienceQuery::UTGOR_ScienceQuery()
	: Super()
{
}

TArray<UTGOR_CoreContent*> UTGOR_ScienceQuery::QueryContent() const
{
	return MigrateContentArray(Sciences);
}

void UTGOR_ScienceQuery::AssignComponent(APawn* Pawn, UTGOR_ProcessComponent* ProcessComponent, bool Completed)
{
	Sciences.Empty();
	OwnerPawn = Pawn;
	HostComponent = ProcessComponent;

	// Figure out which sciences are available to this action component
	AController* Controller = Pawn->GetController();
	if (IsValid(Controller))
	{
		UTGOR_UnlockComponent* UnlockComponent = Controller->FindComponentByClass<UTGOR_UnlockComponent>();
		if (IsValid(UnlockComponent))
		{
			// Filter sciences
			//TODO: TArray<UTGOR_Science*> Unlocked = UnlockComponent->GetUListFromType<UTGOR_Science>();
			//for (UTGOR_Science* Science : Unlocked)
			{
				/*
				if (ProcessComponent->IsCompleted(Science))
				{
					if (Completed)
					{
						Sciences.Emplace(Science);
					}
				}
				else
				{
					if (!Completed)
					{
						Sciences.Emplace(Science);
					}
				}
				*/
			}
		}
	}
}
