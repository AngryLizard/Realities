// The Gateway of Realities: Planes of Existence.


#include "TGOR_ConversionQuery.h"

#include "SimulationSystem/Components/TGOR_StatComponent.h"
#include "SimulationSystem/Content/TGOR_Response.h"
#include "DimensionSystem/Content/TGOR_Energy.h"
#include "SimulationSystem/Content/TGOR_Stat.h"


UTGOR_ConversionQuery::UTGOR_ConversionQuery()
{

}


TArray<UTGOR_CoreContent*> UTGOR_ConversionQuery::QueryContent() const
{
	TArray<UTGOR_CoreContent*> Output;
	for (const FTGOR_ConversionOutput& Conversion : Conversions)
	{
		Output.Emplace(Conversion.Energy);
	}
	return Output;
}


void UTGOR_ConversionQuery::AssignComponent(UTGOR_Stat* Stat)
{
	Conversions.Empty();

	const TArray<UTGOR_Response*>& Responses = Stat->Instanced_ResponseInsertions.Collection;//->GetIListFromType<UTGOR_Response>();
	for (UTGOR_Response* Response : Responses)
	{
		for (const auto& Pair : Response->ConversionValues)
		{
			FTGOR_ConversionOutput Sample;
			Sample.Energy = Pair.Key;
			Sample.Rate = Pair.Value.Rate;
			Sample.Quantity = Pair.Value.Quantity;
			Conversions.Emplace(Sample);
		}
	}
}
