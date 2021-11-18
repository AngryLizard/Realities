// The Gateway of Realities: Planes of Existence.

#pragma once

#include "AttributeSystem/Content/TGOR_Modifier.h"
#include "TGOR_Effect.generated.h"

class UTGOR_Stat;

/**
 * 
 */
UCLASS()
class SIMULATIONSYSTEM_API UTGOR_Effect : public UTGOR_Modifier
{
	GENERATED_BODY()
	
public:
	UTGOR_Effect();

	/** Stat types this effect is triggered by with value threshold */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "!TGOR Effect")
		TSubclassOf<UTGOR_Stat> Stat;

	/** Minimum of the range for when this effect is triggered */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "!TGOR Effect")
		float Min;

	/** Maximum of the range for when this effect is triggered */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "!TGOR Effect")
		float Max;

};
