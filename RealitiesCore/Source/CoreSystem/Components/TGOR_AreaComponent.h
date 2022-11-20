// The Gateway of Realities: Planes of Existence.

#pragma once

#include "CoreMinimal.h"

#include "Components/ShapeComponent.h"
#include "TGOR_AreaComponent.generated.h"

/**
 * UTGOR_AreaComponent Represents a flat area
 */
UCLASS(ClassGroup = (Custom), Blueprintable, meta = (BlueprintSpawnableComponent))
class CORESYSTEM_API UTGOR_AreaComponent : public UShapeComponent
{
	GENERATED_BODY()

public:
	UTGOR_AreaComponent();

	//////////////////////////////////////////// IMPLEMENTABLES ////////////////////////////////////////

	////////////////////////////////////////////////////////////////////////////////////////////////////
public:

	/** Radius where participant can be situated */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "!TGOR Core")
		float Radius = 300.0f;

	/** Gets whether a given location is inside this area */
	UFUNCTION(BlueprintPure, Category = "!TGOR Core", Meta = (Keywords = "C++"))
		bool IsLocationInside(const FVector& Location) const;

	////////////////////////////////////////////////////////////////////////////////////////////////////
public:

	//~ Begin UObject Interface
#if WITH_EDITOR
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
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

