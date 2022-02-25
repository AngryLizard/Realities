// The Gateway of Realities: Planes of Existence.

#pragma once

#include "CoreMinimal.h"
#include "Utility/TGOR_CustomEnumerations.h"
#include "Base/Instances/Inventory/TGOR_MetamorphInstance.h"
#include "Base/Instances/Inventory/TGOR_InventoryInstance.h"

#include "Components/TGOR_Component.h"
#include "TGOR_MetamorphComponent.generated.h"

class UTGOR_ContainerComponent;
class UTGOR_Metamorph;


/**
* FTGOR_MetamorphTimer increases progress and keeps timestamp of last update
*/
USTRUCT(BlueprintType)
struct FTGOR_MetamorphTimer
{
	GENERATED_USTRUCT_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TGOR|Attributes")
		float Progress;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TGOR|Attributes")
		float LastUpdate;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TGOR|Attributes")
		float MetamorphTime;
};

/**
*
*/
USTRUCT(BlueprintType)
struct REALITIES_API FTGOR_ContainerItemPair
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, Category = "TGOR|Region", Meta = (Keywords = "C++"))
		TMap<FTGOR_SlotIndex, float> Items;
};

/**
*
*/
USTRUCT(BlueprintType)
struct REALITIES_API FTGOR_ContainerPair
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, Category = "TGOR|Region", Meta = (Keywords = "C++"))
		UTGOR_ContainerComponent* Container;

	UPROPERTY(EditAnywhere, Category = "TGOR|Region", Meta = (Keywords = "C++"))
		FTGOR_SlotIndex Index;

	UPROPERTY(EditAnywhere, Category = "TGOR|Region", Meta = (Keywords = "C++"))
		UClass* Class;
};

/**
* 
*/
USTRUCT(BlueprintType)
struct REALITIES_API FTGOR_ContainerList
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, Category = "TGOR|Region", Meta = (Keywords = "C++"))
		TArray<FTGOR_ContainerPair> Containers;
};

/**
 * 
 */
UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class REALITIES_API UTGOR_MetamorphComponent : public UTGOR_Component
{
	GENERATED_BODY()
	
public:

	/** Maintains timer */
	UFUNCTION(BlueprintCallable, Category = "TGOR|Simulation", Meta = (Keywords = "C++"))
		void MaintainTimer();

	/** Increase timer for a specific recipe, returns true if limit got reached */
	UFUNCTION(BlueprintCallable, Category = "TGOR|Simulation", Meta = (Keywords = "C++"))
		bool IncreaseTimer(const FTGOR_MetamorphRecipe& Recipe);

	/** Apply metamorph to an item instance over a given influence, return produced items */
	UFUNCTION(BlueprintCallable, Category = "TGOR|Simulation", Meta = (Keywords = "C++"))
		TArray<FTGOR_ItemInstance_OLD> MetamorphItem(UPARAM(ref) FTGOR_ItemInstance_OLD& ItemInstance, const FTGOR_ElementInstance& ElementInstance);

	////////////////////////////////////////////////////////////////////////////////////////////////////
	
	/** Metamorph queue keeping track of timers */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "TGOR|Simulation")
		TMap<int32, FTGOR_MetamorphTimer> MetamorphQueue;

	/** Gets progress of the closest timer inside the queue and the average of its required elemental forces */
	UFUNCTION(BlueprintPure, Category = "TGOR|Simulation", Meta = (Keywords = "C++"))
		float GetProgress(FTGOR_ElementInstance& ElementInstance) const;

	////////////////////////////////////////////////////////////////////////////////////////////////////

	/** Gets container components of all itdemdrops around */
	UFUNCTION(BlueprintCallable, Category = "TGOR|Simulation", Meta = (Keywords = "C++"))
		TArray<UTGOR_ContainerComponent*> GetContainersAround(float Radius);


	/** Do a combination recipe */
	UFUNCTION(BlueprintCallable, Category = "TGOR|Simulation", Meta = (Keywords = "C++"))
		TArray<FTGOR_ItemInstance_OLD> CombineItemsInside(const TArray<UTGOR_ContainerComponent*>& Containers,
													const FTGOR_MetamorphRecipe& Recipe,
													AActor* Instigator);

	/** Check if a combination recipe can be done and return available Itemdrops */
	UFUNCTION(BlueprintCallable, Category = "TGOR|Simulation", Meta = (Keywords = "C++"))
		bool CanCombineItemsInside(	const TArray<UTGOR_ContainerComponent*>& Containers, 
									TArray<FTGOR_ContainerList>& Available, 
									TMap<UTGOR_ContainerComponent*, FTGOR_ContainerItemPair>& Register,
									const FTGOR_MetamorphRecipe& Recipe);

	/** Get Itemdrop/quantity pairs on itemdrops around */
	UFUNCTION(BlueprintCallable, Category = "TGOR|Simulation", Meta = (Keywords = "C++"))
		bool GetAvailableItemsInside(	const TArray<UTGOR_ContainerComponent*>& Containers,
										TArray<FTGOR_ContainerList>& Available, 
										TMap<UTGOR_ContainerComponent*, FTGOR_ContainerItemPair>& Register, 
										const TArray<FTGOR_MetamorphEntryInstance>& Input);

	////////////////////////////////////////////////////////////////////////////////////////////////////

private:
	// Get metamorph time
	float GetMetamorphTime(const FTGOR_MetamorphRecipe& Recipe);
	
	// Metamorph cache
	UTGOR_Metamorph* _cache;
};
