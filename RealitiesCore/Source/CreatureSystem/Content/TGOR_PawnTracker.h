// The Gateway of Realities: Planes of Existence.

#pragma once

#include "CoreMinimal.h"
#include "KnowledgeSystem/Content/TGOR_Tracker.h"
#include "TGOR_PawnTracker.generated.h"

class APawn;

/**
 * 
 */
UCLASS()
class CREATURESYSTEM_API UTGOR_PawnTracker : public UTGOR_Tracker
{
	GENERATED_BODY()
	
public:
	UTGOR_PawnTracker();

	////////////////////////////////////////////////////////////////////////////////////////////////////
public:

	/** Returns tracker value for given pawn over given timespan */
	UFUNCTION(BlueprintImplementableEvent, BlueprintPure, Category = "!TGOR Tracker", Meta = (Keywords = "C++"))
		float Track(const APawn* Pawn, float DeltaSeconds) const;

	////////////////////////////////////////////////////////////////////////////////////////////////////
protected:



};
