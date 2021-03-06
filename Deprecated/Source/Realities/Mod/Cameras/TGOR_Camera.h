// The Gateway of Realities: Planes of Existence.

#pragma once

#include "Realities/Base/Content/TGOR_Content.h"
#include "TGOR_Camera.generated.h"

class UTGOR_CameraComponent;

/**
 * 
 */
UCLASS()
class REALITIES_API UTGOR_Camera : public UTGOR_Content
{
	GENERATED_BODY()
	
public:
	UTGOR_Camera();

	/** Get initial parameter state */
	UFUNCTION(BlueprintPure, Category = "TGOR Camera", Meta = (Keywords = "C++"))
		virtual FVector4 Initial(const UTGOR_CameraComponent* Camera, const USpringArmComponent* SpringArm) const;

	/** Filters an input signal with this modifier */
	UFUNCTION(BlueprintPure, Category = "TGOR Camera", Meta = (Keywords = "C++"))
		virtual FVector4 Filter(const UTGOR_CameraComponent* Camera, const USpringArmComponent* SpringArm, const FVector4& Params) const;

	/** Applies this camera setting to the camera */
	UFUNCTION(BlueprintCallable, Category = "TGOR Camera", Meta = (Keywords = "C++"))
		virtual void Apply(UTGOR_CameraComponent* Camera, USpringArmComponent* SpringArm, const FVector4& Input);


	/** Blendtime (in seconds) if no specific blendtime is specified */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "TGOR Camera", meta = (Keywords = "C++"))
		float DefaultBlendTime;
};
