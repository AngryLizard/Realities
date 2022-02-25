// The Gateway of Realities: Planes of Existence.

#pragma once

#include "Realities/Utility/TGOR_CustomEnumerations.h"
#include "Realities/Base/Instances/System/TGOR_ActionInstance.h"
#include "Realities/Base/Instances/Combat/TGOR_AimInstance.h"

#include "Realities/Mod/Unlocks/Modifiers/Actions/TGOR_Action.h"
#include "TGOR_EquipableAction.generated.h"

class UTGOR_NamedSocket;
class UTGOR_Movement;
class UTGOR_Target;

class UTGOR_AimComponent;
class UTGOR_SocketComponent;
class UTGOR_MovementComponent;


/**
*
*/
USTRUCT(BlueprintType)
struct FTGOR_EquipmentAim
{
	GENERATED_USTRUCT_BODY()

		FTGOR_EquipmentAim();

	/** Own Aim */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		UTGOR_AimComponent* Aim;

	/** Own Movement */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		UTGOR_MovementComponent* Movement;

	/** Aim location */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FVector Location;

	/** Aim instance */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FTGOR_AimInstance Instance;
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

	//////////////////////////////////////////// IMPLEMENTABLES ////////////////////////////////////////

	/** Condition if action can be started.
		If false user can neither schedule this action
		nor will it be visible for players.*/
	UFUNCTION(BlueprintImplementableEvent, Category = "!TGOR Action", Meta = (Keywords = "C++"))
		void Precondition(UTGOR_ActionComponent* Component, const FTGOR_EquipmentAim& Data, ETGOR_ValidEnumeration& Branches) const;

	/** Condition if action can keep running.
		If false, action calls interrupt and terminates instantly. */
	UFUNCTION(BlueprintNativeEvent, Category = "!TGOR Action", Meta = (Keywords = "C++"))
		void Invariant(UTGOR_ActionTask* Storage, UTGOR_ActionComponent* Component, ETGOR_ValidEnumeration& Branches) const;
	virtual void Invariant_Implementation(UTGOR_ActionTask* Storage, UTGOR_ActionComponent* Component, ETGOR_ValidEnumeration& Branches) const;

	/** Called once when action starts, followed by PrepareState. */
	UFUNCTION(BlueprintImplementableEvent, Category = "!TGOR Action", Meta = (Keywords = "C++"))
		void OnPrepareStart(UTGOR_ActionTask* Storage, UTGOR_ActionComponent* Component);

	/** This state will be run either for MaxPrepareTime seconds or until
		NextState is returned. Then transitions into OperateStart.
		Usage example: Playing animations like equipping weapons. */
	UFUNCTION(BlueprintImplementableEvent, Category = "!TGOR Action", Meta = (Keywords = "C++"))
		void OnPrepare(UTGOR_ActionTask* Storage, UTGOR_ActionComponent* Component, float Deltatime, ETGOR_StateEnumeration& Branches);

	/** Called once after transitioning from PrepareState, followed by OperateState. */
	UFUNCTION(BlueprintImplementableEvent, Category = "!TGOR Action", Meta = (Keywords = "C++"))
		void OnOperateStart(UTGOR_ActionTask* Storage, UTGOR_ActionComponent* Component);

	/** This state will be run indefinitely until NextState is returned.
		Do stuff here. */
	UFUNCTION(BlueprintImplementableEvent, Category = "!TGOR Action", Meta = (Keywords = "C++"))
		void OnOperate(UTGOR_ActionTask* Storage, UTGOR_ActionComponent* Component, float Deltatime, ETGOR_StateEnumeration& Branches);

	/** Called once after transitioning from OperateState followed by FinishState. */
	UFUNCTION(BlueprintImplementableEvent, Category = "!TGOR Action", Meta = (Keywords = "C++"))
		void OnFinishStart(UTGOR_ActionTask* Storage, UTGOR_ActionComponent* Component);

	/** This state will be run for MaxFinishTime seconds to then
		transition into Interrupt.
		Usage example: Playing animations like unequipping weapons. */
	UFUNCTION(BlueprintImplementableEvent, Category = "!TGOR Action", Meta = (Keywords = "C++"))
		void OnFinish(UTGOR_ActionTask* Storage, UTGOR_ActionComponent* Component, float Deltatime, ETGOR_StateEnumeration& Branches);

	/** Always called when action is stopped/descheduled, even if invariant is false.
		Used for cleanup after action call.
		Usage example: Stopping sounds/particle effects. */
	UFUNCTION(BlueprintImplementableEvent, Category = "!TGOR Action", Meta = (Keywords = "C++"))
		void OnInterrupt(UTGOR_ActionTask* Storage, UTGOR_ActionComponent* Component);



protected:

private:
};
