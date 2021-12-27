// The Gateway of Realities: Planes of Existence.

#pragma once

#include "CoreMinimal.h"

#include "CustomisationSystem/Interfaces/TGOR_ControlInterface.h"
#include "AnimationSystem/Components/TGOR_AttachComponent.h"
#include "DimensionSystem/Components/TGOR_PilotComponent.h"
#include "TGOR_HandleComponent.generated.h"

class UTGOR_RigidComponent;

/**
 * UTGOR_HandleComponent represent animation targets
 */
UCLASS(ClassGroup = (Custom), Blueprintable, meta = (BlueprintSpawnableComponent))
class ANIMATIONSYSTEM_API UTGOR_HandleComponent : public UTGOR_PilotComponent, public ITGOR_ControlInterface
{
	GENERATED_BODY()

public:
	UTGOR_HandleComponent();
	virtual FName GetControlName() const override;
	virtual FTransform GetControlTransform(USkinnedMeshComponent* Component) const override;

	//////////////////////////////////////////// IMPLEMENTABLES ////////////////////////////////////////

	////////////////////////////////////////////////////////////////////////////////////////////////////
public:

	/** How much this control currently applies to the animation */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "!TGOR Animation", meta=(ClampMin = 0, ClampMax = 1))
		float Intensity = 1.0f;

	/** Name of the controlrig key (if applicable, otherwise None) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "!TGOR Animation")
		FName ControlName;

	/** Cone we're attached to if present */
	UPROPERTY(Transient, BlueprintReadOnly, Category = "!TGOR Animation")
		UTGOR_AttachComponent* MovementCone;

	////////////////////////////////////////////////////////////////////////////////////////////////////
public:

	/** Get simple ground contact from current state */
	UFUNCTION(BlueprintPure, Category = "!TGOR Animation", Meta = (Keywords = "C++"))
		void GetGroundContact(FVector& SurfaceLocation, FVector& SurfaceNormal) const;
};
