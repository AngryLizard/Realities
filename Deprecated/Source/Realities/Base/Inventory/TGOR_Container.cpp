#include "TGOR_Container.h"

#include "Realities/Utility/Storage/TGOR_Serialisation.h"
#include "Realities/Utility/Storage/TGOR_Package.h"
#include "Realities/Base/TGOR_Singleton.h"
#include "Realities/Base/TGOR_GameState.h"
#include "Realities/Mod/Matters/TGOR_Matter.h"

FString UTGOR_Container::Print() const
{
	return FString("Dirty = ") + (Dirty == ETGOR_DirtEnumeration::None ? "f" : (Dirty == ETGOR_DirtEnumeration::Dirty ? "t" : "+t"));
}

bool UTGOR_Container::Compatible(const UTGOR_Container* Other) const
{
	if (this == Other) return true;
	if (!IsValid(Other)) return false;
	if (GetClass() != Other->GetClass()) return false;
	return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void UTGOR_Container::CombineMissingOnto(TMap<UTGOR_Matter*, int32>& Missing) const
{
	const TMap<UTGOR_Matter*, int32>& Combine = GetMissing();
	for (const auto& Pair : Combine)
	{
		Missing.FindOrAdd(Pair.Key) += Pair.Value;
	}
}

void UTGOR_Container::CombineCompositionOnto(TMap<UTGOR_Matter*, int32>& Composition) const
{
	const TMap<UTGOR_Matter*, int32>& Combine = GetComposition();
	for (const auto& Pair : Combine)
	{
		Composition.FindOrAdd(Pair.Key) += Pair.Value;
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////

bool UTGOR_Container::PurgeDirt()
{
	if (Dirty > ETGOR_DirtEnumeration::None)
	{
		Dirty = ETGOR_DirtEnumeration::None;
		return true;
	}
	return false;
}

bool UTGOR_Container::HasNotify()
{
	return Dirty == ETGOR_DirtEnumeration::Notify;
}

void UTGOR_Container::MarkDirty(bool Notify)
{
	Dirty = ETGOR_DirtEnumeration::Dirty;
	if (Notify)
	{
		Dirty = ETGOR_DirtEnumeration::Notify;
	}
}
