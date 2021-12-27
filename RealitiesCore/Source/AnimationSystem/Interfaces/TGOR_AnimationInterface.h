
// The Gateway of Realities: Planes of Existence.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"

#include "CoreSystem/Utility/TGOR_CoreEnumerations.h"

#include "../Content/TGOR_Performance.h"
#include "../Components/TGOR_AnimationComponent.h"

#include "TGOR_AnimationInterface.generated.h"

class UTGOR_Performance;

UINTERFACE(MinimalAPI, meta=(CannotImplementInterfaceInBlueprint))
class UTGOR_AnimationInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class ANIMATIONSYSTEM_API ITGOR_AnimationInterface
{
	GENERATED_BODY()

public:
	ITGOR_AnimationInterface();

	////////////////////////////////////////////////////////////////////////////////////////////////////

	/** Gets the slot type */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Game", Meta = (Keywords = "C++"))
		virtual TSubclassOf<UTGOR_Performance> GetPerformanceType() const;

	/** Gets the relevant animation component */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Game", Meta = (Keywords = "C++"))
		virtual UTGOR_AnimationComponent* GetAnimationComponent() const;

	/**
	// Gets the relevant animation content 
	UFUNCTION(BlueprintCallable, Category = "!TGOR Game", Meta = (Keywords = "C++"))
		virtual UTGOR_Animation* GetAnimation() const;

	// Gets the relevant animation content of a given type or null if not compatible
	UFUNCTION(BlueprintCallable, Category = "!TGOR Game", Meta = (DeterminesOutputType = "Type", ExpandEnumAsExecs = "Branches", Keywords = "C++"))
		virtual UTGOR_Animation* GetAnimationChecked(TSubclassOf<UTGOR_Animation> Type, ETGOR_ValidEnumeration& Branches);
		 */

protected:
};
