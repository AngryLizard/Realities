// The Gateway of Realities: Planes of Existence.


#include "TGOR_Direction.h"

FTGOR_Direction::FTGOR_Direction()
	: Magnitude(0.0f), Direction(FVector::ZeroVector)
{
}

FTGOR_Direction::FTGOR_Direction(const FVector& Vector)
	: Direction(FVector::ZeroVector)
{
	Magnitude = Vector.Size();
	if (Magnitude > KINDA_SMALL_NUMBER)
	{
		Direction = Vector / Magnitude;
	}
}

FTGOR_Direction::operator bool() const
{
	return(Magnitude > KINDA_SMALL_NUMBER);
}