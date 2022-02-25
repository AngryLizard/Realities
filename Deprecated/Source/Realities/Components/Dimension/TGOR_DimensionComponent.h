// The Gateway of Realities: Planes of Existence.

#pragma once

#include "Realities/Interfaces/TGOR_DimensionInterface.h"
#include "Realities/Components/TGOR_Component.h"
#include "TGOR_DimensionComponent.generated.h"

class UTGOR_DimensionData;

/**
*	TGOR_DimensionComponent provides functionality for components that interact with the dimension system.
*	WARNING: Only active in loading phase. Assemble functions need to be called manually when spawned dynamically.
*/
UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class REALITIES_API UTGOR_DimensionComponent : public UTGOR_Component, public ITGOR_DimensionInterface
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UTGOR_DimensionComponent();

	
};
