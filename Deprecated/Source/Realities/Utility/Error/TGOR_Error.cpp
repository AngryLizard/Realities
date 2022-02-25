// The Gateway of Realities: Planes of Existence.

#include "TGOR_Error.h"
#include "Engine.h"

#include <cassert>


void UTGOR_Error::Error(FString Message, FString Location, FString File, FString Line, ETGOR_ErrorMode Error)
{
	if (IsValid(GEngine))
	{
		FColor Colour;
		switch (Error)
		{
		case ETGOR_ErrorMode::Info: Colour = FColor::White; break;
		case ETGOR_ErrorMode::Warning: Colour = FColor::Yellow; break;
		case ETGOR_ErrorMode::Assert: Colour = FColor::Cyan; break;
		case ETGOR_ErrorMode::Error: Colour = FColor::Red; break;
		case ETGOR_ErrorMode::Fatal: Colour = FColor::Black; break;
		}

		if (Error != ETGOR_ErrorMode::Assert)
		{
			GEngine->AddOnScreenDebugMessage(-1, 20.0f, Colour, Message + " at " + Location);
			if (Error >= ETGOR_ErrorMode::Error)
			{
				GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor(0xFF804040), File + " | " + Line);
			}
		}
	}

	UE_LOG(LogTemp, Warning, TEXT("%s"), *(Message + " at " + Location + " - " + File + " | " + Line));
	assert(Error != ETGOR_ErrorMode::Fatal);
}


void UTGOR_Error::Report(FString Message, float Time)
{
	GEngine->AddOnScreenDebugMessage(-1, Time, FColor(0xFF202020), Message);
	UE_LOG(LogTemp, Warning, TEXT("%s"), *Message);
}

void UTGOR_Error::DebugTrace(UWorld* World, FVector a, FVector b, FColor Color)
{
	FHitResult HitOut;
	FCollisionQueryParams Params;
	DrawDebugLine(World, a, b, Color, true, 2.0f, 0, 5);
}