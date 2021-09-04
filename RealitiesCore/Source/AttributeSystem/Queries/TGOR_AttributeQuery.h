// The Gateway of Realities: Planes of Existence.

#pragma once

#include "CoreMinimal.h"
#include "../Components/TGOR_AttributeComponent.h"

#include "UISystem/Queries/TGOR_ContentQuery.h"
#include "TGOR_AttributeQuery.generated.h"

////////////////////////////////////////////// DECL //////////////////////////////////////////////////////

class UTGOR_Content;


/**
 * 
 */
UCLASS()
class ATTRIBUTESYSTEM_API UTGOR_AttributeQuery : public UTGOR_ContentQuery
{
	GENERATED_BODY()
	
public:
	UTGOR_AttributeQuery();
	TArray<UTGOR_CoreContent*> QueryContent() const override;

	////////////////////////////////////////////////////////////////////////////////////////////////////

	/** Assigns StatComponent and filters stat states */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Attributes", Meta = (Keywords = "C++"))
		virtual void AssignComponent(UTGOR_AttributeComponent* AttributeComponent);

	UPROPERTY(BlueprintReadOnly, Category = "!TGOR Attributes")
		TArray<FTGOR_AttributeOutput> Attributes;
};
