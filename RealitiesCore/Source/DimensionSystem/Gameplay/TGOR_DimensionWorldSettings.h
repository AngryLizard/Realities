// The Gateway of Realities: Planes of Existence.

#pragma once

#include "CoreMinimal.h"
#include "CoreSystem/Gameplay/TGOR_WorldSettings.h"
#include "TGOR_DimensionWorldSettings.generated.h"

class UTGOR_Dimension;

/**
 * 
 */
UCLASS(Blueprintable)
class DIMENSIONSYSTEM_API ATGOR_DimensionWorldSettings : public ATGOR_WorldSettings
{
	GENERATED_BODY()
	
public:
	ATGOR_DimensionWorldSettings();

	////////////////////////////////////////////////////////////////////////////////////////////////////

	/* Dimension this world is associated with */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "!TGOR Dimension", Meta = (Keywords = "C++"))
		TSubclassOf<UTGOR_Dimension> Dimension;

	////////////////////////////////////////////////////////////////////////////////////////////////////
	
	/* Max height of Reality */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "!TGOR Dimension", Meta = (Keywords = "C++"))
		float RealityMaxHeight;

	/* Max height of MiniDimensions */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "!TGOR Dimension", Meta = (Keywords = "C++"))
		float MiniDimensionsMaxHeight;

};
