// The Gateway of Realities: Planes of Existence.

#pragma once

#include "CoreMinimal.h"

#include "AnimationSystem/Content/TGOR_AttachCone.h"

#include "DimensionSystem/Interfaces/TGOR_SpawnerInterface.h"
#include "DimensionSystem/Components/TGOR_PilotComponent.h"
#include "TGOR_AttachComponent.generated.h"

/**
*
*/
USTRUCT(BlueprintType)
struct ANIMATIONSYSTEM_API FTGOR_ConeTraceOutput
{
	GENERATED_USTRUCT_BODY();

	/** Trace location */
	UPROPERTY()
		FVector Location = FVector::ZeroVector;

	/** Normal location */
	UPROPERTY()
		FVector Normal = FVector::UpVector;

	/** Handle to cone delta */
	UPROPERTY()
		FVector Delta = FVector::ZeroVector;

	/** Trace direction */
	UPROPERTY()
		FVector Direction = FVector::ZeroVector;

	/** Trace parent */
	UPROPERTY()
		FTGOR_MovementParent Parent;

	FTGOR_MovementDynamic GetDynamicFromTrace(const FTGOR_MovementSpace& Space) const;
};

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

	virtual void UpdateContent_Implementation(FTGOR_SpawnerDependencies& Dependencies) override;
	virtual TMap<int32, UTGOR_SpawnModule*> GetModuleType_Implementation() const override;

	//////////////////////////////////////////// IMPLEMENTABLES ////////////////////////////////////////

	////////////////////////////////////////////////////////////////////////////////////////////////////
public:

	/** AttachCone this component represents */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "!TGOR Movement")
		TSubclassOf<UTGOR_AttachCone> SpawnAttachCone;

	////////////////////////////////////////////////////////////////////////////////////////////////////
protected:

	/*  */
	UPROPERTY(BlueprintReadOnly, Category = "!TGOR Movement", Meta = (Keywords = "C++"))
		UTGOR_AttachCone* AttachCone;

	////////////////////////////////////////////////////////////////////////////////////////////////////
public:

	/** Traces along the cone center, returns true if trace was successful and hit something. */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Movement")
		bool TraceCenter(UTGOR_PilotComponent* Component, const FTGOR_MovementSpace& Space, double TraceRadius, double LengthMultiplier, FTGOR_ConeTraceOutput& Output) const;

	/** Traces along the cone center, averaging according to a second trace in movement direction, returns true if trace was successful and hit something. */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Movement")
		bool TraceMoving(UTGOR_PilotComponent* Component, const FTGOR_MovementSpace& Space, double MaxSpeed, double LerpMultiplier, double TraceRadius, double LengthMultiplier, FTGOR_ConeTraceOutput& Output) const;

	////////////////////////////////////////////////////////////////////////////////////////////////////
public:

	/** Sphere radius for attachment limits */
	UPROPERTY(EditAnywhere, Category = "!TGOR Movement")
		double LimitRadius;

	/** Radial offset along x axis */
	UPROPERTY(EditAnywhere, Category = "!TGOR Movement")
		double OffsetAngleX;

	/** Radial offset along y axis */
	UPROPERTY(EditAnywhere, Category = "!TGOR Movement")
		double OffsetAngleY;

	/** Sphere angle for attachment limits along X axis */
	UPROPERTY(EditAnywhere, Category = "!TGOR Movement")
		double LimitAngleX;

	/** Sphere angle for attachment limits along Y axis */
	UPROPERTY(EditAnywhere, Category = "!TGOR Movement")
		double LimitAngleY;
	
	////////////////////////////////////////////////////////////////////////////////////////////////////
public:

	// Checks whether a point in local space is inside the cone
	UFUNCTION(BlueprintPure, Category = "!TGOR Movement")
		bool IsInside(const FVector& Point) const;

	// Returns angle limit away from alignment direction for a given direction in local space
	UFUNCTION(BlueprintPure, Category = "!TGOR Movement")
		double GetProjectedLimit(const FVector& Vector) const;

	// Returns closest radial projection on the cone if not already inside
	UFUNCTION(BlueprintPure, Category = "!TGOR Movement")
		FVector ProjectInside(const FVector& Point) const;

	UFUNCTION(BlueprintCallable, Category = "!TGOR Movement")
		void SetLimits(double Radius, double AngleX, double AngleY);

	UFUNCTION(BlueprintCallable, Category = "!TGOR Movement")
		void SetOffsets(double OffX, double OffY);

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
		void DebugDrawCollision(double Size, double Duration);

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
