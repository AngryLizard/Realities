// The Gateway of Realities: Planes of Existence.

#pragma once

#include "CoreSystem/Content/TGOR_CoreContent.h"
#include "TGOR_RigParam.generated.h"

/**
 * 
 */
UCLASS(Blueprintable)
class ANIMATIONSYSTEM_API UTGOR_RigParam : public UTGOR_CoreContent
{
	GENERATED_BODY()
	
public:
	UTGOR_RigParam();

	/**  */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "!TGOR Animation", meta = (ClampMin = "0.0", ClampMax = "1.0"))
		float DefaultValue;
};
