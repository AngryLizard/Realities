// The Gateway of Realities: Planes of Existence.

#pragma once

#include "DimensionSystem/Content/TGOR_SpawnModule.h"
#include "TGOR_Camera.generated.h"

class UTGOR_CameraComponent;

/**
 * 
 */
UCLASS()
class CAMERASYSTEM_API UTGOR_Camera : public UTGOR_SpawnModule
{
	GENERATED_BODY()
	
public:
	UTGOR_Camera();

	/** Get initial parameter state */
	UFUNCTION(BlueprintPure, Category = "!TGOR Camera", Meta = (Keywords = "C++"))
		virtual FVector4 Initial(const UTGOR_CameraComponent* Camera, const USpringArmComponent* SpringArm) const;

	/** Filters an input signal with this modifier */
	UFUNCTION(BlueprintPure, Category = "!TGOR Camera", Meta = (Keywords = "C++"))
		virtual FVector4 Filter(const UTGOR_CameraComponent* Camera, const USpringArmComponent* SpringArm, const FVector& Params) const;

	/** Applies this camera setting to the camera */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Camera", Meta = (Keywords = "C++"))
		virtual void Apply(UTGOR_CameraComponent* Camera, USpringArmComponent* SpringArm, const FVector4& Input);
};
