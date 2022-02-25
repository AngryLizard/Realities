// The Gateway of Realities: Planes of Existence.

#pragma once

#include "CoreMinimal.h"
#include "Realities/Base/Instances/System/TGOR_IndexInstance.h"
#include "Realities/UI/Queries/TGOR_ContentQuery.h"
#include "TGOR_RecipeQuery.generated.h"

class UTGOR_Item;
class UTGOR_MatterComponent;

/**
 * UTGOR_RecipeQuery
 */
UCLASS()
class REALITIES_API UTGOR_RecipeQuery : public UTGOR_ContentQuery
{
	GENERATED_BODY()
	
public:
	UTGOR_RecipeQuery();
	virtual TArray<UTGOR_Content*> QueryContent() const override;

	/** Constructs assemble recipes using currently running action slot as matter source */
	UFUNCTION(BlueprintCallable, Category = "TGOR Inventory", Meta = (Keywords = "C++"))
		virtual void AssignComponent(UTGOR_ActionComponent* ActionComponent, UTGOR_MatterComponent* MatterComponent, ETGOR_RecipeEnumeration Match);

	/** Recipes */
	UPROPERTY(BlueprintReadOnly, Category = "TGOR Inventory")
		TArray<UTGOR_Item*> Recipes;

	/** Owning ActionComponent */
	UPROPERTY(BlueprintReadOnly, Category = "TGOR Inventory")
		UTGOR_ActionComponent* OwnerComponent;

	/** Owning CraftingComponent */
	UPROPERTY(BlueprintReadOnly, Category = "TGOR Inventory")
		UTGOR_MatterComponent* HostComponent;

	/** Whether only completed recipes are shown */
	UPROPERTY(BlueprintReadOnly, Category = "TGOR Inventory")
		ETGOR_RecipeEnumeration RecipeMatch;
};
