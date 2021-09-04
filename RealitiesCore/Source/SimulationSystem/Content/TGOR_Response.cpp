// The Gateway of Realities: Planes of Existence.
#include "TGOR_Response.h"
#include "SimulationSystem/Content/TGOR_Stat.h"
#include "DimensionSystem/Content/TGOR_Energy.h"
#include "SimulationSystem/Components/TGOR_StatComponent.h"
#include "RealitiesUGC/Mod/TGOR_ContentManager.h"
#include "CoreSystem/TGOR_Singleton.h"

FTGOR_Conversion::FTGOR_Conversion()
:	Quantity(0),
	Rate(0.0f)
{
}

UTGOR_Response::UTGOR_Response()
	: Super()
{
}

void UTGOR_Response::PostBuildResource()
{
	Super::PostBuildResource();

	SINGLETON_CHK;
	UTGOR_ContentManager* ContentManager = Singleton->GetContentManager();

	// Generate composition
	ConversionValues.Reset();
	for (auto& Pair : Conversions)
	{
		UTGOR_Energy* Content = ContentManager->GetTFromType<UTGOR_Energy>(Pair.Key);
		if (IsValid(Content))
		{
			ConversionValues.Add(Content, Pair.Value);
		}
	}
}

int32 UTGOR_Response::Convert_Implementation(UTGOR_StatComponent* Stats, UTGOR_Segment* Segment, UTGOR_Energy* Energy, int32 Quantity, float Value, float& Delta) const
{
	Delta = 0.0f;

	const FTGOR_Conversion* Ptr = ConversionValues.Find(Energy);
	if (Ptr && FMath::Square(Ptr->Rate) >= SMALL_NUMBER)
	{
		// Get amount we can process
		const float MaxValue = (Ptr->Rate > 0.0f ? 1.0f : 0.0f) - (Value + Delta);
		const int32 Count = FMath::Min(FMath::FloorToInt(MaxValue / Ptr->Rate), Quantity);

		// Process amount
		Delta += Count * Ptr->Rate;
		Quantity -= Count;
	}
	return Quantity;
}