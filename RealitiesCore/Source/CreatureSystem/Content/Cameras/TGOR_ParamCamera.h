// The Gateway of Realities: Planes of Existence.

#pragma once

#include "CreatureSystem/Content/Cameras/TGOR_Camera.h"
#include "TGOR_ParamCamera.generated.h"

/**
 * 
 */
UCLASS()
class CREATURESYSTEM_API UTGOR_ParamCamera : public UTGOR_Camera
{
	GENERATED_BODY()
	
public:
	UTGOR_ParamCamera();

	virtual FVector4 Initial(const UTGOR_CameraComponent* Camera, const USpringArmComponent* SpringArm) const override;
	virtual void Apply(UTGOR_CameraComponent* Camera, USpringArmComponent* SpringArm, const FVector4& Input) override;

	/** Set parameter to given value */
	UFUNCTION(BlueprintImplementableEvent, Category = "!TGOR System", Meta = (Keywords = "C++"))
		void ApplyParameter(UTGOR_CameraComponent* Camera, const FVector4& Param);

	/** Default parameter */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "!TGOR Camera", meta = (Keywords = "C++"))
		FVector4 DefaultParam;
};
