// The Gateway of Realities: Planes of Existence.

#pragma once

#include "CreatureSystem/Content/Cameras/TGOR_Camera.h"
#include "TGOR_LookatCamera.generated.h"

/**
 * 
 */
UCLASS()
class CREATURESYSTEM_API UTGOR_LookatCamera : public UTGOR_Camera
{
	GENERATED_BODY()
	
public:
	UTGOR_LookatCamera();

	virtual FVector4 Initial(const UTGOR_CameraComponent* Camera, const USpringArmComponent* SpringArm) const override;
	virtual FVector4 Filter(const UTGOR_CameraComponent* Camera, const USpringArmComponent* SpringArm, const FVector4& Params) const override;
	virtual void Apply(UTGOR_CameraComponent* Camera, USpringArmComponent* SpringArm, const FVector4& Input) override;
	

	/** Distance for which camera control starts to be disabled */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "!TGOR Camera", meta = (Keywords = "C++"))
		float MinControlDistance;

	/** Distance for which camera control is completely disabled */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "!TGOR Camera", meta = (Keywords = "C++"))
		float MaxControlDistance;
};
