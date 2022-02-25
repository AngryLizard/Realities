// TGOR (C) // CHECKED //
#pragma once

#include "Realities/Base/Instances/Creature/TGOR_MovementInstance.h"

#include "Realities/Base/Content/TGOR_Content.h"
#include "TGOR_Movement.generated.h"

////////////////////////////////////////////// DECL //////////////////////////////////////////////////////

class UTGOR_Animation;
class UTGOR_MovementComponent;

/**
* TGOR_Movement is a movement mode the MovementComponent activates and ticks
*/
UCLASS()
class REALITIES_API UTGOR_Movement : public UTGOR_Content
{
	GENERATED_BODY()

public:
	
	UTGOR_Movement();
	virtual void PostBuildResource() override;

	////////////////////////////////////////////////////////////////////////////////////////////////////

	/** Fill override lists */
	UFUNCTION(BlueprintCallable, Category = "TGOR Movement", Meta = (Keywords = "C++"))
		void OverrideMovements(UTGOR_Movement* Override);

	/** Whether angular velocity is affected by collisions */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TGOR Movement")
		bool CanRotateOnCollision;
	
	/** Mode of this movement */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TGOR Movement")
		ETGOR_MovementEnumeration Mode;

	/** Base animation module of this movement used when GetAnimation isn't overridden */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TGOR Movement")
		TSubclassOf<UTGOR_Animation> MainAnimation;

	////////////////////////////////////////////////////////////////////////////////////////////////////
public:

	/** Get current input vector, only called locally */
	UFUNCTION(BlueprintCallable, Category = "TGOR Movement", Meta = (Keywords = "C++"))
		virtual void QueryInputVector(const UTGOR_MovementComponent* Component, FVector& OutInput, FVector& OutView) const;

	/** Tick movement with a given space and state over a given time. Shouldn't have sideeffects when replaying. */
	UFUNCTION(BlueprintCallable, Category = "TGOR Movement", Meta = (Keywords = "C++"))
		virtual void Tick(UTGOR_MovementComponent* Component, const FTGOR_MovementTick& Tick, const FTGOR_MovementSpace& Space, const FTGOR_MovementExternal& External, bool Replay, FTGOR_MovementOutput& Out);

	/** True if movement is available */
	UFUNCTION(BlueprintCallable, Category = "TGOR Movement", Meta = (Keywords = "C++"))
		virtual bool Invariant(const UTGOR_MovementComponent* Component, const FTGOR_MovementTick& Tick, const FTGOR_MovementSpace& Space, const FTGOR_MovementExternal& External) const;

	/** Get animation to be used at a given point in time */
	UFUNCTION(BlueprintPure, Category = "TGOR Movement", Meta = (Keywords = "C++"))
		virtual UTGOR_Animation* QueryAnimation(const UTGOR_MovementComponent* Component, const FTGOR_MovementTick& Tick, const FTGOR_MovementSpace& Space, const FTGOR_MovementExternal& External) const;

	/** Check whether this movement is overriden by a given movement */
	UFUNCTION(BlueprintPure, Category = "TGOR Movement", Meta = (Keywords = "C++"))
		bool IsOverriddenBy(UTGOR_Movement* Movement) const;

	////////////////////////////////////////////////////////////////////////////////////////////////////
protected:

	/** Grab animation information */
	UFUNCTION(BlueprintPure, Category = "TGOR Movement", Meta = (Keywords = "C++"))
		void GetAnimation(const UTGOR_MovementComponent* Component, UTGOR_Animation*& Animation, USkeletalMeshComponent*& Mesh) const;

	/** Which movements override this one */
	UPROPERTY(BlueprintReadWrite)
		TArray<UTGOR_Movement*> OverriddenBy;
	
protected:

private:

};
