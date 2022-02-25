// The Gateway of Realities: Planes of Existence.


#include "TGOR_ScienceQuery.h"
#include "Realities/Components/System/Actions/TGOR_ActionComponent.h"
#include "Realities/Components/Inventory/TGOR_ProcessComponent.h"
#include "Realities/Components/Player/TGOR_UnlockComponent.h"
#include "Realities/Mod/Processes/TGOR_Science.h"
#include "Realities/Actors/Pawns/TGOR_Pawn.h"

UTGOR_ScienceQuery::UTGOR_ScienceQuery()
	: Super()
{
}

TArray<UTGOR_Content*> UTGOR_ScienceQuery::QueryContent() const
{
	return MigrateContentArray(Sciences);
}

void UTGOR_ScienceQuery::AssignComponent(UTGOR_ActionComponent* ActionComponent, UTGOR_ProcessComponent* ProcessComponent, bool Completed)
{
	Sciences.Empty();
	OwnerComponent = ActionComponent;
	HostComponent = ProcessComponent;

	// Figure out which sciences are available to this action component
	ATGOR_Pawn* Pawn = ActionComponent->GetOwnerPawn();
	if (IsValid(Pawn))
	{
		AController* Controller = Pawn->GetController();
		if (IsValid(Controller))
		{
			UTGOR_UnlockComponent* UnlockComponent = Controller->FindComponentByClass<UTGOR_UnlockComponent>();
			if (IsValid(UnlockComponent))
			{
				// Filter sciences
				TArray<UTGOR_Science*> Unlocked = UnlockComponent->GetUListFromType<UTGOR_Science>();
				for (UTGOR_Science* Science : Unlocked)
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
}
