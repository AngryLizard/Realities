// The Gateway of Realities: Planes of Existence.

#pragma once

#include "CoreMinimal.h"
#include "Realities/Base/Instances/System/TGOR_IndexInstance.h"
#include "Realities/UI/Queries/TGOR_ContentQuery.h"
#include "TGOR_ProcessQuery.generated.h"

class UTGOR_Process;
class UTGOR_ProcessComponent;

/**
 * UTGOR_ProcessQuery
 */
UCLASS()
class REALITIES_API UTGOR_ProcessQuery : public UTGOR_ContentQuery
{
	GENERATED_BODY()
	
public:
	UTGOR_ProcessQuery();
	virtual TArray<UTGOR_Content*> QueryContent() const override;

	/** Constructs assemble recipes using currently running action slot as matter source */
	UFUNCTION(BlueprintCallable, Category = "TGOR Inventory", Meta = (Keywords = "C++"))
		virtual void AssignComponent(UTGOR_ActionComponent* ActionComponent, UTGOR_ProcessComponent* MatterComponent, TSubclassOf<UTGOR_Process> Type, bool Match);

	/** Recipes */
	UPROPERTY(BlueprintReadOnly, Category = "TGOR Inventory")
		TArray<UTGOR_Process*> Processes;

	/** Owning ActionComponent */
	UPROPERTY(BlueprintReadOnly, Category = "TGOR Inventory")
		UTGOR_ActionComponent* OwnerComponent;

	/** Owning CraftingComponent */
	UPROPERTY(BlueprintReadOnly, Category = "TGOR Inventory")
		UTGOR_ProcessComponent* HostComponent;

	/** Whether only completed processes are shown */
	UPROPERTY(BlueprintReadOnly, Category = "TGOR Inventory")
		bool RecipeMatch;
};
