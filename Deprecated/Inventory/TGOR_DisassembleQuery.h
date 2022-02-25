// The Gateway of Realities: Planes of Existence.

#pragma once

#include "CoreMinimal.h"
#include "Realities/Base/Instances/System/TGOR_IndexInstance.h"
#include "Realities/UI/Queries/Inventory/TGOR_RecipeQuery.h"
#include "TGOR_DisassembleQuery.generated.h"

class UTGOR_StorageComponent;

/**
 * 
 */
UCLASS()
class REALITIES_API UTGOR_DisassembleQuery : public UTGOR_RecipeQuery
{
	GENERATED_BODY()
	
public:
	UTGOR_DisassembleQuery();
	virtual TArray<UTGOR_Content*> QueryContent() const override;
	virtual void AssignComponent(UTGOR_ActionComponent* ActionComponent, UTGOR_MatterComponent* MatterComponent, bool Completed) override;
	virtual UTGOR_Item* GetRecipe(int32 Index) const override;

	/** Recipes */
	UPROPERTY(BlueprintReadOnly, Category = "TGOR Inventory")
		TArray<UTGOR_ItemComponent*> AttachedItems;

	/** Owning Inventory */
	UPROPERTY(BlueprintReadOnly, Category = "TGOR Inventory")
		UTGOR_StorageComponent* StorageComponent;
};
