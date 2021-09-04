// The Gateway of Realities: Planes of Existence.

#pragma once

#include "CoreMinimal.h"

#include "UISystem/Queries/TGOR_ContentQuery.h"
#include "TGOR_ProcessQuery.generated.h"

class UTGOR_Process;
class UTGOR_ProcessComponent;

/**
 * UTGOR_ProcessQuery
 */
UCLASS()
class INVENTORYSYSTEM_API UTGOR_ProcessQuery : public UTGOR_ContentQuery
{
	GENERATED_BODY()
	
public:
	UTGOR_ProcessQuery();
	virtual TArray<UTGOR_CoreContent*> QueryContent() const override;

	/** Constructs assemble recipes for a given pawn */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Inventory", Meta = (Keywords = "C++"))
		virtual void AssignComponent(APawn* Pawn, UTGOR_ProcessComponent* MatterComponent, TSubclassOf<UTGOR_Process> Type, bool Match);

	/** Recipes */
	UPROPERTY(BlueprintReadOnly, Category = "!TGOR Inventory")
		TArray<UTGOR_Process*> Processes;

	/** Owning Pawn */
	UPROPERTY(BlueprintReadOnly, Category = "!TGOR Inventory")
		APawn* OwnerPawn;

	/** Owning CraftingComponent */
	UPROPERTY(BlueprintReadOnly, Category = "!TGOR Inventory")
		UTGOR_ProcessComponent* HostComponent;

	/** Whether only completed processes are shown */
	UPROPERTY(BlueprintReadOnly, Category = "!TGOR Inventory")
		bool RecipeMatch;
};
