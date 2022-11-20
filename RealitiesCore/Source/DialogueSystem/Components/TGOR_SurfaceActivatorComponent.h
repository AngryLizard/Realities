// The Gateway of Realities: Planes of Existence.

#pragma once

#include "CoreMinimal.h"

#include "TGOR_ActivatorComponent.h"
#include "TGOR_SurfaceActivatorComponent.generated.h"

/**
 * UTGOR_SurfaceActivatorComponent
 */
UCLASS(ClassGroup = (Custom), Blueprintable, meta = (BlueprintSpawnableComponent))
class DIALOGUESYSTEM_API UTGOR_SurfaceActivatorComponent : public UTGOR_ActivatorComponent
{
	GENERATED_BODY()

public:
	UTGOR_SurfaceActivatorComponent();

	virtual FVector WorldToTarget(const FVector& Location) const override;
	virtual FVector TargetToWorld(const FVector& Local) const override;

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

	////////////////////////////////////////////////////////////////////////////////////////////////////
protected:

	/** Get transform relative to global transform */
	UFUNCTION(BlueprintPure, Category = "!TGOR Interaction", Meta = (Keywords = "C++"))
		FTransform GetRelativeTargetTransform(const FVector& Local) const;

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

