// The Gateway of Realities: Planes of Existence.

#include "TGOR_Buffer.h"


FTGOR_Buffer::FTGOR_Buffer()
{
}


void FTGOR_Buffer::Clear()
{
	Data.Empty();
}

bool FTGOR_Buffer::HasData() const
{
	return(Data.Num() > 0);
}

bool FTGOR_Buffer::operator!=(const FTGOR_Buffer& Buffer) const
{
	return(!(*this == Buffer));
}

bool FTGOR_Buffer::operator==(const FTGOR_Buffer& Buffer) const
{
	if (Data.Num() != Buffer.Data.Num()) return(false);
	return(memcmp(Data.GetData(), Buffer.Data.GetData(), Data.Num() * sizeof(int8)) == 0);
}

