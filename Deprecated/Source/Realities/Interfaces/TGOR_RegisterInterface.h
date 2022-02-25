// The Gateway of Realities: Planes of Existence.

#pragma once

#include "CoreMinimal.h"
#include "Templates/SubclassOf.h"

#include "UObject/Interface.h"
#include "TGOR_RegisterInterface.generated.h"

class UTGOR_Content;

UINTERFACE(MinimalAPI)
class UTGOR_RegisterInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class REALITIES_API ITGOR_RegisterInterface
{
	GENERATED_BODY()

public:
	
	/**  */
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "TGOR System")
		TSet<UTGOR_Content*> GetActiveContent() const;
		virtual TSet<UTGOR_Content*> GetActiveContent_Implementation() const;


private:
};
