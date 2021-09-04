// The Gateway of Realities: Planes of Existence.

#pragma once

#include "CoreMinimal.h"

#include "UObject/Interface.h"
#include "TGOR_RegisterInterface.generated.h"

class UTGOR_CoreContent;

UINTERFACE(MinimalAPI)
class UTGOR_RegisterInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class CORESYSTEM_API ITGOR_RegisterInterface
{
	GENERATED_BODY()

public:

	/**  */
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "!TGOR System")
		bool IsContentActive(UTGOR_CoreContent* Content) const;
	virtual bool IsContentActive_Implementation(UTGOR_CoreContent* Content) const;

	/**  */
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "!TGOR System")
		TSet<UTGOR_CoreContent*> GetActiveContent() const;
		virtual TSet<UTGOR_CoreContent*> GetActiveContent_Implementation() const;


private:
};
