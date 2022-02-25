// The Gateway of Realities: Planes of Existence.

#pragma once

#include "Kismet/KismetSystemLibrary.h"
#include "Base/Controller/TGOR_PlayerController.h"
#include "Actors/Inventory/TGOR_ItemDropActor.h"

#include "UI/TGOR_Menu.h"
#include "TGOR_InventoryManager.generated.h"

/**
* TGOR_InventoryManager provides general utility functions to make inventory interactions easier
*/
UCLASS()
class REALITIES_API UTGOR_InventoryManager : public UTGOR_Menu
{
	GENERATED_BODY()
	
public:
	
	/** Computes position of mouse pointer in world space. Sets location below specified pawn if result is further than Distance away. */
	UFUNCTION(BlueprintCallable, Category = "TGOR|Inventory", Meta = (ExpandEnumAsExecs = "Branches", Keywords = "C++"))
		static FVector ScreenPositionToWorldLocation(APawn* Pawn, FVector2D ScreenPosition, ETraceTypeQuery Query, float Height, float Distance, ETGOR_ComputeEnumeration& Branches);
};