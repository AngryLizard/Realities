#pragma once

#include "Base/Instances/Inventory/TGOR_ItemInstance_OLD.h"
#include "Base/Content/TGOR_Content.h"

#include "Actors/TGOR_Actor.h"
#include "TGOR_ItemDropActor.generated.h"

class UTGOR_InteractableComponent;
class UTGOR_SingleItemComponent;
class UTGOR_SaveComponent;
class UTGOR_Item;


UCLASS()
class REALITIES_API ATGOR_ItemDropActor : public ATGOR_Actor
{
	GENERATED_BODY()
	
public:

	////////////////////////////////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////// ENGINE ////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////////////////////////////

	ATGOR_ItemDropActor(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	virtual void BeginPlay() override;
	virtual void Tick( float DeltaSeconds ) override;

	//////////////////////////////////////////////// IMPLEMENTABLES /////////////////////////////////////////


	UFUNCTION(BlueprintImplementableEvent, Category = "TGOR|Inventory", Meta = (Keywords = "C++"))
		void OnDrop();


	UFUNCTION(BlueprintImplementableEvent, Category = "TGOR|Inventory", Meta = (Keywords = "C++"))
		void UpdateDisplay(const FTGOR_Display& Display);

	////////////////////////////////////////////// COMPONENTS //////////////////////////////////////////////////////

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Actor, meta = (AllowPrivateAccess = "true"))
		UTGOR_InteractableComponent* Interactable;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Actor, meta = (AllowPrivateAccess = "true"))
		UTGOR_SingleItemComponent* Container;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Actor, meta = (AllowPrivateAccess = "true"))
		UTGOR_SaveComponent* SaveComponent;

	////////////////////////////////////////////////////////////////////////////////////////////////////


	UFUNCTION(BlueprintCallable, Category = "TGOR|Inventory", Meta = (Keywords = "C++"))
		void Combine(ATGOR_ItemDropActor* Drop);

	UFUNCTION(Category = "TGOR|Inventory", Meta = (Keywords = "C++"))
		void UpdateItem();


	UFUNCTION(BlueprintCallable, Category = "TGOR|Inventory", Meta = (Keywords = "C++"))
		UTGOR_Item* EnsureItem();

	////////////////////////////////////////////////////////////////////////////////////////////////////
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TGOR|Inventory")
		bool DestroyOnEmpty;

	UPROPERTY(BlueprintReadOnly, Category = "TGOR|Inventory")
		UTGOR_Item* CurrentItem;

	/////////////////////////////////////////////// INTERNAL ///////////////////////////////////////////

protected:

private:

	UTGOR_Item* _item;

};