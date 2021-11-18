// The Gateway of Realities: Planes of Existence.

#pragma once

#include "CoreMinimal.h"
#include "DimensionSystem/Content/TGOR_SpawnModule.h"
#include "TGOR_Modifier.generated.h"

class UTGOR_Attribute;

/**
 * 
 */
UCLASS(Blueprintable)
class ATTRIBUTESYSTEM_API UTGOR_Modifier : public UTGOR_SpawnModule
{
	GENERATED_BODY()

public:
	UTGOR_Modifier();

	////////////////////////////////////////////////////////////////////////////////////////////////////
public:

	/** Attributes in this modifier */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "!TGOR Insertion")
		TMap<TSubclassOf<UTGOR_Attribute>, float> AttributeInsertions;
	DECL_INSERTION(AttributeInsertions);

	virtual void MoveInsertion(UTGOR_Content* Insertion, ETGOR_InsertionActionEnumeration Action, bool& Success) override;
};