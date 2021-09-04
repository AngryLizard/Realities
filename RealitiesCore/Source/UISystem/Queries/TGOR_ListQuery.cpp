// The Gateway of Realities: Planes of Existence.


#include "TGOR_ListQuery.h"


UTGOR_ListQuery::UTGOR_ListQuery()
	: Super()
{

}

TArray<FTGOR_Display> UTGOR_ListQuery::QueryDisplays() const
{
	return(OnQueryDisplays());
}

void UTGOR_ListQuery::CallIndex(int32 Index)
{
	OnCallIndex(Index);
}