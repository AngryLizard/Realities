// The Gateway of Realities: Planes of Existence.

#pragma once

#include "CreatureSystem/Content/TGOR_Camera.h"
#include "TGOR_DollyCamera.generated.h"

/**
 * 
 */
UCLASS()
class CAMERA_API UTGOR_DollyCamera : public UTGOR_Camera
{
	GENERATED_BODY()
	
public:
	UTGOR_DollyCamera();

	virtual FVector4 Initial(const UTGOR_CameraComponent* Camera, const USpringArmComponent* SpringArm) const override;
	virtual FVector4 Filter(const UTGOR_CameraComponent* Camera, const USpringArmComponent* SpringArm, const FVector4& Params) const override;
	virtual void Apply(UTGOR_CameraComponent* Camera, USpringArmComponent* SpringArm, const FVector4& Input) override;


	/** Default camera FOV */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "!TGOR Camera", meta = (Keywords = "C++"))
		float DefaultFOV;

	/** Max dolly strength in [0,1] */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "!TGOR Camera", meta = (Keywords = "C++"))
		float DollyStrength;

	/** Default vignette strength */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "!TGOR Camera", meta = (Keywords = "C++"))
		float DefaultVignette;

	/** Max vignette strength */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "!TGOR Camera", meta = (Keywords = "C++"))
		float VignetteStrength;

};
