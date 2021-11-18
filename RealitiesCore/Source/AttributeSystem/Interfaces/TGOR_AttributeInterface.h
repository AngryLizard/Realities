// The Gateway of Realities: Planes of Existence.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "TGOR_AttributeInterface.generated.h"

class UTGOR_AttributeComponent;
class UTGOR_Attribute;

UINTERFACE(MinimalAPI)
class UTGOR_AttributeInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * Interface for objects that modify attributes
 */
class ATTRIBUTESYSTEM_API ITGOR_AttributeInterface
{
	GENERATED_BODY()

public:

	/** Get attribute modifications */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "!TGOR Game", Meta = (Keywords = "C++"))
		TArray<UTGOR_Modifier*> QueryActiveModifiers() const;
	virtual TArray<UTGOR_Modifier*> QueryActiveModifiers_Implementation() const;
};
