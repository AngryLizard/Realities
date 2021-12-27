// The Gateway of Realities: Planes of Existence.

#pragma once

#include "CoreSystem/Content/TGOR_CoreContent.h"
#include "TGOR_Performance.generated.h"


/**
 * 
 */
UCLASS(Blueprintable)
class ANIMATIONSYSTEM_API UTGOR_Performance : public UTGOR_CoreContent
{
	GENERATED_BODY()
	
public:
	UTGOR_Performance();

	////////////////////////////////////////////////////////////////////////////////////////////////////
public:

	/** AnimBP slot identifier */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "!TGOR Movement")
		FString SubAnimIdentifier = "Default";

};
