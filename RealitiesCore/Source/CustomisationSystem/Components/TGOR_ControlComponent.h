// The Gateway of Realities: Planes of Existence.

#pragma once

#include "CoreMinimal.h"

#include "CustomisationSystem/Interfaces/TGOR_ControlInterface.h"
#include "Components/ShapeComponent.h"
#include "TGOR_ControlComponent.generated.h"

#define CONTROL_RADIUS 10.0f


UENUM(BlueprintType)
enum class ETGOR_ControlType : uint8
{
	Pivot,
	Vector
};

/**
 * UTGOR_ControlComponent represent character customisations
 */
UCLASS(ClassGroup = (Custom), Blueprintable, meta = (BlueprintSpawnableComponent))
class CUSTOMISATIONSYSTEM_API UTGOR_ControlComponent : public UShapeComponent, public ITGOR_ControlInterface
{
	GENERATED_BODY()

public:
	UTGOR_ControlComponent();
	virtual FName GetControlName() const override;
	virtual FTransform GetControlTransform(USkinnedMeshComponent* Component) const override;

	//////////////////////////////////////////// IMPLEMENTABLES ////////////////////////////////////////

	////////////////////////////////////////////////////////////////////////////////////////////////////
public:

	/** Type of the control */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "!TGOR Customisation")
		ETGOR_ControlType ControlType;
	
	////////////////////////////////////////////////////////////////////////////////////////////////////
public:

	//~ Begin UObject Interface
#if WITH_EDITOR
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
	virtual void PostEditChangeChainProperty(FPropertyChangedChainEvent& PropertyChangedEvent) override;
	virtual void PostEditComponentMove(bool bFinished) override;
#endif // WITH_EDITOR
	//~ End UObject Interface

	//~ Begin USceneComponent Interface
	virtual FBoxSphereBounds CalcBounds(const FTransform& LocalToWorld) const override;
	//~ End USceneComponent Interface

	//~ Begin UPrimitiveComponent Interface.
	virtual FPrimitiveSceneProxy* CreateSceneProxy() override;
	virtual bool IsZeroExtent() const override;
	virtual bool AreSymmetricRotations(const FQuat& A, const FQuat& B, const FVector& Scale3D) const override;
	//~ End UPrimitiveComponent Interface.

	//~ Begin UShapeComponent Interface
	virtual void UpdateBodySetup() override;
	//~ End UShapeComponent Interface
};
