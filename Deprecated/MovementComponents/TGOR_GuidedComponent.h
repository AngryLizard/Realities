// The Gateway of Realities: Planes of Existence.

#pragma once

#include "CoreMinimal.h"
#include "Realities/Utility/Error/TGOR_Error.h"

#include "Realities/Components/Movement/TGOR_MobilityComponent.h"
#include "TGOR_GuidedComponent.generated.h"


/**
* TGOR_GuidedComponent allows setting velocity and forces directly
*/
UCLASS(ClassGroup = (Custom), Blueprintable, meta = (BlueprintSpawnableComponent))
class REALITIES_API UTGOR_GuidedComponent : public UTGOR_MobilityComponent
{
	GENERATED_BODY()
	
public:
	UTGOR_GuidedComponent();
	virtual void BeginPlay() override;

	////////////////////////////////////////////////////////////////////////////////////////////////////
public:

	/** Override velocities and accelerations */
	UFUNCTION(BlueprintCallable, Category = "TGOR Movement", Meta = (Keywords = "C++"))
		void SetVelocity(const FVector& LinearVec, const FVector& AngularVec, const FVector& LinearAcc, const FVector& AngularAcc);

	/** Override applied velocities and simulate forces */
	UFUNCTION(BlueprintCallable, Category = "TGOR Movement", Meta = (Keywords = "C++"))
		void SimulateVelocity(const FVector& LinearVec, const FVector& AngularVec);

private:

	/** Last updated timestamp */
	UPROPERTY()
		FTGOR_Time Timestamp;
};
