// The Gateway of Realities: Planes of Existence.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "TGOR_UnlockInterface.generated.h"

class UTGOR_UnlockComponent;

UINTERFACE(MinimalAPI)
class UTGOR_UnlockInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 *
 */
class REALITIES_API ITGOR_UnlockInterface
{
	GENERATED_BODY()

public:

	/** Set unlock structures */
	UFUNCTION(BlueprintNativeEvent, Category = "!TGOR Game", Meta = (Keywords = "C++"))
		void UpdateUnlocks(UTGOR_UnlockComponent* Component);
	virtual void UpdateUnlocks_Implementation(UTGOR_UnlockComponent* Component);

	/** Tell whether a given content class is supported/unlocked */
	UFUNCTION(BlueprintNativeEvent, Category = "!TGOR Game", Meta = (Keywords = "C++"))
		bool SupportsContent(UTGOR_Unlock* Unlock) const;
	virtual bool SupportsContent_Implementation(UTGOR_Unlock* Unlock) const;

};
