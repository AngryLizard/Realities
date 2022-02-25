#pragma once

#include "CoreMinimal.h"
#include "Realities/CoreSystem/Interfaces/TGOR_SingletonInterface.h"
#include "Animation/AnimInstance.h"
#include "TGOR_SetupAnimInstance.generated.h"

class UTGOR_ControlComponent;

UCLASS(Transient, Blueprintable)
class REALITIES_API UTGOR_SetupAnimInstance : public UAnimInstance, public ITGOR_SingletonInterface
{
	GENERATED_UCLASS_BODY()

public:
	virtual void NativeInitializeAnimation() override;
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;

	UFUNCTION()
		void UpdateControls();

private:
	void UpdateControlsFromHierarchy(struct FRigControlHierarchy& Hierarchy);
};
