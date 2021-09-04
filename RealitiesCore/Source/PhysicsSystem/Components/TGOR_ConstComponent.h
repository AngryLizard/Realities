// The Gateway of Realities: Planes of Existence.

#pragma once

#include "CoreMinimal.h"

#include "RealitiesUtility/Structures/TGOR_AutoThreshold.h"

#include "DimensionSystem/Components/TGOR_PilotComponent.h"
#include "TGOR_ConstComponent.generated.h"


/**
* TGOR_GuidedComponent allows setting velocity and forces directly
*/
UCLASS(ClassGroup = (Custom), Blueprintable, meta = (BlueprintSpawnableComponent))
class PHYSICSSYSTEM_API UTGOR_ConstComponent : public UTGOR_PilotComponent
{
	GENERATED_BODY()
	
public:
	UTGOR_ConstComponent();
	virtual float Simulate(float Time) override;

	////////////////////////////////////////////////////////////////////////////////////////////////////
public:

	/** Synchronizes rotation with global (and replicated) GameTimestamp according to a constant angular velocity */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "!TGOR Movement")
		bool SynchronisesRotationWithTimestamp;
	
	/** Error threshold for which rotation gets synched */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "!TGOR Movement")
		FTGOR_AutoThreshold AngularThreshold;

	/** Override velocities and resets accelerations */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Movement", Meta = (Keywords = "C++"))
		void SetVelocity(const FVector& LinearVec, const FVector& AngularVec);

	/** Override accelerations */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Movement", Meta = (Keywords = "C++"))
		void SetAcceleration(const FVector& LinearAcc, const FVector& AngularAcc);

private:
};
