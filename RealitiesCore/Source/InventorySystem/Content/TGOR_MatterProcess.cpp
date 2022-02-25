// The Gateway of Realities: Planes of Existence.

#include "TGOR_MatterProcess.h"
#include "InventorySystem/Tasks/TGOR_ProcessTask.h"

UTGOR_MatterProcess::UTGOR_MatterProcess()
	: Super()
{
}


float UTGOR_MatterProcess::ComputeCompositionMass(const TMap<UTGOR_Matter*, int32>& Composition)
{
	float Mass = 0.0f;
	for (const auto& Pair : Composition)
	{
		Mass += Pair.Value * Pair.Key->Mass;
	}
	return Mass;
}

TMap<UTGOR_Matter*, int32> UTGOR_MatterProcess::ComputeCompositionFilter(const TMap<UTGOR_Matter*, int32>& Composition, const TMap<UTGOR_Matter*, int32>& Filter)
{
	TMap<UTGOR_Matter*, int32> Output;
	for (const auto& Pair : Filter)
	{
		if (const int32* Ptr = Composition.Find(Pair.Key))
		{
			Output.Add(Pair.Key, FMath::Min(Pair.Value, *Ptr));
		}
	}
	return Output;
}

bool UTGOR_MatterProcess::IsCompositionCompatible(const TMap<UTGOR_Matter*, int32>& Composition, const TMap<UTGOR_Matter*, int32>& Filter)
{
	for (const auto& Pair : Filter)
	{
		if (Pair.Value > 0)
		{
			if (const int32* Ptr = Composition.Find(Pair.Key))
			{
				// Check whether what 
				if ((*Ptr) > 0)
				{
					return true;
				}
			}
		}
	}
	return false;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

TMap<UTGOR_Matter*, int32> UTGOR_MatterProcess::MatterRationing(const TMap<UTGOR_Matter*, int32>& Composition, float Antimatter)
{

	TArray<UTGOR_Matter*> Keys;
	Composition.GenerateKeyArray(Keys);

	// Randomly permute keys
	const int32 Last = Keys.Num() - 1;
	for (int32 i = 0; i < Last; i++)
	{
		Keys.Swap(i, MatterPermutator.RandRange(i, Last));
	}

	TMap<UTGOR_Matter*, int32> Output;
	for (UTGOR_Matter* Key : Keys)
	{
		if (Key->Mass >= SMALL_NUMBER)
		{
			const int32 Process = FMath::FloorToInt(Antimatter / Key->Mass);
			const int32 Quantity = FMath::Min(Process, Composition[Key]);
			if (Quantity > 0)
			{
				Output.Add(Key, Quantity);
				Antimatter -= Quantity * Key->Mass;
			}
		}
	}
	return Output;
}