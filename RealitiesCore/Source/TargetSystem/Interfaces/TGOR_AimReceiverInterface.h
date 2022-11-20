
// The Gateway of Realities: Planes of Existence.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "TargetSystem/TGOR_AimInstance.h"

#include "TGOR_AimReceiverInterface.generated.h"

UINTERFACE(MinimalAPI)
class UTGOR_AimReceiverInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 *
 */
class TARGETSYSTEM_API ITGOR_AimReceiverInterface
{
	GENERATED_BODY()

public:

	/** Test whether a given candidate is valid for aim target */
	UFUNCTION(BlueprintNativeEvent, Category = "!TGOR Game", Meta = (Keywords = "C++"))
		bool TestAimCandidate(UTGOR_AimTargetComponent* Candidate);
	virtual bool TestAimCandidate_Implementation(UTGOR_AimTargetComponent* Candidate);

	/** Test whether a given aim target is valid */
	UFUNCTION(BlueprintNativeEvent, Category = "!TGOR Game", Meta = (Keywords = "C++"))
		bool TestAimTarget(const FTGOR_AimInstance& Aim);
	virtual bool TestAimTarget_Implementation(const FTGOR_AimInstance& Aim);

	/** Apply an aim target. By the time of execution it is guaranteed to be valid */
	UFUNCTION(BlueprintNativeEvent, Category = "!TGOR Game", Meta = (Keywords = "C++"))
		void ApplyAimTarget(const FTGOR_AimInstance& Aim);
	virtual void ApplyAimTarget_Implementation(const FTGOR_AimInstance& Aim);

};
