// The Gateway of Realities: Planes of Existence.

#pragma once

#include "TGOR_Camera.h"
#include "TGOR_PivotCamera.generated.h"

/**
 * 
 */
UCLASS()
class REALITIES_API UTGOR_PivotCamera : public UTGOR_Camera
{
	GENERATED_BODY()
	
public:
	UTGOR_PivotCamera();

	virtual FVector4 Initial(const UTGOR_CameraComponent* Camera, const USpringArmComponent* SpringArm) const override;
	virtual void Apply(UTGOR_CameraComponent* Camera, USpringArmComponent* SpringArm, const FVector4& Input) override;

};
