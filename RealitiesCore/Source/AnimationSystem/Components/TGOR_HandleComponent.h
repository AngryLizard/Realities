// The Gateway of Realities: Planes of Existence.

#pragma once

#include "CoreMinimal.h"

#include "CustomisationSystem/Interfaces/TGOR_ControlInterface.h"
#include "DimensionSystem/Components/TGOR_PilotComponent.h"
#include "TGOR_HandleComponent.generated.h"

class UTGOR_AttachComponent;
class UTGOR_RigidComponent;


/**
*
*/
USTRUCT(BlueprintType)
struct ANIMATIONSYSTEM_API FTGOR_HandleTraceOutput : public FTGOR_MovementDynamic
{
	GENERATED_USTRUCT_BODY();

	/** Handle to cone delta */
	UPROPERTY()
		FVector Delta = FVector::ZeroVector;

	/** Trace direction */
	UPROPERTY()
		FVector Direction = FVector::ZeroVector;
};

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
	virtual FTransform GetControlTransform(UTGOR_ControlSkeletalMeshComponent* Component) const override;

	//////////////////////////////////////////// IMPLEMENTABLES ////////////////////////////////////////

	////////////////////////////////////////////////////////////////////////////////////////////////////
public:

	/** Flatness of this control */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "!TGOR Animation")
		float Flatness;

	/** Name of the controlrig key (if applicable, otherwise None) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "!TGOR Animation")
		FName ControlName;

	/** Cone we're attached to if present */
	UPROPERTY(Transient, BlueprintReadOnly, Category = "!TGOR Animation")
		UTGOR_AttachComponent* MovementCone;

	////////////////////////////////////////////////////////////////////////////////////////////////////
public:

	/** Traces for new handle position, relocates pilot accordingly (assumes owning cone is directly parented to root).
		Returns true if trace was successful and hit something. */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Movement")
		bool TraceHandle(UTGOR_PilotComponent* Component, const FTGOR_MovementSpace& Space, float Multiplier, FTGOR_HandleTraceOutput& Output);



	////////////////////////////////////////////////////////////////////////////////////////////////////
public:

	//~ Begin UPrimitiveComponent Interface.
	virtual FPrimitiveSceneProxy* CreateSceneProxy() override;
	//~ End UPrimitiveComponent Interface.

};
