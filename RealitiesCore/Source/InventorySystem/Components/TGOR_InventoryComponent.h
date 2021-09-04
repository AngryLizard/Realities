// TGOR (C) // CHECKED //
#pragma once

#include "CoreMinimal.h"

#include "CoreSystem/Storage/TGOR_SaveInterface.h"
#include "TGOR_ItemRegisterComponent.h"
#include "TGOR_InventoryComponent.generated.h"

////////////////////////////////////////////// DECL //////////////////////////////////////////////////////


///////////////////////////////////////////// STRUCTS ///////////////////////////////////////////////////


/**
 * TGOR_InventoryComponent allows storing inventory items
 */
UCLASS(ClassGroup = (Custom), Blueprintable, meta = (BlueprintSpawnableComponent))
class INVENTORYSYSTEM_API UTGOR_InventoryComponent : public UTGOR_ItemRegisterComponent, public ITGOR_SaveInterface
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

	virtual bool CanStoreItemAt(int32 Identifier, UTGOR_ItemStorage* Storage) const override;

	//////////////////////////////////////////// IMPLEMENTABLES ////////////////////////////////////////


	UPROPERTY(BlueprintAssignable, Category = "!TGOR Storage")
		FItemMoveDelegate OnItemPut;

	////////////////////////////////////////////////////////////////////////////////////////////////////
public:

	/** Puts item in first available slot, returns residuals. */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Inventory", Meta = (Keywords = "C++"))
		UTGOR_ItemStorage* PutItem(UTGOR_ItemStorage* Storage);

	/** Generates item and puts it into this inventory, returns residual */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Inventory", Meta = (Keywords = "C++"))
		UTGOR_ItemStorage* CreateItem(TSubclassOf<UTGOR_Item> Type);

	////////////////////////////////////////////////////////////////////////////////////////////////////
protected:
	
	/** Max number of supported items. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "!TGOR Inventory")
		int32 MaxItemCount;
	
	/////////////////////////////////////////////// INTERNAL ///////////////////////////////////////////

protected:

private:

};
