// The Gateway of Realities: Planes of Existence.

#pragma once

#include "CoreMinimal.h"

#include "DimensionSystem/Components/TGOR_PilotComponent.h"
#include "TGOR_AttachComponent.generated.h"

/**
 * UTGOR_AttachComponent allows separate attachment points.
 */
UCLASS(ClassGroup = (Custom), Blueprintable, meta = (BlueprintSpawnableComponent))
class ANIMATIONSYSTEM_API UTGOR_AttachComponent : public UShapeComponent, public ITGOR_SpawnerInterface
{
	GENERATED_BODY()

public:
	UTGOR_AttachComponent();
	virtual void OnChildAttached(USceneComponent* ChildComponent) override;

	//////////////////////////////////////////// IMPLEMENTABLES ////////////////////////////////////////

	////////////////////////////////////////////////////////////////////////////////////////////////////
public:

	/** Sphere radius for attachment limits */
	UPROPERTY(EditAnywhere, Category = "!TGOR Movement")
		float LimitRadius;

	/** Radial offset along x axis */
	UPROPERTY(EditAnywhere, Category = "!TGOR Movement")
		float OffsetAngleX;

	/** Radial offset along y axis */
	UPROPERTY(EditAnywhere, Category = "!TGOR Movement")
		float OffsetAngleY;

	/** Sphere angle for attachment limits along X axis */
	UPROPERTY(EditAnywhere, Category = "!TGOR Movement")
		float LimitAngleX;

	/** Sphere angle for attachment limits along Y axis */
	UPROPERTY(EditAnywhere, Category = "!TGOR Movement")
		float LimitAngleY;
	
	////////////////////////////////////////////////////////////////////////////////////////////////////
public:

	// Checks whether a point in local space is inside the cone
	UFUNCTION(BlueprintPure, Category = "!TGOR Movement")
		bool IsInside(const FVector& Point) const;

	// Returns closest radial projection on the cone if not already inside
	UFUNCTION(BlueprintPure, Category = "!TGOR Movement")
		FVector ProjectInside(const FVector& Point) const;

	UFUNCTION(BlueprintCallable, Category = "!TGOR Movement")
		void SetLimits(float Radius, float AngleX, float AngleY);

	UFUNCTION(BlueprintCallable, Category = "!TGOR Movement")
		void SetOffsets(float OffX, float OffY);

	/** Get local rotational offset of alignment */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Movement")
		FQuat GetAlignmentRotation() const;

	/** Get local alignment direction relative to parent */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Movement")
		FVector GetAlignmentDirection() const;

	/** Get local alignment location relative to parent */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Movement")
		FVector GetAlignmentLocation() const;

	/** Draw collision vertices */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Movement", Meta = (Keywords = "C++"))
		void DebugDrawCollision(float Size, float Duration);

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
