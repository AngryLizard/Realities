// TGOR (C) // CHECKED //
#pragma once

#include "CoreMinimal.h"
#include "Net/UnrealNetwork.h"

#include "InventorySystem/TGOR_ProcessInstance.h"
#include "DimensionSystem/TGOR_MobilityInstance.h"
#include "PhysicsSystem/TGOR_PhysicsInstance.h"

#include "CoreSystem/Tasks/TGOR_Task.h"
#include "TGOR_ProcessTask.generated.h"

////////////////////////////////////////////// DECL //////////////////////////////////////////////////////

class UTGOR_Process;
class UTGOR_ProcessComponent;

/**
*
*/
USTRUCT(BlueprintType)
struct INVENTORYSYSTEM_API FTGOR_ProcessTaskIdentifier
{
	GENERATED_USTRUCT_BODY();

	/** Process content */
	UPROPERTY(BlueprintReadOnly, Category = "!TGOR Inventory")
		UTGOR_Process* Content = nullptr;

	/** Owning Process component */
	UPROPERTY(BlueprintReadOnly, Category = "!TGOR Inventory")
		UTGOR_ProcessComponent* Component = nullptr;

	/** Slot this Process resides in */
	UPROPERTY(BlueprintReadOnly, Category = "!TGOR Inventory")
		int32 Slot = -1;
};

/**
*
*/
UCLASS(Abstract, Blueprintable)
class INVENTORYSYSTEM_API UTGOR_ProcessTask : public UTGOR_Task
{
	GENERATED_BODY()

public:

	////////////////////////////////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////// ENGINE ////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////////////////////////////

	UTGOR_ProcessTask();
	void GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const override;

	////////////////////////////////////////////////////////////////////////////////////////////////////
public:

	/** Gets assigned Process content */
	UFUNCTION(BlueprintPure, Category = "!TGOR Inventory", Meta = (Keywords = "C++"))
		UTGOR_Process* GetProcess() const;

	/** Gets assigned Process component */
	UFUNCTION(BlueprintPure, Category = "!TGOR Inventory", Meta = (Keywords = "C++"))
		UTGOR_ProcessComponent* GetOwnerComponent() const;

	/** Gets assigned Process slot index */
	UFUNCTION(BlueprintPure, Category = "!TGOR Inventory", Meta = (Keywords = "C++"))
		int32 GetSlotIndex() const;

	////////////////////////////////////////////////////////////////////////////////////////////////////
public:

	/** Called once when this task is created */
	virtual void Initialise();

	////////////////////////////////////////////////////////////////////////////////////////////////////
protected:

	/** Called once when this task is assigned to a component. Should usually only happen once. */
	UFUNCTION(BlueprintImplementableEvent, Category = "!TGOR Inventory", Meta = (Keywords = "C++"))
		void OnInitialise();

	////////////////////////////////////////////////////////////////////////////////////////////////////
public:


	////////////////////////////////////////////////////////////////////////////////////////////////////
public:

	/** Owning Process component */
	UPROPERTY(ReplicatedUsing = RepNotifyIdentifier, Transient, BlueprintReadOnly, Category = "!TGOR Inventory")
		FTGOR_ProcessTaskIdentifier Identifier;

	/** Initialise parenting */
	UFUNCTION()
		void RepNotifyIdentifier()
	{
		Initialise();
	}
};
