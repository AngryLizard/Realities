// TGOR (C) // CHECKED //
#pragma once

#include "CoreMinimal.h"
#include "Realities/Utility/TGOR_CustomEnumerations.h"
#include "Realities/Utility/Storage/TGOR_Serialisation.h"
#include "Realities/Base/Instances/System/TGOR_ActionInstance.h"
#include "Realities/Mod/Animations/TGOR_Animation.h"
#include "Realities/Mod/Spawner/Creatures/TGOR_Creature.h"

#include "Realities/Interfaces/TGOR_SaveInterface.h"
#include "Realities/Components/Inventory/TGOR_ItemRegisterComponent.h"
#include "TGOR_InventoryComponent.generated.h"

////////////////////////////////////////////// DECL //////////////////////////////////////////////////////


///////////////////////////////////////////// STRUCTS ///////////////////////////////////////////////////


/**
 * TGOR_InventoryComponent allows storing inventory items
 */
UCLASS(ClassGroup = (Custom), Blueprintable, meta = (BlueprintSpawnableComponent))
class REALITIES_API UTGOR_InventoryComponent : public UTGOR_ItemRegisterComponent, public ITGOR_SaveInterface
{
	GENERATED_BODY()

public:

	////////////////////////////////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////// ENGINE ////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////////////////////////////

	UTGOR_InventoryComponent();

	void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	void GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const override;

	virtual bool CanStoreItemAt(const FTGOR_SlotIndex& Index, UTGOR_ItemStorage* Storage) const override;
	virtual int32 GetContainerCapacity(const FTGOR_ContainerIndex& Index) const override;

	//////////////////////////////////////////// IMPLEMENTABLES ////////////////////////////////////////


	UPROPERTY(BlueprintAssignable, Category = "TGOR Storage")
		FItemMoveDelegate OnItemPut;

	////////////////////////////////////////////////////////////////////////////////////////////////////
public:

	/** Puts item in first available slot, returns residuals. */
	UFUNCTION(BlueprintCallable, Category = "TGOR Inventory", Meta = (Keywords = "C++"))
		UTGOR_ItemStorage* PutItem(UTGOR_ItemStorage* Storage);

	/** Tries putting an item into a given container, returns residuals. */
	UFUNCTION(BlueprintCallable, Category = "TGOR Inventory", Meta = (Keywords = "C++"))
		UTGOR_ItemStorage* PutItemInto(const FTGOR_ContainerIndex& Index, UTGOR_ItemStorage* Storage);

	/** Generates item and puts it into this inventory, returns residual */
	UFUNCTION(BlueprintCallable, Category = "TGOR Inventory", Meta = (Keywords = "C++"))
		UTGOR_ItemStorage* CreateItem(TSubclassOf<UTGOR_Item> Type);

	////////////////////////////////////////////////////////////////////////////////////////////////////
protected:
	

	/** Max number of supported containers. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "TGOR Inventory")
		int32 MaxContainerCount;

	/** Max number of supported items per container. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "TGOR Inventory")
		int32 MaxItemCount;
	
	/////////////////////////////////////////////// INTERNAL ///////////////////////////////////////////

protected:

private:

};
