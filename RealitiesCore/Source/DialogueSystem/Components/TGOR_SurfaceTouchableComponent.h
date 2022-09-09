// The Gateway of Realities: Planes of Existence.

#pragma once

#include "CoreMinimal.h"

#include "TGOR_TouchableComponent.h"
#include "TGOR_SurfaceTouchableComponent.generated.h"


UENUM(BlueprintType)
enum class ETGOR_DomainControlType : uint8
{
	Projection UMETA(ToolTip = "Use cursor location"),
	Displacement UMETA(ToolTip = "Use change of cursor location")
};

UENUM(BlueprintType)
enum class ETGOR_DomainSimulationType : uint8
{
	Position UMETA(ToolTip = "Direct translation"),
	Velocity UMETA(ToolTip = "Spring simulation")
};

/**
 * UTGOR_SurfaceTouchableComponent
 */
UCLASS(ClassGroup = (Custom), Blueprintable, meta = (BlueprintSpawnableComponent))
class DIALOGUESYSTEM_API UTGOR_SurfaceTouchableComponent : public UTGOR_TouchableComponent
{
	GENERATED_BODY()

public:
	UTGOR_SurfaceTouchableComponent();
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	virtual bool GetTargetProjection(const FVector& RayOrigin, const FVector& RayDirection, FVector& Projection, float& Distance) const override;
	virtual void UpdateTarget(const FVector& RayOrigin, const FVector& RayDirection, float Intensity) override;
	virtual FTransform GetTargetTransform() const override;

	//////////////////////////////////////////// IMPLEMENTABLES ////////////////////////////////////////

	////////////////////////////////////////////////////////////////////////////////////////////////////
public:

	/** Interactable surface width */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "!TGOR Interaction")
		FVector2D Size = FVector2D(100.f, 100.f);

	/** Interactable surface radius (changes bend/shape) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "!TGOR Interaction")
		FVector2D Radius = FVector2D(0.f, 0.f);

	/** Domain min value */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "!TGOR Interaction")
		FVector2D DomainMin = FVector2D(-1.f, -1.f);

	/** Domain max value */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "!TGOR Interaction")
		FVector2D DomainMax = FVector2D(1.f, 1.f);

	/** How much transform moves with the domain */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "!TGOR Interaction")
		FVector2D Adherence = FVector2D(1.f, 1.f);

	/** Type of control this touchable uses */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "!TGOR Interaction")
		ETGOR_DomainControlType ControlType = ETGOR_DomainControlType::Projection;

	/** Type of control this touchable uses */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "!TGOR Interaction")
		ETGOR_DomainSimulationType SimulationType = ETGOR_DomainSimulationType::Position;

	/** By how much to scale the cursor input domain */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "!TGOR Interaction", meta = (EditCondition = "SimulationType == ETGOR_DomainSimulationType::Position", EditConditionHides))
		float DomainScaling = 1.f;

	/** Relative pull strength */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "!TGOR Interaction", meta = (EditCondition = "SimulationType == ETGOR_DomainSimulationType::Velocity", EditConditionHides))
		float Strength = 1.f;

	/** Spring constant towards resting point */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "!TGOR Interaction", meta = (EditCondition = "SimulationType == ETGOR_DomainSimulationType::Velocity", EditConditionHides))
		float SpringTowardsCenter = 0.f;

	/** Multiple of critical damping used for damping */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "!TGOR Interaction", meta = (EditCondition = "SimulationType == ETGOR_DomainSimulationType::Velocity", EditConditionHides))
		float SpringDampingRatio = 1.f;

	////////////////////////////////////////////////////////////////////////////////////////////////////
public:

	/** Convert ray to target domain */
	UFUNCTION(BlueprintPure, Category = "!TGOR Interaction", Meta = (Keywords = "C++"))
		FVector2D ConvertToTargetDomain(const FVector& CursorProjection) const;

	/** Get domain position */
	UFUNCTION(BlueprintPure, Category = "!TGOR Interaction", Meta = (Keywords = "C++"))
		FVector2D GetDomainPosition() const;

	/** Get transform relative to global transform */
	UFUNCTION(BlueprintPure, Category = "!TGOR Interaction", Meta = (Keywords = "C++"))
		FTransform GetRelativeTargetTransform() const;

	////////////////////////////////////////////////////////////////////////////////////////////////////
private:

	/** Target domain objective position */
	FVector2D CurrentTargetDomainCursor;

	/** State in target domain of this touchable, what this means depends on the implementation */
	FVector2D CurrentTargetDomainPosition;

	/** Velocity in target domain of this touchable */
	FVector2D CurrentTargetDomainVelocity;

	/** Last cursor location in target domain of this touchable */
	FVector2D LastTargetDomainCursor;

	////////////////////////////////////////////////////////////////////////////////////////////////////
private:

	// USceneComponent interface
	virtual bool HasAnySockets() const override;
	virtual FTransform GetSocketTransform(FName InSocketName, ERelativeTransformSpace TransformSpace = RTS_World) const override;
	virtual void QuerySupportedSockets(TArray<FComponentSocketDescription>& OutSockets) const override;
	// End of USceneComponent interface

	/** The name of the socket at the end of the spring arm (looking back towards the spring arm origin) */
	static const FName SocketName;

	////////////////////////////////////////////////////////////////////////////////////////////////////
public:

	//~ Begin UObject Interface
#if WITH_EDITOR
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif // WITH_EDITOR
	//~ End UObject Interface

	//~ Begin UPrimitiveComponent Interface.
	virtual FPrimitiveSceneProxy* CreateSceneProxy() override;
	//~ End UPrimitiveComponent Interface.

};

