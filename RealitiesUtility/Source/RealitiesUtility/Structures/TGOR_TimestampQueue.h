// The Gateway of Realities: Planes of Existence.

#pragma once

#include "CoreMinimal.h"
#include "RealitiesUtility/Structures/TGOR_Time.h"

#define STANDARD_TIMESTAMP_QUEUE CTGOR_TimestampQueue<float, 8>

/**
 * 
 */
template<typename K, int N>
class REALITIESUTILITY_API CTGOR_TimestampQueue
{
private:
	struct CTGOR_TimestampTuple
	{
		FTGOR_Time Timestamp;
		K Value;
	};

	// Actual table length
	static const int Len = 1 << N;

	// Static list
	CTGOR_TimestampTuple Elements[Len];
	int Read;
	int Write;

public:
	CTGOR_TimestampQueue()
	{
		Write = Read = 0;
		memset(Elements, 0, Len * sizeof(CTGOR_TimestampTuple));
	}

	virtual ~CTGOR_TimestampQueue()
	{
	}

	// Add an entry at the end of the list
	void AddEntry(FTGOR_Time Timestamp, K Value)
	{
		// Make sure timestamps are well ordered
		FTGOR_Time WriteTime = Elements[Write].Timestamp;
		if (WriteTime > Timestamp)
		{
			Timestamp = WriteTime;
		}
		
		// Increase and wrap around write pointer
		Write = (Write + 1) % Len;

		// Increase read pointer if overlap happened
		if (Read == Write)
		{
			Read = (Read + 1) % Len;
		}

		// Set tuple at new write position
		CTGOR_TimestampTuple& Tuple = Elements[Write];
		Tuple.Timestamp = Timestamp;
		Tuple.Value = Value;
	}

	// Increase read pointer until write has been reached
	bool IncreaseReadPointer(int& Pointer)
	{
		// Compute next pointer
		int Next = (Pointer + 1) % Len;

		// Make sure not to exceed write pointer
		if (Next == Write)
		{
			return(false);
		}

		// Increase read pointer
		Pointer = Next;
		return(true);
	}

	// Check value at given timestamp and update values above it given a range, return last adapted value
	K Reconcile(FTGOR_Time Timestamp, K Value, K Min, K Max)
	{
		// Find first element whose timestamp is in range
		while (Elements[Read].Timestamp < Timestamp)
		{
			// Make sure not to exceed write pointer
			if (!IncreaseReadPointer(Read)) break;
		}

		// Get difference to adapt
		float Diff = Value - Elements[Read].Value;


		// Set up temporary read pointer
		int Pointer = Read;

		// Read up to write pointer and adapt elements
		do
		{
			// Get affected element
			CTGOR_TimestampTuple& Tuple = Elements[Pointer];
			
			// Make sure value is in range
			Value = Tuple.Value + Diff;
			Value = FMath::Clamp(Value, Min, Max);

			// Change diff in case value got clamped
			Diff = Value - Tuple.Value;

			// Set new value
			Tuple.Value = Value;

		} while (IncreaseReadPointer(Pointer));

		return(Value);
	}
};
