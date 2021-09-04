// The Gateway of Realities: Planes of Existence.

#pragma once

#include "CoreSystem/Storage/TGOR_SaveInterface.h"

#include "../TGOR_ItemInstance.h"

#include "TGOR_ContainerComponent.h"
#include "TGOR_ItemComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FItemDropDelegate, UTGOR_ItemStorage*, Storage);

/**
* This component can be attached to every actor to give them the functionallity to store a item instance.
*/
UCLASS(BlueprintType, Blueprintable, ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class INVENTORYSYSTEM_API UTGOR_ItemComponent : public UTGOR_ContainerComponent, public ITGOR_SaveInterface
{
	GENERATED_BODY()

public:
	UTGOR_ItemComponent();
	void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	void GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const override;

	virtual TSet<UTGOR_CoreContent*> GetActiveContent_Implementation() const override;
	virtual bool PurgeItemStorage(UTGOR_ItemStorage* Storage) override;

	//////////////////////////////////////////// IMPLEMENTABLES ////////////////////////////////////////

	UPROPERTY(BlueprintAssignable, Category = "!TGOR Inventory")
		FItemDropDelegate OnPack;

	UPROPERTY(BlueprintAssignable, Category = "!TGOR Inventory")
		FItemDropDelegate OnUnpack;

	////////////////////////////////////////////////////////////////////////////////////////////////////
private:

	/** Currently included slots. */
	UPROPERTY(ReplicatedUsing = RepNotifyItemSlot)
		FTGOR_ItemInstance ItemSlot;

	/**  */
	UFUNCTION()
		void RepNotifyItemSlot();

	////////////////////////////////////////////////////////////////////////////////////////////////////
public:

	/** Tries to pick up the item instance. On success the actor of this component will despawn and return the item. */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Inventory", Meta = (Keywords = "C++"))
		UTGOR_ItemStorage* PutItem(UTGOR_ItemStorage* Storage);

	/** Tries to pick up the item instance. On success the actor of this component will despawn and return the item. */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Inventory", Meta = (Keywords = "C++"))
		UTGOR_ItemStorage* PickItem();

	/** Returns item storage. */
	UFUNCTION(BlueprintPure, Category = "!TGOR Inventory", Meta = (Keywords = "C++"))
		UTGOR_ItemStorage* PeekStorage() const;


protected:

};
