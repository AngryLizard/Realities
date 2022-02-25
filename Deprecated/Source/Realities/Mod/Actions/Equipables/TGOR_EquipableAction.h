// The Gateway of Realities: Planes of Existence.

#pragma once

#include "Realities/Utility/TGOR_CustomEnumerations.h"
#include "Realities/Base/Instances/System/TGOR_ActionInstance.h"
#include "Realities/Base/Instances/Combat/TGOR_AimInstance.h"

#include "Realities/Mod/Actions/TGOR_Action.h"
#include "TGOR_EquipableAction.generated.h"

/**
 *
 */

UENUM(BlueprintType)
enum class ETGOR_AimIgnoreEnumeration : uint8
{
	All,
	IgnoreSelf,
	OnlySelf
};


/**
*
*/
UCLASS()
class REALITIES_API UTGOR_EquipableAction : public UTGOR_Action
{
	GENERATED_BODY()

public:

	////////////////////////////////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////// ENGINE ////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////////////////////////////

	UTGOR_EquipableAction();

	virtual bool Prevariant(UTGOR_ActionComponent* Component) const override;
	virtual bool Invariant(UTGOR_ActionStorage* Storage, UTGOR_ActionComponent* Component) const override;
	virtual void Activate(UTGOR_ActionStorage* Storage, UTGOR_ActionComponent* Component) override;
	virtual bool Equip(UTGOR_ActionStorage* Storage, UTGOR_ActionComponent* Component, float Deltatime) override;
	virtual void ForceEquip(UTGOR_ActionStorage* Storage, UTGOR_ActionComponent* Component) override;
	virtual bool Operate(UTGOR_ActionStorage* Storage, UTGOR_ActionComponent* Component, float Deltatime) override;
	virtual bool Unequip(UTGOR_ActionStorage* Storage, UTGOR_ActionComponent* Component, float Deltatime) override;
	virtual void ForceUnequip(UTGOR_ActionStorage* Storage, UTGOR_ActionComponent* Component) override;

	//////////////////////////////////////////// IMPLEMENTABLES ////////////////////////////////////////
	
	/** Returns true if this action can be called */
	UFUNCTION(BlueprintImplementableEvent, Category = "TGOR Action", Meta = (Keywords = "C++"))
		void CanTick(UTGOR_ActionStorage* Storage, UTGOR_ActionComponent* Component, ETGOR_InvarianceEnumeration& Branches) const;

	/** Called once before action starts equipping */
	UFUNCTION(BlueprintImplementableEvent, Category = "TGOR Action", Meta = (Keywords = "C++"))
		void PrepareTick(UTGOR_ActionStorage* Storage, UTGOR_ActionComponent* Component);

	/** Called while action is equipping */
	UFUNCTION(BlueprintImplementableEvent, Category = "TGOR Action", Meta = (Keywords = "C++"))
		void AwaitStartTick(UTGOR_ActionStorage* Storage, UTGOR_ActionComponent* Component, float Deltatime, ETGOR_WaitEnumeration& Branches);

	/** Called when action started ticking */
	UFUNCTION(BlueprintImplementableEvent, Category = "TGOR Action", Meta = (Keywords = "C++"))
		void StartTick(UTGOR_ActionStorage* Storage, UTGOR_ActionComponent* Component);

	/** Called while action is active */
	UFUNCTION(BlueprintImplementableEvent, Category = "TGOR Action", Meta = (Keywords = "C++"))
		void Tick(UTGOR_ActionStorage* Storage, UTGOR_ActionComponent* Component, float Deltatime, ETGOR_TerminationEnumeration& Branches);

	/** Called while action is unequipping */
	UFUNCTION(BlueprintImplementableEvent, Category = "TGOR Action", Meta = (Keywords = "C++"))
		void AwaitEndTick(UTGOR_ActionStorage* Storage, UTGOR_ActionComponent* Component, float Deltatime, ETGOR_WaitEnumeration& Branches);

	/** Called when action ended ticking */
	UFUNCTION(BlueprintImplementableEvent, Category = "TGOR Action", Meta = (Keywords = "C++"))
		void EndTick(UTGOR_ActionStorage* Storage, UTGOR_ActionComponent* Component);
	
	////////////////////////////////////////////////////////////////////////////////////////////////////

	/** Checks current aim whether target is in current action insertion list and in range */
	UFUNCTION(BlueprintPure, Category = "TGOR Action", Meta = (Keywords = "C++"))
		void CheckAim(UTGOR_Component* Component, ETGOR_AimIgnoreEnumeration Ignore, bool& Valid, FTGOR_AimInstance& Instance, FVector& Location, UTGOR_AimComponent*& Aim) const;

	/** Checks current movement whether mode is in current action insertion list */
	UFUNCTION(BlueprintPure, Category = "TGOR Action", Meta = (Keywords = "C++"))
		void CheckMovement(UTGOR_Component* Component, bool& Valid, UTGOR_MovementComponent*& Movement) const;

	/** Filters aim manually */
	UFUNCTION(BlueprintCallable, Category = "TGOR Action", Meta = (Keywords = "C++"))
		void FilterAim(UTGOR_ActionComponent* Component, TSubclassOf<UTGOR_Target> Filter);

	/** Fixes aim manually */
	UFUNCTION(BlueprintCallable, Category = "TGOR Action", Meta = (Keywords = "C++"))
		void FixAim(UTGOR_ActionComponent* Component);

	/** Unfixes/Unfilters aim manually */
	UFUNCTION(BlueprintCallable, Category = "TGOR Action", Meta = (Keywords = "C++"))
		void FreeAim(UTGOR_ActionComponent* Component);

	/** Filters aim */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "TGOR Action", meta = (EditCondition = "!FixesAim"))
		TSubclassOf<UTGOR_Target> AimFilter;

	/** Fixes aim */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "TGOR Action")
		bool FixesAim;

protected:

private:
};
