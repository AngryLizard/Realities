// The Gateway of Realities: Planes of Existence.

#pragma once

#include "CoreMinimal.h"

#include "Realities/Components/Movement/TGOR_SocketComponent.h"
#include "TGOR_PilotComponent.generated.h"


DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FMovementTeleportedDelegate, const FTGOR_MovementDynamic&, From, const FTGOR_MovementDynamic&, To);

/**
 * TGOR_PilotComponent Moves owner's root component. Actors should only have one of these.
 */
UCLASS(ClassGroup = (Custom), Blueprintable, meta = (BlueprintSpawnableComponent))
class REALITIES_API UTGOR_PilotComponent : public UTGOR_SocketComponent
{
	GENERATED_BODY()
	
public:
	UTGOR_PilotComponent();

	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	virtual void MovementAdjust(const FTGOR_MovementBase& Old, const FTGOR_MovementBase& New) override;
	virtual USceneComponent* GetMovementBasis() const override;

	////////////////////////////////////////////////////////////////////////////////////////////////////
public:

	/** Get Base Component */
	UFUNCTION(BlueprintPure, Category = "TGOR Movement", Meta = (Keywords = "C++"))
		USceneComponent* GetBaseComponent() const;

	/** Set Base Component */
	UFUNCTION(BlueprintCallable, Category = "TGOR Movement", Meta = (Keywords = "C++"))
		virtual void SetBaseComponent(USceneComponent* Component);

	////////////////////////////////////////////////////////////////////////////////////////////////////
protected:

	/** Threshold when comparing netupdates. Only smooths updates when threshold is met. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "TGOR Network")
		FTGOR_PositionThreshold NetDeltaThreshold;

	/** Delta for movement corrections so visuals can update smoothly */
	UPROPERTY(BlueprintReadOnly, Category = "TGOR Movement")
		FTGOR_MovementPosition NetSmoothDelta;


	/** Sets NetSmoothDelta to a given delta transform (mostly used for testing) */
	UFUNCTION(BlueprintCallable, Category = "TGOR Movement", Meta = (Keywords = "C++"))
		void SetNetSmooth(FVector Delta, FRotator Rotation);

	/** NetSmoothDelta is the visual movement offset introduced by replication to be used for smooth visual updates */
	UFUNCTION(BlueprintCallable, Category = "TGOR Movement", Meta = (Keywords = "C++"))
		void NetSmooth(float Timestep, float Speed);

	/** Transforms a component to the current smoothdelta depending on the initial transform (transform when there is no delta) */
	UFUNCTION(BlueprintCallable, Category = "TGOR Movement", Meta = (Keywords = "C++"))
		void SetComponentFromSmoothDelta(USceneComponent* Component, const FTransform& InitialWorld);

private:

	/** Component that is piloted by this movement */
	UPROPERTY()
		TWeakObjectPtr<USceneComponent> BaseComponent;

	////////////////////////////////////////////////////////////////////////////////////////////////////
public:

	UPROPERTY(BlueprintAssignable, Category = "TGOR Movement")
		FMovementTeleportedDelegate OnTeleportedMovement;

	/** Teleports this component to a position */
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "TGOR Movement", Meta = (Keywords = "C++"))
		virtual bool Teleport(const FTGOR_MovementDynamic& Dynamic);

};
