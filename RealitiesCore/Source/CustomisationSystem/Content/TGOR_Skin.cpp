// The Gateway of Realities: Planes of Existence.

#include "TGOR_Skin.h"

#include "CustomisationSystem/Content/TGOR_Bodypart.h"

UTGOR_Skin::UTGOR_Skin()
	: Super(),
	Normal(nullptr),
	Surface(nullptr)
{
}

////////////////////////////////////////////////////////////////////////////////////////////////////

UTGOR_Skin* UTGOR_Skin::GetBodypartSkin(UTGOR_Bodypart* Bodypart)
{
	if (IsValid(Bodypart))
	{
		if (Bodypart->Instanced_SkinInsertions.Contains(this))
		{
			return this;
		}

		for (UTGOR_Skin* Skin : Instanced_CollectionInsertions.Collection)
		{
			UTGOR_Skin* Collection = Skin->GetBodypartSkin(Bodypart);
			if (IsValid(Collection))
			{
				return Collection;
			}
		}
	}
	return nullptr;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void UTGOR_Skin::MoveInsertion(UTGOR_Content* Insertion, ETGOR_InsertionActionEnumeration Action, bool& Success)
{
	Super::MoveInsertion(Insertion, Action, Success);

	MOV_INSERTION(CollectionInsertions);
}

