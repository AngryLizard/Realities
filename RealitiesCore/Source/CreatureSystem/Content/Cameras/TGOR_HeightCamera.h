// The Gateway of Realities: Planes of Existence.

#pragma once

#include "CreatureSystem/Content/Cameras/TGOR_Camera.h"
#include "TGOR_HeightCamera.generated.h"

/**
 * 
 */
UCLASS()
class CREATURESYSTEM_API UTGOR_HeightCamera : public UTGOR_Camera
{
	GENERATED_BODY()
	
public:
	UTGOR_HeightCamera();

	virtual FVector4 Initial(const UTGOR_CameraComponent* Camera, const USpringArmComponent* SpringArm) const override;
	virtual void Apply(UTGOR_CameraComponent* Camera, USpringArmComponent* SpringArm, const FVector4& Input) override;

	/** Default camera height offset */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "!TGOR Camera", meta = (Keywords = "C++"))
		float DefaultHeight;
};
