// The Gateway of Realities: Planes of Existence.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "TGOR_AnimationLibrary.generated.h"

class UTGOR_Primitive;
class UTGOR_HandleComponent;


UCLASS()
class UTGOR_AnimationLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_UCLASS_BODY()

	/** Gets first pilot of a given primitive type (Checks target type not content, safe to be called during content load) */
	UFUNCTION(BlueprintPure, Category = "!TGOR Animation")
		static UTGOR_HandleComponent* GetHandleComponent(AActor* Owner, TSubclassOf<UTGOR_Primitive> Type);

	/** Get all pilots of a given primitive type (Checks target type not content, safe to be called during content load) */
	UFUNCTION(BlueprintPure, Category = "!TGOR Animation")
		static TArray<UTGOR_HandleComponent*> GetHandleComponents(AActor* Owner, TSubclassOf<UTGOR_Primitive> Type);
};
