// The Gateway of Realities: Planes of Existence.

#pragma once

#include "CameraSystem/Content/TGOR_Camera.h"
#include "TGOR_ZoomCamera.generated.h"

/**
 * 
 */
UCLASS()
class CAMERASYSTEM_API UTGOR_ZoomCamera : public UTGOR_Camera
{
	GENERATED_BODY()
	
public:
	UTGOR_ZoomCamera();

	virtual FVector4 Initial(const UTGOR_CameraComponent* Camera, const USpringArmComponent* SpringArm) const override;
	virtual void Apply(UTGOR_CameraComponent* Camera, USpringArmComponent* SpringArm, const FVector4& Input) override;

	/** Default camera zoom */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "!TGOR Camera", meta = (Keywords = "C++"))
		float DefaultZoom;
};
