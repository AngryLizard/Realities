// The Gateway of Realities: Planes of Existence.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/WorldSettings.h"
#include "TGOR_WorldSettings.generated.h"

class UTGOR_Dimension;
class UTGOR_Creature;

/**
 * 
 */
UCLASS(Blueprintable)
class REALITIES_API ATGOR_WorldSettings : public AWorldSettings
{
	GENERATED_BODY()
	
public:
	ATGOR_WorldSettings();
	virtual void NotifyBeginPlay() override;

	////////////////////////////////////////////////////////////////////////////////////////////////////

	/* Dimension this world is associated with */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TGOR Dimension", Meta = (Keywords = "C++"))
		TSubclassOf<UTGOR_Dimension> Dimension;

	/* Overrides profile if not empty. This should be enabled if you want to test multiplayer in editor. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TGOR System", Meta = (Keywords = "C++"))
		FString ProfileOverride;

	/* Directory to save to */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TGOR System", Meta = (Keywords = "C++"))
		FString SaveDirectory;

	/* Whether to directly spawn as default pawn if there is no save. This should probably be enabled when testing a map other than entry map. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TGOR System", Meta = (Keywords = "C++"))
		bool TestEntry;

	/* Creature to spawn during testing */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TGOR System", Meta = (Keywords = "C++"))
		TSubclassOf<UTGOR_Creature> TestCreature;

	////////////////////////////////////////////////////////////////////////////////////////////////////
	
	/* Max height of Reality */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TGOR Dimension", Meta = (Keywords = "C++"))
		float RealityMaxHeight;

	/* Max height of MiniDimensions */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TGOR Dimension", Meta = (Keywords = "C++"))
		float MiniDimensionsMaxHeight;

};
