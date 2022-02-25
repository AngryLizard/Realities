// The Gateway of Realities: Planes of Existence.

#pragma once

#include <functional>
#include "Kismet/KismetSystemLibrary.h"
#include "Realities/Base/Instances/Combat/TGOR_AimInstance.h"

#include "Realities/Interfaces/TGOR_RegisterInterface.h"
#include "Realities/Interfaces/TGOR_DimensionInterface.h"
#include "Realities/Components/Combat/TGOR_AimTargetComponent.h"
#include "TGOR_AimComponent.generated.h"

class UTGOR_InteractableComponent;

////////////////////////////////////////////// STRUCT //////////////////////////////////////////////////////

USTRUCT(BlueprintType)
struct FTGOR_AimSuspensionHandle
{
	GENERATED_USTRUCT_BODY()

	/** Register to poll for activity */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "TGOR Aim")
		TScriptInterface<ITGOR_RegisterInterface> Register;

	/** Whether aim should be suspended altogether. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "TGOR Aim")
		bool Suspend;

	/** Filter only for certain targets. Completely suspend if none. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "TGOR Aim")
		TSubclassOf<UTGOR_Target> Filter;
};

/**
*
*/
USTRUCT(BlueprintType)
struct FTGOR_WeightedPoint
{
	GENERATED_USTRUCT_BODY()

	FTGOR_WeightedPoint();

	/** Point to be weighted */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FTGOR_AimPoint Point;

	/** Weight of the point */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float Weight;
};


/**
* TGOR_AimComponent adds aiming functionality to an actor
*/
UCLASS(ClassGroup = (Custom), Blueprintable, meta = (BlueprintSpawnableComponent))
class REALITIES_API UTGOR_AimComponent : public UTGOR_AimTargetComponent, public ITGOR_DimensionInterface
{
	GENERATED_BODY()

public:

	UTGOR_AimComponent();
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	////////////////////////////////////////////////////////////////////////////////////////////////////

	/** Sets focus */
	UFUNCTION(Unreliable, Server, WithValidation)
		void ReplicateFocus(FTGOR_AimInstance Instance);
		void ReplicateFocus_Implementation(FTGOR_AimInstance Instance);
		bool ReplicateFocus_Validate(FTGOR_AimInstance Instance);

	/** Query scene for targets from camera */
	UFUNCTION(BlueprintCallable, Category = "TGOR Aim", Meta = (Keywords = "C++"))
		void UpdateAimFromCameraNearby(const FVector& Location, const FVector& Direction);

	/** Query current candidates for targets from camera */
	UFUNCTION(BlueprintCallable, Category = "TGOR Aim", Meta = (Keywords = "C++"))
		void UpdateAimFromCamera(const FVector& Location, const FVector& Direction);

	/** Compute current aim location in world space */
	UFUNCTION(BlueprintPure, Category = "TGOR Aim", Meta = (Keywords = "C++"))
		FVector ComputeCurrentAimLocation(bool Sticky) const;

	/** Compute given aim location in world space */
	UFUNCTION(BlueprintPure, Category = "TGOR Aim", Meta = (Keywords = "C++"))
		static FVector ComputeAimLocation(const FTGOR_AimInstance& Instance, bool Sticky);

	/** Register aim suspension to go active. */
	UFUNCTION(BlueprintCallable, Category = "TGOR Aim", Meta = (Keywords = "C++"))
		void RegisterHandle(TScriptInterface<ITGOR_RegisterInterface> Register, UTGOR_Content* Content);

	/** Add filter to aim for. */
	UFUNCTION(BlueprintCallable, Category = "TGOR Aim", Meta = (Keywords = "C++"))
		void RegisterFilter(TScriptInterface<ITGOR_RegisterInterface> Register, UTGOR_Content* Content, TSubclassOf<UTGOR_Target> Filter);

	/** Unregister manually. */
	UFUNCTION(BlueprintCallable, Category = "TGOR Aim", Meta = (Keywords = "C++"))
		void UnregisterHandle(UTGOR_Content* Content);

	////////////////////////////////////////////////////////////////////////////////////////////////////
protected:

	/** Handles for aim suspension */
	UPROPERTY(BlueprintReadOnly, Category = "TGOR Bodypart")
		TMap<UTGOR_Content*, FTGOR_AimSuspensionHandle> Handles;

	/** Get whether we are suspending aim */
	UFUNCTION(BlueprintPure, Category = "TGOR Aim", Meta = (Keywords = "C++"))
		bool IsSuspended() const;

	/** Get final filter for target classes */
	UFUNCTION(BlueprintPure, Category = "TGOR Aim", Meta = (Keywords = "C++"))
		TSubclassOf<UTGOR_Target> GetAimFilter() const;

	////////////////////////////////////////////////////////////////////////////////////////////////////
protected:

	/** Current interaction candidates */
	UPROPERTY(BlueprintReadOnly, Category = "TGOR Aim", Meta = (Keywords = "C++"))
		TArray<UTGOR_InteractableComponent*> Candidates;

	/** Current aim points */
	UPROPERTY(BlueprintReadOnly, Category = "TGOR Aim", Meta = (Keywords = "C++"))
		TArray<FTGOR_WeightedPoint> Points;

	/** Replicate aimtuple */
	UFUNCTION()
		void OnRepNotifyAimInstance();

	/** Current Aim */
	UPROPERTY(ReplicatedUsing = OnRepNotifyAimInstance)
		FTGOR_AimInstance AimInstance;

	/** Weight of own component for targets */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TGOR Aim", Meta = (Keywords = "C++"))
		float SelfAimWeight;

	/** Maximum aim distance */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TGOR Aim", Meta = (Keywords = "C++"))
		float MaxAimDistance;

	/** Set AimDistance to best weighted target even if they are out of range */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TGOR Aim", Meta = (Keywords = "C++"))
		bool ConsiderOutOfRange;

	/** Camera trace type */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TGOR Aim", Meta = (Keywords = "C++"))
		TEnumAsByte<ECollisionChannel> CameraTrace;

	////////////////////////////////////////////////////////////////////////////////////////////////////
public:

	/** Get current aim instance */
	UFUNCTION(BlueprintPure, Category = "TGOR Aim", Meta = (Keywords = "C++"))
		const FTGOR_AimInstance& GetCurrentAim() const;

	/** Find whether we are currently aiming at a given type of target */
	UFUNCTION(BlueprintPure, Category = "TGOR Aim", Meta = (Keywords = "C++"))
		bool HasTarget(TSubclassOf<UTGOR_Target> Type) const;

	/** Find whether we are currently aiming at a given actor */
	UFUNCTION(BlueprintPure, Category = "TGOR Aim", Meta = (Keywords = "C++"))
		bool IsAimingAt(AActor* Actor) const;

	/** Find whether we are currently aiming at the owner of this component */
	UFUNCTION(BlueprintPure, Category = "TGOR Aim", Meta = (Keywords = "C++"))
		bool IsAimingAtSelf() const;
};
