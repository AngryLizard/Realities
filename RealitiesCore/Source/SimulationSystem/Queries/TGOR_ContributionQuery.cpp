// The Gateway of Realities: Planes of Existence.


#include "TGOR_ContributionQuery.h"
#include "AttributeSystem/Content/TGOR_Modifier.h"


bool FTGOR_ModifierFloatPair::operator<(const FTGOR_ModifierFloatPair& Other) const
{
	return Value < Other.Value;
}

UTGOR_ContributionQuery::UTGOR_ContributionQuery()
	: Super()
{
}

TArray<UTGOR_CoreContent*> UTGOR_ContributionQuery::QueryContent() const
{
	TArray<UTGOR_CoreContent*> List;
	for (const FTGOR_ModifierFloatPair& Instance : Instances)
	{
		List.Emplace(Instance.Content);
	}
	return List;
}

void UTGOR_ContributionQuery::AssignMap(const TMap<UTGOR_Modifier*, float>& Map, UTGOR_Attribute* Attribute)
{
	Owner = Attribute;

	Instances.Reset();
	for (const auto& Pair : Map)
	{
		FTGOR_ModifierFloatPair Instance;
		Instance.Content = Pair.Key;
		Instance.Value = Pair.Value;
		Instances.Emplace(Instance);
	}
	Instances.Sort();
}
