// The Gateway of Realities: Planes of Existence.

#include "TGOR_Mask.h"

#include "CustomisationSystem/Content/TGOR_Bodypart.h"

UTGOR_Mask::UTGOR_Mask()
	: Super(),
	Texture(nullptr)
{
}

////////////////////////////////////////////////////////////////////////////////////////////////////

UTGOR_Mask* UTGOR_Mask::GetBodypartMask(UTGOR_Bodypart* Bodypart)
{
	if (IsValid(Bodypart))
	{
		if (Bodypart->Instanced_MaskInsertions.Contains(this))
		{
			return this;
		}

		for (UTGOR_Mask* Mask : Instanced_CollectionInsertions.Collection)
		{
			UTGOR_Mask* Collection = Mask->GetBodypartMask(Bodypart);
			if (IsValid(Collection))
			{
				return Collection;
			}
		}
	}
	return nullptr;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void UTGOR_Mask::MoveInsertion(UTGOR_Content* Insertion, ETGOR_InsertionActionEnumeration Action, bool& Success)
{
	Super::MoveInsertion(Insertion, Action, Success);

	MOV_INSERTION(CollectionInsertions);
}

