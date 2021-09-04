// The Gateway of Realities: Planes of Existence.

#pragma once

#include "CoreMinimal.h"

#include "UISystem/Queries/TGOR_ContentQuery.h"
#include "TGOR_ScienceQuery.generated.h"

class UTGOR_Science;
class UTGOR_ActionComponent;
class UTGOR_ProcessComponent;

/**
 * 
 */
UCLASS()
class INVENTORYSYSTEM_API UTGOR_ScienceQuery : public UTGOR_ContentQuery
{
	GENERATED_BODY()

public:
	UTGOR_ScienceQuery();
	virtual TArray<UTGOR_CoreContent*> QueryContent() const override;

	/** Constructs science recipes using currently running action slot */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Inventory", Meta = (Keywords = "C++"))
		void AssignComponent(APawn* Pawn, UTGOR_ProcessComponent* ProcessComponent, bool Completed);


	/** Filtered sciences */
	UPROPERTY(BlueprintReadOnly, Category = "!TGOR Inventory")
		TArray<UTGOR_Science*> Sciences;

	/** Owning Pawn */
	UPROPERTY(BlueprintReadOnly, Category = "!TGOR Inventory")
		APawn* OwnerPawn;

	/** Owning ScienceComponent */
	UPROPERTY(BlueprintReadOnly, Category = "!TGOR Inventory")
		UTGOR_ProcessComponent* HostComponent;
};