// The Gateway of Realities: Planes of Existence.


#include "TGOR_SelectionQuery.h"
#include "Realities/UI/TGOR_ButtonArray.h"

UTGOR_SelectionQuery::UTGOR_SelectionQuery()
	: Super(), InitialSelection(-1), Selection(-1), Parent(nullptr)
{
}

void UTGOR_SelectionQuery::CallIndex(int32 Index)
{
	Selection = Index;
	Super::CallIndex(Index);

	// Notify parent
	if (IsValid(Parent))
	{
		Parent->Select(Selection);
	}
}

void UTGOR_SelectionQuery::AssignParent(UTGOR_ButtonArray* Menu)
{
	Parent = Menu;
}

void UTGOR_SelectionQuery::InitialiseSelection(int32 Index)
{
	InitialSelection = Index;
	Selection = Index;
}

void UTGOR_SelectionQuery::Revert()
{
	CallIndex(InitialSelection);
	OnRevert();
}

void UTGOR_SelectionQuery::Commit()
{
	// Notify self
	InitialSelection = Selection;
	Super::CallIndex(Selection);
	OnCommit();

	// Notify parent
	if (IsValid(Parent))
	{
		Parent->Commit(Selection);
	}
}