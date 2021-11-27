// The Gateway of Realities: Planes of Existence.

#pragma once

#include <functional>
#include "CoreMinimal.h"

#include "CustomisationSystem/TGOR_BodypartInstance.h"

#include "Components/SkinnedMeshComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "CoreSystem/Interfaces/TGOR_SingletonInterface.h"
#include "TGOR_ControlSkeletalMeshComponent.generated.h"

class UControlRig;
class UTGOR_ControlComponent;
class UTGOR_ControlSkeletalMeshComponent;
class ITGOR_ControlInterface;


USTRUCT()
struct FTGOR_ControlRigCache
{
	GENERATED_BODY()

public:
	void InitialiseControls(TSubclassOf<UControlRig> ControlRigClass, UObject* Owner, USkinnedMeshComponent* SkinnedMesh, bool bForce);
	void InitialiseControlTransforms(USkinnedMeshComponent* SkinnedMesh);
	void UpdateTransforms(TArray<FTransform>& Transforms, USkinnedMeshComponent* SkinnedMesh, float DeltaTime);
	void UpdateControlTransforms(USkinnedMeshComponent* SkinnedMesh, bool bIsInitial);
	bool HasMapping() const;

	UPROPERTY(transient)
		UControlRig* ControlRig = nullptr;

	UPROPERTY(transient)
		TMap<FName, uint16> RigBoneMapping;

	UPROPERTY(transient)
		TMap<FName, uint16> RigCurveMapping;

	UPROPERTY(transient)
		TMap<uint16, TScriptInterface<ITGOR_ControlInterface>> ControlMapping;
};


/**
 * 
 */
UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class CUSTOMISATIONSYSTEM_API UTGOR_ControlSkeletalMeshComponent : public USkinnedMeshComponent, public ITGOR_SingletonInterface
{
	GENERATED_BODY()

public:
	UTGOR_ControlSkeletalMeshComponent();
	virtual void InitializeComponent() override;
	virtual void OnRegister() override;
	virtual void SetSkeletalMesh(class USkeletalMesh* NewMesh, bool bReinitPose = true) override;
	virtual void TickPose(float DeltaTime, bool bNeedsValidRootMotion) override;

	//////////////////////////////////////////// IMPLEMENTABLES ////////////////////////////////////////

	////////////////////////////////////////////////////////////////////////////////////////////////////
public:

	/** Cached ControlRig, this ControlRig drives the Skeleton */
	UPROPERTY(EditAnywhere, Category = "!TGOR Animation")
		TSubclassOf<UControlRig> TargetRigClass;

	////////////////////////////////////////////////////////////////////////////////////////////////////
protected:

	UPROPERTY(transient)
		FTGOR_ControlRigCache TargetCache;

	////////////////////////////////////////////////////////////////////////////////////////////////////
public:

	/** Gets the relative transform of one component to this mesh */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Animation", Meta = (Keywords = "C++"))
		static FTransform GetRelativeControlTransform(USkinnedMeshComponent* Parent, const USceneComponent* Child);

	/**  */
	UFUNCTION()
		virtual void InitialiseControls(bool bForce);

	/**  */
	UFUNCTION()
		virtual void UpdateTransforms(float DeltaTime);

	//~ Begin USkinnedMeshComponent Interface
	virtual void RefreshBoneTransforms(FActorComponentTickFunction* TickFunction = NULL) override;
	virtual bool AllocateTransformData() override;
	virtual bool ShouldUpdateTransform(bool bLODHasChanged) const override;
	//~ End USkinnedMeshComponent Interface

	void FillComponentSpaceTransforms();

#if WITH_EDITORONLY_DATA
	/** If true, this will Tick until disabled */
	UPROPERTY(EditAnywhere, Category = "!TGOR Animation")
		bool bUpdateAnimationInEditor = true;
#endif

#if WITH_EDITOR
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif // WITH_EDITOR

	////////////////////////////////////////////////////////////////////////////////////////////////////
protected:

	/** Initialise controlrig control if AutoInitialiseControlRigenabled. */
	virtual void InitAutoControlRig(bool bForceReinit = false);

	// this is marked if transform has to be updated
	bool bNeedsRefreshTransform;

	////////////////////////////////////////////////////////////////////////////////////////////////////
protected:

	/** Temporary array of rotation/translation/scale for each bone. */
	TArray<FTransform> LocalSpaceTransforms;
};
