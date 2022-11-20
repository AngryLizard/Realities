// The Gateway of Realities: Planes of Existence.

#pragma once

#include "CoreMinimal.h"

#include "DimensionSystem/Interfaces/TGOR_SpawnerInterface.h"
#include "Components/ShapeComponent.h"
#include "TGOR_AimTargetComponent.generated.h"

class UTGOR_Target;

USTRUCT(BlueprintType)
struct FTGOR_AimQueryParams
{
	GENERATED_USTRUCT_BODY();

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "!TGOR Aim")
		TSubclassOf<UTGOR_Target> Filter;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "!TGOR Aim")
		FVector Origin = FVector::ZeroVector;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "!TGOR Aim")
		FVector Direction = FVector::ForwardVector;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "!TGOR Aim")
		float MaxAimDistance;
};

/**
 * 
 */
UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class TARGETSYSTEM_API UTGOR_AimTargetComponent : public UShapeComponent, public ITGOR_SpawnerInterface
{
	GENERATED_BODY()

public:

	UTGOR_AimTargetComponent();
	virtual void UpdateContent_Implementation(FTGOR_SpawnerDependencies& Dependencies) override;
	virtual TMap<int32, UTGOR_SpawnModule*> GetModuleType_Implementation() const override;

public:
	////////////////////////////////////////////////////////////////////////////////////////////////////

	/** Target radius of targetable */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "!TGOR Aim")
		float TargetRadius = 300.0f;

	/** Threshold percentage of when target becomes activatable (used for user input events) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "!TGOR Aim")
		float ActivationThreshold = 1.0f;

	/** Whether targeting is enabled */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "!TGOR Aim")
		bool CanBeTargeted = true;

	////////////////////////////////////////////////////////////////////////////////////////////////////
public:

	/** Project ray onto a plane facing camera and located at this activator */
	UFUNCTION(BlueprintPure, Category = "!TGOR Dialogue", Meta = (Keywords = "C++"))
		FVector ProjectRay(const FVector& RayOrigin, const FVector& RayDirection) const;

	/** Computes relative distance of a point to given target center */
	UFUNCTION(BlueprintPure, Category = "!TGOR Dialogue", Meta = (Keywords = "C++"))
		float ComputeRelativeDistance(const FVector& Location, const FVector& Center, float Threshold = 1.0f) const;

	/** Query target focus location */
	UFUNCTION(BlueprintPure, Category = "!TGOR Aim", Meta = (Keywords = "C++"))
		virtual FTransform GetTargetTransform() const;

	/** Convert a world location to local domain */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Dialogue", Meta = (Keywords = "C++"))
		virtual FVector WorldToTarget(const FVector& Location) const;

	/** Convert local domain to world transform */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Dialogue", Meta = (Keywords = "C++"))
		virtual FVector TargetToWorld(const FVector& Local) const;

	////////////////////////////////////////////////////////////////////////////////////////////////////
public:

	/** Entity type this interactable spawns with. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "!TGOR Aim")
		TArray<TSubclassOf<UTGOR_Target>> SpawnTargets;

	////////////////////////////////////////////////////////////////////////////////////////////////////

	/** Array of currently supported targets */
	UPROPERTY(BlueprintReadWrite, Category = "!TGOR Aim")
		TArray<UTGOR_Target*> Targets;

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
	virtual void UpdateBodySetup() override;
	virtual FPrimitiveSceneProxy* CreateSceneProxy() override;
	virtual bool IsZeroExtent() const override;
	virtual bool AreSymmetricRotations(const FQuat& A, const FQuat& B, const FVector& Scale3D) const override;
	//~ End UPrimitiveComponent Interface.

	class TARGETSYSTEM_API FDrawAimTargetSceneProxy : public FPrimitiveSceneProxy
	{
	public:
		SIZE_T GetTypeHash() const override;
		FDrawAimTargetSceneProxy(const UTGOR_AimTargetComponent* InComponent);

		virtual void DrawTargetSpace(const FMatrix& Transform, const FLinearColor& DrawColor, FPrimitiveDrawInterface* PDI) const {};
		virtual void GetDynamicMeshElements(const TArray<const FSceneView*>& Views, const FSceneViewFamily& ViewFamily, uint32 VisibilityMap, FMeshElementCollector& Collector) const;
		virtual FPrimitiveViewRelevance GetViewRelevance(const FSceneView* View) const;
		virtual uint32 GetMemoryFootprint(void) const override { return(sizeof(*this) + GetAllocatedSize()); }
		uint32 GetAllocatedSize(void) const { return(FPrimitiveSceneProxy::GetAllocatedSize()); }

	protected:
		const uint32  bDrawOnlyIfSelected : 1;
		const FColor  ShapeColor;
		const float TargetRadius;
		const float ActivationThreshold;
	};
};
