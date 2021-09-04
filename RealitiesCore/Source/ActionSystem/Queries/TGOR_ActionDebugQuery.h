// The Gateway of Realities: Planes of Existence.

#pragma once

#include "CoreMinimal.h"

#include "../TGOR_ActionInstance.h"

#include "UISystem/Queries/TGOR_SelectionQuery.h"
#include "TGOR_ActionDebugQuery.generated.h"

class UTGOR_InteractableComponent;
class UTGOR_ActionComponent;
class UTGOR_ActionSlot;
class UTGOR_Action;

/**
 * 
 */
UCLASS()
class ACTIONSYSTEM_API UTGOR_ActionDebugQuery : public UTGOR_SelectionQuery
{
	GENERATED_BODY()
	
public:
	UTGOR_ActionDebugQuery();

	virtual TArray<FTGOR_Display> QueryDisplays() const override;

	////////////////////////////////////////////////////////////////////////////////////////////////////

	/** Fills this query with all slots of a given component */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Interaction", Meta = (Keywords = "C++"))
		void AssignComponent(UTGOR_ActionComponent* Component, float Duration);


	////////////////////////////////////////////////////////////////////////////////////////////////////
protected:

	/** */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "!TGOR Interaction")
		TArray<FTGOR_ActionDebugInfo> Entries;

};
