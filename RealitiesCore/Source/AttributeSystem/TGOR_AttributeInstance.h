// The Gateway of Realities: Planes of Existence.

#pragma once

#include "CoreSystem/Storage/TGOR_PackageGroup.h"

#include "TGOR_AttributeInstance.generated.h"

class UTGOR_AttributeComponent;
class UTGOR_Attribute;

/**
* TGOR_AttributeInstance stores attribute values
*/
USTRUCT(BlueprintType)
struct ATTRIBUTESYSTEM_API FTGOR_AttributeInstance
{
	GENERATED_USTRUCT_BODY()

	FTGOR_AttributeInstance();

	////////////////////////////////////////////////////////////////////////////////////////////////////

	float GetAttribute(UTGOR_Attribute* Attribute, float Default) const;
	
	////////////////////////////////////////////////////////////////////////////////////////////////////

	/** List of attribute values */
	UPROPERTY(BlueprintReadOnly)
		TMap<TObjectPtr<UTGOR_Attribute>, float> Values;

};

