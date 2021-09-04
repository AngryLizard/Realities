// The Gateway of Realities: Planes of Existence.


#include "TGOR_ContentQuery.h"
#include "CoreSystem/Content/TGOR_CoreContent.h"

UTGOR_ContentQuery::UTGOR_ContentQuery()
	: Super(), Required(true)
{
}

TArray<FTGOR_Display> UTGOR_ContentQuery::QueryDisplays() const
{
	const TArray<UTGOR_CoreContent*> Contents = QueryContent();

	TArray<FTGOR_Display> Displays;
	if (!Required)
	{
		Displays.Add(NoneDisplay);
	}

	for (UTGOR_CoreContent* Content : Contents)
	{
		if (IsValid(Content))
		{
			Displays.Emplace(Content->GetDisplay());
		}
		else
		{
			Displays.Emplace(EmptyDisplay);
		}
	}
	return(Displays);
}

void UTGOR_ContentQuery::CallIndex(int32 Index)
{
	Super::CallIndex(Index);
	if (Required)
	{
		CallContent(Index);
	}
	else
	{
		if (Index == 0)
		{
			CallNone();
		}
		else
		{
			CallContent(Index - 1);
		}
	}
}

void UTGOR_ContentQuery::Commit()
{
	Super::Commit();
	if (Required)
	{
		CommitContent(Selection);
	}
	else
	{
		if (Selection == 0)
		{
			CommitNone();
		}
		else
		{
			CommitContent(Selection - 1);
		}
	}
}


TArray<UTGOR_CoreContent*> UTGOR_ContentQuery::QueryContent() const
{
	return(OnQueryContent());
}

void UTGOR_ContentQuery::CallContent(int32 Index)
{
	OnCallContent(Index);
}

void UTGOR_ContentQuery::CallNone()
{
	OnCallNone();
}

void UTGOR_ContentQuery::CommitContent(int32 Index)
{
	OnCommitContent(Index);
}

void UTGOR_ContentQuery::CommitNone()
{
	OnCommitNone();
}

