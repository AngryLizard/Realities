// The Gateway of Realities: Planes of Existence.

#pragma once

#include "CameraSystem/Content/TGOR_Camera.h"
#include "TGOR_ShoulderCamera.generated.h"

/**
 * 
 */
UCLASS()
class CAMERASYSTEM_API UTGOR_ShoulderCamera : public UTGOR_Camera
{
	GENERATED_BODY()
	
public:
	UTGOR_ShoulderCamera();

	virtual FVector4 Initial(const UTGOR_CameraComponent* Camera, const USpringArmComponent* SpringArm) const override;
	virtual void Apply(UTGOR_CameraComponent* Camera, USpringArmComponent* SpringArm, const FVector4& Input) override;

	/** Default camera shoulder offset */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "!TGOR Camera", meta = (Keywords = "C++"))
		float DefaultShoulder;
};
