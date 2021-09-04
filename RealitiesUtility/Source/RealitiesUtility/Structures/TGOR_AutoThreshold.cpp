// The Gateway of Realities: Planes of Existence.


#include "TGOR_AutoThreshold.h"

FTGOR_AutoThreshold::FTGOR_AutoThreshold()
: FTGOR_AutoThreshold(0.02f)
{
}

FTGOR_AutoThreshold::FTGOR_AutoThreshold(float InitialThreshold)
:	UpdateTime(2.0f),
	Multiplier(1.4f),
	Timer(0.0f),
	Average(0.0f),
	Threshold(InitialThreshold),
	MaxThreshold(0.0001f)
{
}

bool FTGOR_AutoThreshold::Update(float Deltatime, float Error)
{
	bool Update = false;

	// Count up average over one timeupdate
	Timer += Deltatime;
	Average += Error * Deltatime;
	if (Timer >= UpdateTime)
	{
		// Compute average value
		const float Value = Average / Timer;

		// Update if over threshold
		if (Value > Threshold)
		{
			Update = true;
		}

		// Update threshold
		Threshold = FMath::Max(Value * Multiplier, MaxThreshold);
		Average = 0.0f;
		Timer = 0.0f;
	}

	return Update;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
