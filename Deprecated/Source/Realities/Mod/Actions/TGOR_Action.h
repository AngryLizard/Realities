// TGOR (C) // CHECKED //
#pragma once

#include "CoreMinimal.h"
#include "Realities/Base/Instances/System/TGOR_ActionInstance.h"
#include "Realities/Base/Instances/Inventory/TGOR_StorageInstance.h"
#include "Runtime/GameplayTags/Classes/GameplayTagContainer.h"
#include "Templates/SubclassOf.h"

#include "Realities/Base/Content/TGOR_Content.h"
#include "TGOR_Action.generated.h"

////////////////////////////////////////////// DECL //////////////////////////////////////////////////////

class ATGOR_CreatureCharacter;
class UTGOR_Animation;
class UTGOR_Matter;

/**
* TGOR_Action handles any kind of replicated actions
*/
UCLASS()
class REALITIES_API UTGOR_Action : public UTGOR_Content
{
	GENERATED_BODY()

		friend class SGATGORNode_ActionParamPin;
		friend class SGATGORNode_ActionEventPin;
		friend class UTGOR_ActionStorage;

public:
	
	////////////////////////////////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////// ENGINE ////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////////////////////////////

	UTGOR_Action();
	virtual void PostBuildResource() override;

	/**
	* Create an empty storage instance for this action.
	* @see TGOR_Storage
	*/
	UFUNCTION(BlueprintCallable, Category = "TGOR Inventory", Meta = (Keywords = "C++"))
		virtual UTGOR_ActionStorage* CreateStorage();

	/** Build Storage modules and set their default values */
	UFUNCTION(BlueprintCallable, Category = "TGOR Inventory", Meta = (Keywords = "C++"))
		virtual void BuildStorage(UTGOR_ActionStorage* Storage);

	/** Change values and module values of a created storage object */
	UFUNCTION(BlueprintImplementableEvent, Category = "TGOR Inventory", Meta = (Keywords = "C++"))
		void InitialiseStorage(UTGOR_ActionStorage* Storage);

	////////////////////////////////////////////////////////////////////////////////////////////////////
protected:

	/** Modules that are part of this item storage */
	UPROPERTY(EditAnywhere, Category = "TGOR Actions")
		FTGOR_StorageDeclaration Modules;


	/** Cooldown for this action in seconds */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "TGOR Actions")
		float ScheduleCooldown;

	/** Tag which to filter for (or none if empty) */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "TGOR Actions")
		FGameplayTag FilterTag;

	/** How to replicate action payload */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		ETGOR_ReplicationType Importance;

	/** H */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		ETGOR_NotifyReplication Notify;

	/** Type of item supported by this action */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "TGOR Actions")
		FTGOR_ItemRestriction SupportedItem;

	////////////////////////////////////////////////////////////////////////////////////////////////////
public:

	/** Whether this action sends its payload reliably */
	UFUNCTION(BlueprintPure, Category = "TGOR Actions", Meta = (Keywords = "C++"))
		bool IsReliable() const;

	/** Checks whether this action supports a given item */
	UFUNCTION(BlueprintPure, Category = "TGOR Actions", Meta = (Keywords = "C++"))
		bool SupportsItem(UTGOR_Item* Item) const;
	
	/** Returns whether action can be called in this context (correct slot, cooldown etc) */
	UFUNCTION(BlueprintCallable, Category = "TGOR Actions", Meta = (Keywords = "C++"))
		virtual bool CanCall(UTGOR_ActionComponent* Component, const FTGOR_SlotIndex& SlotIndex);

	////////////////////////////////////////////////////////////////////////////////////////////////////
protected:

	/** Base animation module of this action, gets played automatically when scheduled */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TSubclassOf<UTGOR_Animation> MainAnimation;

	/** Max time for equip */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "TGOR System")
		float MaxEquipTime;

	/** Max time for unequip */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "TGOR System")
		float MaxUnequipTime;

	////////////////////////////////////////////////////////////////////////////////////////////////////
public:

	/** Invariant needs to hold before equip and during operation */
	virtual bool Prevariant(UTGOR_ActionComponent* Component) const;

	/** Invariant needs to hold during operation */
	virtual bool Invariant(UTGOR_ActionStorage* Storage, UTGOR_ActionComponent* Component) const;

	/** Gets called before equip */
	virtual void Activate(UTGOR_ActionStorage* Storage, UTGOR_ActionComponent* Component);

	/** Equip until MaxEquipTime is reached or this returns false (Gets called at least once) */
	virtual bool Equip(UTGOR_ActionStorage* Storage, UTGOR_ActionComponent* Component, float Deltatime);

	/** Gets called in between equip and operation to ensure operation, skips operation if false */
	virtual void ForceEquip(UTGOR_ActionStorage* Storage, UTGOR_ActionComponent* Component);

	/** Operate as long as invariant holds. Active (Can call subactions) while returns true */
	virtual bool Operate(UTGOR_ActionStorage* Storage, UTGOR_ActionComponent* Component, float Deltatime);

	/** Unequip until MaxUnequipTime is reached or this returns false (Gets called at least once) */
	virtual bool Unequip(UTGOR_ActionStorage* Storage, UTGOR_ActionComponent* Component, float Deltatime);

	/** Gets called after unequip to ensure proper removal, also gets called if invariant is false. */
	virtual void ForceUnequip(UTGOR_ActionStorage* Storage, UTGOR_ActionComponent* Component);


	/////////////////////////////////////////////// INTERNAL ///////////////////////////////////////////

protected:
private:

};
