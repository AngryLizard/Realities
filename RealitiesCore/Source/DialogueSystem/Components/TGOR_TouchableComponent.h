// The Gateway of Realities: Planes of Existence.

#pragma once

#include "CoreMinimal.h"

#include "TargetSystem/Components/TGOR_AimTargetComponent.h"
#include "TGOR_TouchableComponent.generated.h"

/**
 * UTGOR_TouchableComponent represent components that can be interacted with by hand cursor
 */
UCLASS(ClassGroup = (Custom))
class DIALOGUESYSTEM_API UTGOR_TouchableComponent : public UTGOR_AimTargetComponent
{
	GENERATED_BODY()

public:
	UTGOR_TouchableComponent();

	//////////////////////////////////////////// IMPLEMENTABLES ////////////////////////////////////////

	////////////////////////////////////////////////////////////////////////////////////////////////////
public:

	/** Animation instance to use while focused */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "!TGOR Interaction")
		TSubclassOf<class UAnimInstance> AnimInstance;

	/** Local rotation added to the Target Transform */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "!TGOR Interaction")
		FRotator OffsetRotation = FRotator::ZeroRotator;

	/** Size of the preview rectangle */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "!TGOR Interaction")
		float PreviewSize = 20.f;

	////////////////////////////////////////////////////////////////////////////////////////////////////
public:

	/** Attempts to project a ray onto this touchable and returns the projection as well as how far away from the touchable it is (0 for being inside its domain, 1 outside target radius) */
	UFUNCTION(BlueprintPure, Category = "!TGOR Interaction", Meta = (Keywords = "C++"))
		virtual bool GetTargetProjection(const FVector& RayOrigin, const FVector& RayDirection, FVector& Projection, float& Distance) const;

	/** Update target with current cursor state */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Interaction", Meta = (Keywords = "C++"))
		virtual void UpdateTarget(const FVector& RayOrigin, const FVector& RayDirection, float Intensity);

	/** Query location for where the current target/cursor state from UpdateTarget is */
	UFUNCTION(BlueprintPure, Category = "!TGOR Interaction", Meta = (Keywords = "C++"))
		virtual FTransform GetTargetTransform() const;

	////////////////////////////////////////////////////////////////////////////////////////////////////
public:

	/** Query intensity of current target/cursor on this touchable */
	UFUNCTION(BlueprintPure, Category = "!TGOR Interaction", Meta = (Keywords = "C++"))
		float GetTargetIntensity() const;

	////////////////////////////////////////////////////////////////////////////////////////////////////
protected:

	/** Last updated target intensity */
	UPROPERTY(BlueprintReadOnly, Category = "!TGOR Interaction")
		float CurrentIntensity;

	/** Project ray onto target space */
	UFUNCTION(BlueprintPure, Category = "!TGOR Interaction", Meta = (Keywords = "C++"))
		FVector ProjectRay(const FVector& RayOrigin, const FVector& RayDirection) const;

	////////////////////////////////////////////////////////////////////////////////////////////////////
public:

	//~ Begin UPrimitiveComponent Interface.
	virtual FPrimitiveSceneProxy* CreateSceneProxy() override;
	virtual bool AreSymmetricRotations(const FQuat& A, const FQuat& B, const FVector& Scale3D) const override;
	//~ End UPrimitiveComponent Interface.
	
	class FDrawTouchableSceneProxy : public UTGOR_AimTargetComponent::FDrawAimTargetSceneProxy
	{
	public:
		FDrawTouchableSceneProxy(const UTGOR_TouchableComponent* InComponent);

		virtual void DrawTargetSpace(const FMatrix& Transform, const FLinearColor& DrawColor, FPrimitiveDrawInterface* PDI) const override;
		void DrawRect(const FMatrix& Transform, FPrimitiveDrawInterface* PDI, const FLinearColor& ColorX, const FLinearColor& ColorY, const FVector& ScaledX, const FVector& ScaledY, const  float thickness, float border) const;

	protected:
		const float PreviewSize;
		const FVector LocalX;
		const FVector LocalY;
	};
};

