// TGOR (C) // CHECKED //
#pragma once

#include "CoreMinimal.h"
#include "Net/UnrealNetwork.h"

#include "MovementSystem/TGOR_MovementInstance.h"
#include "DimensionSystem/TGOR_MobilityInstance.h"
#include "PhysicsSystem/TGOR_PhysicsInstance.h"

#include "AnimationSystem/Tasks/TGOR_AnimatedTask.h"
#include "TGOR_MovementTask.generated.h"

////////////////////////////////////////////// DECL //////////////////////////////////////////////////////

class UTGOR_Movement;
class UTGOR_MovementComponent;
class UTGOR_AttachComponent;

/**
*
*/
USTRUCT(BlueprintType)
struct MOVEMENTSYSTEM_API FTGOR_MovementTaskIdentifier
{
	GENERATED_USTRUCT_BODY();

	/** Movement content */
	UPROPERTY(BlueprintReadOnly, Category = "!TGOR Movement")
		UTGOR_Movement* Content = nullptr;

	/** Owning movement component */
	UPROPERTY(BlueprintReadOnly, Category = "!TGOR Movement")
		UTGOR_MovementComponent* Component = nullptr;

	/** Slot this movement resides in */
	UPROPERTY(BlueprintReadOnly, Category = "!TGOR Movement")
		int32 Slot = -1;
};

/**
* 
*/
UCLASS(Abstract, Blueprintable)
class MOVEMENTSYSTEM_API UTGOR_MovementTask : public UTGOR_AnimatedTask
{
	GENERATED_BODY()

public:
	
	////////////////////////////////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////// ENGINE ////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////////////////////////////

	UTGOR_MovementTask();
	void GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const override;
	virtual TScriptInterface<ITGOR_AnimationInterface> GetAnimationOwner() const override;

	////////////////////////////////////////////////////////////////////////////////////////////////////
public:

	/** Gets assigned movement content */
	UFUNCTION(BlueprintPure, Category = "!TGOR Movement", Meta = (Keywords = "C++"))
		UTGOR_Movement* GetMovement() const;

	/** Gets assigned movement component */
	UFUNCTION(BlueprintPure, Category = "!TGOR Movement", Meta = (Keywords = "C++"))
		UTGOR_MovementComponent* GetOwnerComponent() const;

	/** Gets assigned movement slot index */
	UFUNCTION(BlueprintPure, Category = "!TGOR Movement", Meta = (Keywords = "C++"))
		int32 GetSlotIndex() const;

	/** Get current speed ratio to apply to maximum speed where applicable */
	UFUNCTION(BlueprintPure, Category = "!TGOR Movement", Meta = (Keywords = "C++"))
		float GetSpeedRatio() const;

	/** Get attribute for movement */
	UFUNCTION(BlueprintPure, Category = "!TGOR Movement", Meta = (Keywords = "C++"))
		float GetAttribute(TSubclassOf<UTGOR_Attribute> Type, float Default) const;

	////////////////////////////////////////////////////////////////////////////////////////////////////
public:

	/** NavMovement interface */
	virtual float GetMaxSpeed() const;

	////////////////////////////////////////////////////////////////////////////////////////////////////
public:

	/** Called once when this task is created */
	virtual void Initialise();

	/** Called once movement is invoked */
	virtual void PrepareStart();

	/** Called every tick to check whether this task is allowed be running */
	virtual bool Invariant(const FTGOR_MovementSpace& Space, const FTGOR_MovementExternal& External) const;

	/** Called once when this task became active */
	virtual void Reset(const FTGOR_MovementSpace& Space, const FTGOR_MovementExternal& External);

	/** Get current input vector and view, only called locally */
	virtual void QueryInput(FVector& OutInput, FVector& OutView) const;

	/** Process movement, only called once per engine tick (opposed to simulation tick) */
	virtual void Process(float DeltaTime);

	/** Simulate one movement tick */
	virtual void Update(const FTGOR_MovementSpace& Space, const FTGOR_MovementExternal& External, const FTGOR_MovementTick& Tick, FTGOR_MovementOutput& Output);

	/** Called once movement is stopped */
	virtual void Interrupt();

	/** Tick animation */
	virtual void Animate(const FTGOR_MovementSpace& Space, float DeltaTime);

	////////////////////////////////////////////////////////////////////////////////////////////////////

	/** Called every frame to update animation (multiple movement ticks can happen per animation frame). */
	UFUNCTION(BlueprintImplementableEvent, Category = "!TGOR Movement", Meta = (Keywords = "C++"))
		void OnAnimate(const FTGOR_MovementSpace& Space, float DeltaTime);

	/** Called once when this task is assigned to a component. Should usually only happen once. */
	UFUNCTION(BlueprintImplementableEvent, Category = "!TGOR Movement", Meta = (Keywords = "C++"))
		void OnInitialise();

	////////////////////////////////////////////////////////////////////////////////////////////////////
public:

	/** Owning movement component */
	UPROPERTY(ReplicatedUsing = RepNotifyIdentifier, Transient, BlueprintReadOnly, Category = "!TGOR Movement")
		FTGOR_MovementTaskIdentifier Identifier;

	/** Initialise parenting */
	UFUNCTION()
		void RepNotifyIdentifier()
	{
		Initialise();
	}

};
