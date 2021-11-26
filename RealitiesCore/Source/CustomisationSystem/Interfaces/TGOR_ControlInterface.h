// The Gateway of Realities: Planes of Existence.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"

#include "TGOR_ControlInterface.generated.h"

class UTGOR_ControlSkeletalMeshComponent;

UINTERFACE(MinimalAPI, meta=(CannotImplementInterfaceInBlueprint))
class UTGOR_ControlInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class CUSTOMISATIONSYSTEM_API ITGOR_ControlInterface
{
	GENERATED_BODY()

public:
	ITGOR_ControlInterface();

	////////////////////////////////////////////////////////////////////////////////////////////////////

	/** Whether this control only changes transform on initialisation */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Game", Meta = (Keywords = "C++"))
		virtual bool IsInitialOnly() const;

	/** Gets the name of this control */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Game", Meta = (Keywords = "C++"))
		virtual FName GetControlName() const;

	/** Gets the transform of this control relative to a given component */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Game", Meta = (Keywords = "C++"))
		virtual FTransform GetControlTransform(USkinnedMeshComponent* Component) const;
};
