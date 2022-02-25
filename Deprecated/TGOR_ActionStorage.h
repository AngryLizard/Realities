#pragma once

#include "CoreMinimal.h"
#include "Realities/Base/Instances/System/TGOR_IndexInstance.h"
#include "Realities/Base/Instances/System/TGOR_ActionInstance.h"
#include "Realities/Utility/TGOR_CustomEnumerations.h"

#include "Realities/Base/Inventory/TGOR_Storage.h"
#include "TGOR_ActionStorage.generated.h"

class UTGOR_Action;
class UTGOR_ActionComponent;


/**
 * The abstract base class for all action storages.
 */
UCLASS(Blueprintable)
class REALITIES_API UTGOR_ActionStorage : public UTGOR_Storage
{
	GENERATED_BODY()

	friend struct FTGOR_ActionInstance; 
	friend class UTGOR_Action;

public:

	UTGOR_ActionStorage();
	virtual void Clear() override;
	virtual FString Print() const override;
	virtual bool Compatible(const UTGOR_Container* Other) const override;

	virtual void Write_Implementation(FTGOR_GroupWritePackage& Package, UTGOR_Singleton* Context) const override;
	virtual bool Read_Implementation(FTGOR_GroupReadPackage& Package, UTGOR_Singleton* Context) override;
	virtual void Send_Implementation(FTGOR_NetworkWritePackage& Package, UTGOR_Singleton* Context) const override;
	virtual bool Recv_Implementation(FTGOR_NetworkReadPackage& Package, UTGOR_Singleton* Context) override;

	virtual bool Equals(const UTGOR_Storage* Other) const override;
	virtual bool Merge(const UTGOR_Storage* Other, ETGOR_NetvarEnumeration Priority) override;
	virtual UTGOR_Content* GetOwner() const override;

	////////////////////////////////////////////////////////////////////////////////////////////////////
	
	/** Get current action state */
	UFUNCTION(BlueprintPure, Category = "!TGOR Actions", Meta = (Keywords = "C++"))
		ETGOR_ActionStateEnumeration GetState() const;

	/** Get current slot index */
	UFUNCTION(BlueprintPure, Category = "!TGOR Actions", Meta = (Keywords = "C++"))
		int32 GetSlotIdentifier() const;

	/** Returns duration since last ActionState changed */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Actions", Meta = (Keywords = "C++"))
		float GetDuration() const;

	/** Returns whether this storage is currently playing in given component */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Actions", Meta = (Keywords = "C++"))
		bool IsRunningIn(UTGOR_ActionComponent* Component) const;

	////////////////////////////////////////////////////////////////////////////////////////////////////

	/** Gets this action's slot item */
	UFUNCTION(BlueprintPure, Category = "!TGOR Actions", Meta = (Keywords = "C++"))
		UTGOR_ItemStorage* GetCurrentItem(UTGOR_ActionComponent* Component) const;

	/** Swaps an item with this action's slot item */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Actions", Meta = (Keywords = "C++"))
		UTGOR_ItemStorage* SwapWithCurrentItem(UTGOR_ActionComponent* Component, UTGOR_ItemStorage* ItemStorage);

	/** Takes this action's slot item */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Actions", Meta = (Keywords = "C++"))
		UTGOR_ItemStorage* TakeCurrentItem(UTGOR_ActionComponent* Component);

	/** Checks whether given quantity of matter is available to be depleted */
	UFUNCTION(BlueprintPure, Category = "!TGOR Actions", Meta = (Keywords = "C++"))
		bool HasMatter(UTGOR_ActionComponent* Component, UTGOR_Matter* Matter, int32 Quantity) const;

	/** Remove given quantity of matter from current item, removed item if empty, returns removed quantity */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Actions", Meta = (Keywords = "C++"))
		int32 DepleteMatter(UTGOR_ActionComponent* Component, UTGOR_Matter* Matter, int32 Quantity);

	////////////////////////////////////////////////////////////////////////////////////////////////////

	/** Prepare trigger structure for Update calls */
	void Prepare(UTGOR_ActionComponent* Component, int32 SlotIdentifier);

	/** Updates this action, deschedules itself if invariance becomes false */
	bool Update(UTGOR_ActionComponent* Component, float Deltatime);
	
	/** Forwards an ActionTrigger to a given state, calling obligatory events in between */
	bool Forward(UTGOR_ActionComponent* Component, ETGOR_ActionStateEnumeration State);

	/** Interrupt this action and clears Trigger */
	void Interrupt(UTGOR_ActionComponent* Component);

	/** Updates storage to server/client. */
	void Replicate(UTGOR_ActionComponent* Component);

	/** Updates server storage with given storage class from client. */
	void Integrate(UTGOR_ActionComponent* Component, UTGOR_ActionStorage* Storage);

	////////////////////////////////////////////////////////////////////////////////////////////////////

	/** Return the content action that is stored inside this storage. */
	UFUNCTION(BlueprintPure, Category = "!TGOR Storage", Meta = (Keywords = "C++"))
		virtual UTGOR_Action* GetAction() const;

	////////////////////////////////////////////////////////////////////////////////////////////////////
protected:
	
	/** Schedule time */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FTGOR_Time Time;

	/** Local state of the action, not replicated for fast-forward. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		ETGOR_ActionStateEnumeration LocalState;

	/** Target state of the action, replicated. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		ETGOR_ActionStateEnumeration TargetState;

	/** Action slot in ActionComponent */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		int32 Identifier;

	////////////////////////////////////////////////////////////////////////////////////////////////////

	/** The item of this storage. */
	UPROPERTY(VisibleAnywhere, Category = "!TGOR Storage")
		UTGOR_Action* ActionRef;
};