// The Gateway of Realities: Planes of Existence.

#pragma once

#include "Kismet/KismetSystemLibrary.h"

#include "../TGOR_AimInstance.h"

#include "CoreSystem/Interfaces/TGOR_RegisterInterface.h"
#include "DimensionSystem/Interfaces/TGOR_DimensionInterface.h"
#include "TGOR_AimTargetComponent.h"
#include "TGOR_AimComponent.generated.h"

class UTGOR_InteractableComponent;

////////////////////////////////////////////// STRUCT //////////////////////////////////////////////////////

USTRUCT(BlueprintType)
struct FTGOR_AimSuspensionHandle
{
	GENERATED_USTRUCT_BODY()

	/** Register to poll for activity */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "!TGOR Aim")
		TScriptInterface<ITGOR_RegisterInterface> Register;

	/** Whether aim should be suspended altogether. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "!TGOR Aim")
		bool Suspend = false;

	/** Filter only for certain targets. Completely suspend if none. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "!TGOR Aim")
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
		float Weight = 1.0f;

	/** Direction of the point to origin */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FVector Direction = FVector::ZeroVector;
};


/**
* TGOR_AimComponent adds aiming functionality to an actor
*/
UCLASS(ClassGroup = (Custom), Blueprintable, meta = (BlueprintSpawnableComponent))
class TARGETSYSTEM_API UTGOR_AimComponent : public UTGOR_AimTargetComponent, public ITGOR_DimensionInterface
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

	/** Query scene for targets near a location, returns whether any were found */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Aim", Meta = (Keywords = "C++"))
		bool UpdateCandidatesNearby();

	/** Query current candidates for targets from camera */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Aim", Meta = (Keywords = "C++"))
		void UpdateAimFromCamera(const FVector& Location, const FVector& Direction);

	/** Compute current aim location in world space */
	UFUNCTION(BlueprintPure, Category = "!TGOR Aim", Meta = (Keywords = "C++"))
		FVector ComputeCurrentAimLocation(bool Sticky) const;

	/** Compute given aim location in world space */
	UFUNCTION(BlueprintPure, Category = "!TGOR Aim", Meta = (Keywords = "C++"))
		static FVector ComputeAimLocation(const FTGOR_AimInstance& Instance, bool Sticky);

	/** Register aim suspension to go active. */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Aim", Meta = (Keywords = "C++"))
		void RegisterHandle(TScriptInterface<ITGOR_RegisterInterface> Register, UTGOR_CoreContent* Content);

	/** Add filter to aim for. */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Aim", Meta = (Keywords = "C++"))
		void RegisterFilter(TScriptInterface<ITGOR_RegisterInterface> Register, UTGOR_CoreContent* Content, TSubclassOf<UTGOR_Target> Filter);

	/** Unregister manually. */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Aim", Meta = (Keywords = "C++"))
		void UnregisterHandle(UTGOR_CoreContent* Content);

	////////////////////////////////////////////////////////////////////////////////////////////////////
protected:

	/** Handles for aim suspension */
	UPROPERTY(BlueprintReadOnly, Category = "!TGOR Bodypart")
		TMap<UTGOR_CoreContent*, FTGOR_AimSuspensionHandle> Handles;

	/** Get whether we are suspending aim */
	UFUNCTION(BlueprintPure, Category = "!TGOR Aim", Meta = (Keywords = "C++"))
		bool IsSuspended() const;

	/** Get final filter for target classes */
	UFUNCTION(BlueprintPure, Category = "!TGOR Aim|Internal", Meta = (Keywords = "C++"))
		TSubclassOf<UTGOR_Target> GetAimFilter() const;

	////////////////////////////////////////////////////////////////////////////////////////////////////
protected:

	/** Current interaction candidates */
	UPROPERTY(BlueprintReadOnly, Category = "!TGOR Aim", Meta = (Keywords = "C++"))
		TArray<UTGOR_InteractableComponent*> Candidates;

	/** Current aim points */
	UPROPERTY(BlueprintReadOnly, Category = "!TGOR Aim", Meta = (Keywords = "C++"))
		TArray<FTGOR_WeightedPoint> Points;

	/** Replicate aimtuple */
	UFUNCTION()
		void OnRepNotifyAimInstance();

	/** Current Aim */
	UPROPERTY(ReplicatedUsing = OnRepNotifyAimInstance)
		FTGOR_AimInstance AimInstance;

	/** Angle threshold in radians for when points are discarded */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "!TGOR Aim", Meta = (Keywords = "C++"))
		float AngleThreshold;

	/** Weight of own component for targets */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "!TGOR Aim", Meta = (Keywords = "C++"))
		float SelfAimWeight;

	/** Maximum aim distance */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "!TGOR Aim", Meta = (Keywords = "C++"))
		float MaxAimDistance;

	/** Distance in front of character to ignore when aiming from behind */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "!TGOR Aim", Meta = (Keywords = "C++"))
		float ForwardAimDistance;

	/** Set AimDistance to best weighted target even if they are out of range */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "!TGOR Aim", Meta = (Keywords = "C++"))
		bool ConsiderOutOfRange;

	/** Camera trace type */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "!TGOR Aim", Meta = (Keywords = "C++"))
		TEnumAsByte<ECollisionChannel> CameraTrace;

	////////////////////////////////////////////////////////////////////////////////////////////////////
public:

	/** Get current aim instance */
	UFUNCTION(BlueprintPure, Category = "!TGOR Aim", Meta = (Keywords = "C++"))
		const FTGOR_AimInstance& GetCurrentAim() const;

	/** Find whether we are currently aiming at a given type of target */
	UFUNCTION(BlueprintPure, Category = "!TGOR Aim", Meta = (Keywords = "C++"))
		bool HasTarget(TSubclassOf<UTGOR_Target> Type) const;

	/** Find whether we are currently aiming at a given actor */
	UFUNCTION(BlueprintPure, Category = "!TGOR Aim", Meta = (Keywords = "C++"))
		bool IsAimingAt(AActor* Actor) const;

	/** Find whether we are currently aiming at the owner of this component */
	UFUNCTION(BlueprintPure, Category = "!TGOR Aim", Meta = (Keywords = "C++"))
		bool IsAimingAtSelf() const;
};
