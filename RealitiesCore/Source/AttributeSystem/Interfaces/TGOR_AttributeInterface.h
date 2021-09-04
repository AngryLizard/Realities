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
 *
 */
class ATTRIBUTESYSTEM_API ITGOR_AttributeInterface
{
	GENERATED_BODY()

public:

	/** Set attribute structures */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "!TGOR Game", Meta = (Keywords = "C++"))
		void UpdateAttributes(const UTGOR_AttributeComponent* Component);
	virtual void UpdateAttributes_Implementation(const UTGOR_AttributeComponent* Component);

	/** Get cached attribute value */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "!TGOR Game", Meta = (Keywords = "C++"))
		float GetAttribute(UTGOR_Attribute* Attribute, float Default) const;
	virtual float GetAttribute_Implementation(UTGOR_Attribute* Attribute, float Default) const;

};
