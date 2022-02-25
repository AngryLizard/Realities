// The Gateway of Realities: Planes of Existence.
#include "TGOR_Tracker.h"

////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////

#define LOCTEXT_NAMESPACE "Tracker"

UTGOR_Tracker::UTGOR_Tracker()
	: Super()
{
	DefaultKeyword = LOCTEXT("DefaultKeyword", "With");
	Unit = LOCTEXT("DefaultUnit", "s");
}

#undef LOCTEXT_NAMESPACE

////////////////////////////////////////////////////////////////////////////////////////////////////

FText UTGOR_Tracker::GetKeyword(UTGOR_Content* Content) const
{
	FText Out = DefaultKeyword;
	if (IsValid(Content))
	{
		UClass* BestFit = UTGOR_Content::StaticClass();
		for (const TPair<TSubclassOf<UTGOR_Content>, FText>& Pair : Keywords)
		{
			if (Pair.Key->IsChildOf(BestFit) && Content->IsA(Pair.Key))
			{
				BestFit = Pair.Key;
				Out = Pair.Value;
			}
		}
	}
	return Out;
}