// The Gateway of Realities: Planes of Existence.

#pragma once

#include "CoreMinimal.h"
#include "Realities/Mod/Trackers/TGOR_Tracker.h"
#include "TGOR_AvatarTracker.generated.h"

class ATGOR_Avatar;

/**
 * 
 */
UCLASS()
class REALITIES_API UTGOR_AvatarTracker : public UTGOR_Tracker
{
	GENERATED_BODY()
	
public:
	UTGOR_AvatarTracker();

	////////////////////////////////////////////////////////////////////////////////////////////////////
public:

	/** Returns tracker value for given avatar over given timespan */
	UFUNCTION(BlueprintImplementableEvent, BlueprintPure, Category = "TGOR Tracker", Meta = (Keywords = "C++"))
		float Track(const ATGOR_Avatar* Avatar, float DeltaSeconds) const;

	////////////////////////////////////////////////////////////////////////////////////////////////////
protected:



};
