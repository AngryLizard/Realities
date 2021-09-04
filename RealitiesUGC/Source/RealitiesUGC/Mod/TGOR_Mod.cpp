// The Gateway of Realities: Planes of Existence. By Salireths.


#include "TGOR_Mod.h"


UTGOR_Mod::UTGOR_Mod(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer),
	CoreOnly(false)
{
	Entry.Name = L"Mod";
	Entry.Version = 0;
}

FString UTGOR_Mod::GetModIdentifier() const
{
	return Entry.Name + "-" + FString::FromInt(Entry.Version);
}

bool UTGOR_Mod::HasDependency(const UTGOR_Mod* Mod) const
{
	for (const TSubclassOf<UTGOR_Mod>& Dependency : Dependencies)
	{
		if (Mod->IsA(Dependency))
		{
			return true;
		}
	}
	return false;
}
