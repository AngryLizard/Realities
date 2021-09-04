// The Gateway of Realities: Planes of Existence.

#pragma once

#include "CoreMinimal.h"
#include "CoreSystem/Content/TGOR_CoreContent.h"
#include "TGOR_Modifier.generated.h"

class UTGOR_Attribute;

/**
 * 
 */
UCLASS(Blueprintable)
class ATTRIBUTESYSTEM_API UTGOR_Modifier : public UTGOR_CoreContent
{
	GENERATED_BODY()

public:
	UTGOR_Modifier();
	virtual void PostBuildResource() override;

	////////////////////////////////////////////////////////////////////////////////////////////////////
protected:

	/** Defined attributes to modify */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TMap<TSubclassOf<UTGOR_Attribute>, float> Attributes;

	////////////////////////////////////////////////////////////////////////////////////////////////////
public:

	/** Modified attribute values */
	UPROPERTY()
		TMap<UTGOR_Attribute*, float> AttributeValues;


};