// The Gateway of Realities: Planes of Existence.


#include "TGOR_StatsQuery.h"

#include "SimulationSystem/Components/TGOR_StatComponent.h"
#include "SimulationSystem/Content/TGOR_Stat.h"


UTGOR_StatsQuery::UTGOR_StatsQuery()
: Super()
{
	Required = true;
}

TArray<UTGOR_CoreContent*> UTGOR_StatsQuery::QueryContent() const
{
	TArray<UTGOR_CoreContent*> Output;
	for (const FTGOR_StatOutput& Stat : Stats)
	{
		Output.Emplace(Stat.Stat);
	}
	return Output;
}

void UTGOR_StatsQuery::AssignComponent(UTGOR_StatComponent* StatComponent, ETGOR_StatTypeEnumeration Filter)
{
	Stats = StatComponent->GetStats();

	// Filter
	Stats.FilterByPredicate([Filter](const FTGOR_StatOutput& Output)->bool { return Output.Stat->Type <= Filter; });

	// Sort by types
	Stats.Sort([](const FTGOR_StatOutput& A, const FTGOR_StatOutput& B)->bool { return A.Stat->Type <= B.Stat->Type; });
}