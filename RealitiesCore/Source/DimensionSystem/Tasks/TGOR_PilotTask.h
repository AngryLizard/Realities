// TGOR (C) // CHECKED //
#pragma once

#include "CoreMinimal.h"
#include "Net/UnrealNetwork.h"

#include "../TGOR_PilotInstance.h"

#include "../Interfaces/TGOR_BufferInterface.h"
#include "CoreSystem/Tasks/TGOR_Task.h"
#include "TGOR_PilotTask.generated.h"

////////////////////////////////////////////// DECL //////////////////////////////////////////////////////

class UTGOR_Pilot;
class UTGOR_PilotComponent;

/**
*
*/
USTRUCT(BlueprintType)
struct DIMENSIONSYSTEM_API FTGOR_PilotTaskIdentifier
{
	GENERATED_USTRUCT_BODY();

	/** Pilot content */
	UPROPERTY(BlueprintReadOnly, Category = "!TGOR Movement")
		UTGOR_Pilot* Content = nullptr;

	/** Owning pilot component */
	UPROPERTY(BlueprintReadOnly, Category = "!TGOR Movement")
		UTGOR_PilotComponent* Component = nullptr;

	/** Slot this pilot resides in */
	UPROPERTY(BlueprintReadOnly, Category = "!TGOR Movement")
		int32 Slot = -1;
};

/**
* 
*/
UCLASS(Abstract, Blueprintable)
class DIMENSIONSYSTEM_API UTGOR_PilotTask : public UTGOR_Task
{
	GENERATED_BODY()

public:
	
	////////////////////////////////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////// ENGINE ////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////////////////////////////

	UTGOR_PilotTask();
	void GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const override;
	virtual void BeginDestroy() override;

	////////////////////////////////////////////////////////////////////////////////////////////////////
public:

	/** Gets assigned pilot content */
	UFUNCTION(BlueprintPure, Category = "!TGOR Movement", Meta = (Keywords = "C++"))
		UTGOR_Pilot* GetPilot() const;

	/** Gets assigned pilot component */
	UFUNCTION(BlueprintPure, Category = "!TGOR Movement", Meta = (Keywords = "C++"))
		UTGOR_PilotComponent* GetOwnerComponent() const;

	/** Gets assigned pilot slot index */
	UFUNCTION(BlueprintPure, Category = "!TGOR Movement", Meta = (Keywords = "C++"))
		int32 GetSlotIndex() const;

	////////////////////////////////////////////////////////////////////////////////////////////////////

	/** Get mobility this storage as parented to. */
	UFUNCTION(BlueprintPure, Category = "!TGOR Movement", Meta = (Keywords = "C++"))
		virtual UTGOR_MobilityComponent* GetParent() const;

	/** Get current base transform of this socket. */
	UFUNCTION(BlueprintPure, Category = "!TGOR Movement", Meta = (Keywords = "C++"))
		virtual FTGOR_MovementDynamic ComputeBase() const;

	/** Get current position of this socket. */
	UFUNCTION(BlueprintPure, Category = "!TGOR Movement", Meta = (Keywords = "C++"))
		virtual FTGOR_MovementPosition ComputePosition() const;

	/** Get current space of this socket. */
	UFUNCTION(BlueprintPure, Category = "!TGOR Movement", Meta = (Keywords = "C++"))
		virtual FTGOR_MovementSpace ComputeSpace() const;

	/** Tick this pilot */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Movement", Meta = (Keywords = "C++"))
		virtual void Update(float DeltaTime);

	/** Call to detach this task from its current parent */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Movement", Meta = (Keywords = "C++"))
		virtual void Unparent();

	////////////////////////////////////////////////////////////////////////////////////////////////////
public:

	virtual bool Initialise();

	////////////////////////////////////////////////////////////////////////////////////////////////////

	/** Called once when this task is assigned to a component. Should usually only happen once. */
	UFUNCTION(BlueprintImplementableEvent, Category = "!TGOR Movement", Meta = (Keywords = "C++"))
		void OnInitialise();

	////////////////////////////////////////////////////////////////////////////////////////////////////
public:

	/** Owning pilot component */
	UPROPERTY(ReplicatedUsing = RepNotifyIdentifier, Transient, BlueprintReadOnly, Category = "!TGOR Movement")
		FTGOR_PilotTaskIdentifier Identifier;

	/** Initialise parenting */
	UFUNCTION()
		void RepNotifyIdentifier()
	{
		Initialise();
	}

	////////////////////////////////////////////////////////////////////////////////////////////////////
public:

	/** Call to set pilot state to active (register to parent) */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Movement", Meta = (Keywords = "C++"))
		virtual void Register();

	/** Call to set pilot state to inactive (unregister from parent) */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Movement", Meta = (Keywords = "C++"))
		virtual void Unregister();

	/** Whether this pilot is currently active */
	UFUNCTION(BlueprintPure, Category = "!TGOR Movement", Meta = (Keywords = "C++"))
		bool IsRegistered() const;

	////////////////////////////////////////////////////////////////////////////////////////////////////
public:

	/** Set influence this pilot applies to its parent */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Movement", Meta = (Keywords = "C++"))
		void SetInfluence(const FTGOR_MovementForce& Force);

	/** Get influence this pilot applies to its parent */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Movement", Meta = (Keywords = "C++"))
		const FTGOR_MovementForce& GetInfluence() const;

	////////////////////////////////////////////////////////////////////////////////////////////////////
private:

	/** Whether this pilot is currently active (usually if active slot) */
	UPROPERTY()
		bool IsRegisteredToParent;

	/** Force this pilot applies to its parent */
	UPROPERTY()
		FTGOR_MovementForce Influence;
};
