// The Gateway of Realities: Planes of Existence.

#pragma once

#include "CoreMinimal.h"

#include "CustomisationSystem/Components/TGOR_ControlSkeletalMeshComponent.h"
#include "TGOR_RetargetComponent.generated.h"


/**
 * UTGOR_RetargetComponent forwards controls from a different skeletal mesh to the driving target ControlRig. The source SkeletalMesh component must be the direct parent of this component.
 */
UCLASS(ClassGroup = (Custom), Blueprintable, meta = (BlueprintSpawnableComponent))
class CUSTOMISATIONSYSTEM_API UTGOR_RetargetComponent : public UTGOR_ControlSkeletalMeshComponent
{
	GENERATED_BODY()

public:
	UTGOR_RetargetComponent();
	virtual void InitialiseControls(bool bForce) override;
	virtual void UpdateTransforms(float DeltaTime) override;
	virtual void InitAutoControlRig(bool bForceReinit = false) override;

	////////////////////////////////////////////////////////////////////////////////////////////////////
public:

	/** Cached ControlRig, this ControlRig drives the TargetRig */
	UPROPERTY(EditAnywhere, Category = "!TGOR Animation")
		TSubclassOf<UControlRig> SourceRigClass;

	////////////////////////////////////////////////////////////////////////////////////////////////////
protected:

	UPROPERTY(transient)
		USkeletalMeshComponent* SourceComponent;

	UPROPERTY(transient)
		FTGOR_ControlRigCache SourceCache;

	UPROPERTY(transient)
		TMap<int32, int32> RetargetRigControlMapping;

	////////////////////////////////////////////////////////////////////////////////////////////////////
public:

#if WITH_EDITOR
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif // WITH_EDITOR

};
