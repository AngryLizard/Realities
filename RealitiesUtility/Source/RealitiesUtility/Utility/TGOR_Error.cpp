// The Gateway of Realities: Planes of Existence.

#include "TGOR_Error.h"
#include "Engine.h"

#include <cassert>


void UTGOR_Error::Error(FString Message, FString Location, FString File, FString Line, ETGOR_ErrorMode Error)
{
#if WITH_EDITOR
	if (IsValid(GEngine))
	{
		FColor Color;
		switch (Error)
		{
		case ETGOR_ErrorMode::Info: Color = FColor::White; break;
		case ETGOR_ErrorMode::Warning: Color = FColor::Yellow; break;
		case ETGOR_ErrorMode::Assert: Color = FColor::Cyan; break;
		case ETGOR_ErrorMode::Error: Color = FColor::Red; break;
		case ETGOR_ErrorMode::Fatal: Color = FColor::Black; break;
		}

		if (Error != ETGOR_ErrorMode::Assert)
		{
			GEngine->AddOnScreenDebugMessage(-1, 20.0f, Color, Message + " at " + Location);
			if (Error >= ETGOR_ErrorMode::Error)
			{
				GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor(0xFF804040), File + " | " + Line);
			}
		}
	}
#endif

	UE_LOG(LogTemp, Warning, TEXT("%s"), *(Message + " at " + Location + " - " + File + " | " + Line));
	assert(Error != ETGOR_ErrorMode::Fatal);
}


void UTGOR_Error::Report(FString Message, float Time)
{
#if WITH_EDITOR
	GEngine->AddOnScreenDebugMessage(-1, Time, FColor(0xFFE0E0E0), Message);
#endif
	UE_LOG(LogTemp, Warning, TEXT("%s"), *Message);
}

void UTGOR_Error::DebugTrace(UWorld* World, FVector a, FVector b, FColor Color)
{
#if WITH_EDITOR
	FHitResult HitOut;
	FCollisionQueryParams Params;
	DrawDebugLine(World, a, b, Color, true, 2.0f, 0, 5);
#endif
}

void UTGOR_Error::ThrowException(UObject* Object, FString Message)
{
#if WITH_EDITOR
	FString ScriptStack;
	FScriptExceptionHandler::Get().HandleException(ELogVerbosity::Error, *Message, *ScriptStack);
#endif
}
