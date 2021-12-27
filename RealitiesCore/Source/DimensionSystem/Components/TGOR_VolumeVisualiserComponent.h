// The Gateway of Realities: Planes of Existence.

#pragma once

#include "CoreSystem/Components/TGOR_Component.h"
#include "TGOR_VolumeVisualiserComponent.generated.h"

class UTGOR_DimensionData;
class ATGOR_PhysicsVolume;

/**
*	UTGOR_VolumeVisualiserComponent visualises volume owning actor
*/
UCLASS( ClassGroup=(Custom), Within=TGOR_PhysicsVolume )
class DIMENSIONSYSTEM_API UTGOR_VolumeVisualiserComponent : public UShapeComponent
{
	GENERATED_BODY()

	////////////////////////////////////////////////////////////////////////////////////////////////////
public:
	UTGOR_VolumeVisualiserComponent();

	/** Distance between samples */
	UPROPERTY(EditAnywhere, Category = "!TGOR Movement")
		float SampleDistance;

	/** Sample max length */
	UPROPERTY(EditAnywhere, Category = "!TGOR Movement")
		float SampleLength;

	////////////////////////////////////////////////////////////////////////////////////////////////////
public:

	//~ Begin UObject Interface
#if WITH_EDITOR
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
	void UpdateVisuals();
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
