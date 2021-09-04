// The Gateway of Realities: Planes of Existence.

#pragma once

#include "CoreMinimal.h"

#include "UISystem/Queries/TGOR_ContentQuery.h"
#include "TGOR_StorageQuery.generated.h"

class UTGOR_Item;
class UTGOR_StorageComponent;

/**
 * UTGOR_StorageQuery
 */
UCLASS()
class INVENTORYSYSTEM_API UTGOR_StorageQuery : public UTGOR_ContentQuery
{
	GENERATED_BODY()
	
public:
	UTGOR_StorageQuery();
	virtual TArray<UTGOR_CoreContent*> QueryContent() const override;

	/** Constructs assemble recipes using currently running action slot as matter source */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Inventory", Meta = (Keywords = "C++"))
		virtual void AssignComponent(UTGOR_StorageComponent* StorageComponent);

	/** Owning CraftingComponent */
	UPROPERTY(BlueprintReadOnly, Category = "!TGOR Inventory")
		UTGOR_StorageComponent* HostComponent;
};
