// The Gateway of Realities: Planes of Existence.

#pragma once

#include "Kismet/KismetSystemLibrary.h"

#include "../TGOR_AimInstance.h"
#include "TargetSystem/Interfaces/TGOR_AimReceiverInterface.h"

#include "CoreSystem/Interfaces/TGOR_RegisterInterface.h"
#include "DimensionSystem/Interfaces/TGOR_DimensionInterface.h"
#include "Components/ArrowComponent.h"
#include "TGOR_AimComponent.generated.h"

class UTGOR_AimTargetComponent;

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
* TGOR_AimComponent adds aiming functionality to an actor
*/
UCLASS(ClassGroup = (Custom), Blueprintable, meta = (BlueprintSpawnableComponent))
class TARGETSYSTEM_API UTGOR_AimComponent : public UArrowComponent, public ITGOR_DimensionInterface
{
	GENERATED_BODY()

public:

	UTGOR_AimComponent();
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	////////////////////////////////////////////////////////////////////////////////////////////////////

	/** Query scene for targets near a location, returns whether any were found */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Aim", Meta = (Keywords = "C++", AdvancedDisplay = "Radius, bIgnoreSelf"))
		bool UpdateCandidatesNearby(TScriptInterface<ITGOR_AimReceiverInterface> Receiver, const FVector& Center, TEnumAsByte<ECollisionChannel> TraceChannel, float Radius = 10000.0f, bool bIgnoreSelf = true);

	/** Query current candidates for targets from camera */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Aim", Meta = (Keywords = "C++", AdvancedDisplay = "AngleThreshold, SelfAimWeight, bIgnoreFront, frontAimDistance"))
		bool UpdatePointsFromCamera(TScriptInterface<ITGOR_AimReceiverInterface> Receiver, const FVector& Location, const FVector& Direction, TSubclassOf<UTGOR_Target> Filter = nullptr, float AngleThreshold = 0.05f, float SelfAimWeight = 1.0f, bool bIgnoreFront = true, float frontAimDistance = 0.0f);

	/** Query current candidates for targets from camera */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Aim", Meta = (Keywords = "C++", AdvancedDisplay = "bConsiderOutOfRange"))
		bool UpdateAimFromPoints(TScriptInterface<ITGOR_AimReceiverInterface> Receiver, bool bConsiderOutOfRange = false);

	/** Removes all candidates and aim points. */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Aim", Meta = (Keywords = "C++"))
		void ResetAim();

	/** Compute given aim location in world space */
	UFUNCTION(BlueprintPure, Category = "!TGOR Aim", Meta = (Keywords = "C++"))
		static FVector ComputeAimLocation(const FTGOR_AimInstance& Instance, bool Sticky);

	/** Get nearby targets according to distance to aim-component */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Aim", Meta = (Keywords = "C++"))
		TArray<UTGOR_AimTargetComponent*> GetNearbyCandidates(const FVector& Center, TSubclassOf<UTGOR_Target> Filter, float MaxDistance) const;

	////////////////////////////////////////////////////////////////////////////////////////////////////
protected:

	/** Current interaction candidates */
	UPROPERTY(BlueprintReadOnly, Category = "!TGOR Aim", Meta = (Keywords = "C++"))
		TArray<UTGOR_AimTargetComponent*> Candidates;

	/** Current aim points */
	UPROPERTY(BlueprintReadOnly, Category = "!TGOR Aim", Meta = (Keywords = "C++"))
		TArray<FTGOR_WeightedAimPoint> Points;

	////////////////////////////////////////////////////////////////////////////////////////////////////
public:

	/** Distance in front of character to ignore when aiming from behind */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "!TGOR Aim", Meta = (Keywords = "C++", EditCondition = "bIgnoreBehindAim"))
		float ForwardAimDistance = 250.0f;
};
