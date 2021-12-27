// The Gateway of Realities: Planes of Existence.

#pragma once

#include "CoreMinimal.h"

#include "AnimationSystem/Content/TGOR_Ellipsoid.h"

#include "CustomisationSystem/Interfaces/TGOR_ControlInterface.h"
#include "DimensionSystem/Interfaces/TGOR_SpawnerInterface.h"
#include "Components/ShapeComponent.h"
#include "TGOR_EllipsoidComponent.generated.h"

#define ELLIPSOID_RADIUS 10.0f


/**
 * UTGOR_EllipsoidComponent represent ellipsoidal collision elements
 */
UCLASS(ClassGroup = (Custom), Blueprintable, meta = (BlueprintSpawnableComponent))
class ANIMATIONSYSTEM_API UTGOR_EllipsoidComponent : public UShapeComponent, public ITGOR_SpawnerInterface, public ITGOR_ControlInterface
{
	GENERATED_BODY()

public:
	UTGOR_EllipsoidComponent();
	virtual FName GetControlName() const override;
	virtual FTransform GetControlTransform(USkinnedMeshComponent* Component) const override;

	virtual void UpdateContent_Implementation(FTGOR_SpawnerDependencies& Dependencies) override;
	virtual TMap<int32, UTGOR_SpawnModule*> GetModuleType_Implementation() const override;

	//////////////////////////////////////////// IMPLEMENTABLES ////////////////////////////////////////

	////////////////////////////////////////////////////////////////////////////////////////////////////
public:

	/** Ellipsoid this component represents */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "!TGOR Animation")
		TSubclassOf<UTGOR_Ellipsoid> SpawnEllipsoid;

	////////////////////////////////////////////////////////////////////////////////////////////////////
protected:

	/*  */
	UPROPERTY(BlueprintReadOnly, Category = "!TGOR Animation", Meta = (Keywords = "C++"))
		UTGOR_Ellipsoid* Ellipsoid;

	////////////////////////////////////////////////////////////////////////////////////////////////////
public:

	/** How much this collision applies */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "!TGOR Animation", meta = (ClampMin = 0, ClampMax = 1))
		float Intensity = 1.0f;

	/** Name of the controlrig key (if applicable, otherwise None) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "!TGOR Animation")
		FName ControlName;

	////////////////////////////////////////////////////////////////////////////////////////////////////
public:

	/** Set this ellipsoid to match a simple ground contact */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Animation", Meta = (Keywords = "C++"))
		void AdaptToGroundContact(const FVector& SurfaceLocation, const FVector& SurfaceNormal, float Stretch = 5.0f);

	////////////////////////////////////////////////////////////////////////////////////////////////////
public:

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
