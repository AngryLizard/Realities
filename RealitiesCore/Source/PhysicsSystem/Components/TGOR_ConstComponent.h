// The Gateway of Realities: Planes of Existence.

#pragma once

#include "CoreMinimal.h"

#include "RealitiesUtility/Structures/TGOR_AutoThreshold.h"

#include "CoreSystem/Components/TGOR_Component.h"
#include "TGOR_ConstComponent.generated.h"


/**
* TGOR_GuidedComponent allows setting velocity and forces directly
*/
UCLASS(ClassGroup = (Custom), Blueprintable, meta = (BlueprintSpawnableComponent))
class PHYSICSSYSTEM_API UTGOR_ConstComponent : public UTGOR_Component
{
	GENERATED_BODY()
	
public:
	UTGOR_ConstComponent();
	float Simulate(float Time);

	////////////////////////////////////////////////////////////////////////////////////////////////////
public:

	/** Synchronizes rotation with global (and replicated) GameTimestamp according to a constant angular velocity */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "!TGOR Movement")
		bool SynchronisesRotationWithTimestamp;
	
	/** Error threshold for which rotation gets synched */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "!TGOR Movement")
		FTGOR_AutoThreshold AngularThreshold;

	/**
	UFUNCTION(BlueprintCallable, Category = "!TGOR Movement", Meta = (Keywords = "C++"))
		void SetVelocity(const FVector& LinearVec, const FVector& AngularVec);

	UFUNCTION(BlueprintCallable, Category = "!TGOR Movement", Meta = (Keywords = "C++"))
		void SetAcceleration(const FVector& LinearAcc, const FVector& AngularAcc);
		*/

private:
};
