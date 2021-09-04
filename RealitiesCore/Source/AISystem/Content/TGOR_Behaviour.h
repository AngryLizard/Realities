// The Gateway of Realities: Planes of Existence.

#pragma once
#include "MovementSystem/TGOR_MovementInstance.h"

#include "CoreSystem/Content/TGOR_CoreContent.h"
#include "TGOR_Behaviour.generated.h"

class ATGOR_NpcController;


/**
 * 
 */
UCLASS(Blueprintable)
class AISYSTEM_API UTGOR_Behaviour : public UTGOR_CoreContent
{
	GENERATED_BODY()
	
public:
	UTGOR_Behaviour();

	////////////////////////////////////////////////////////////////////////////////////////////////////

	/** Update behaviour for this tick */
	UFUNCTION(BlueprintImplementableEvent, Category = "!TGOR NPC", Meta = (Keywords = "C++"))
		void UpdateBehaviour(ATGOR_NpcController* Controller, FVector& Input, FQuat& View);

	////////////////////////////////////////////////////////////////////////////////////////////////////

};
