// The Gateway of Realities: Planes of Existence.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Engine/Classes/GameFramework/SpringArmComponent.h"

#include "DimensionSystem/TGOR_PilotInstance.h"
#include "CameraSystem/TGOR_CameraInstance.h"

#include "CoreSystem/Interfaces/TGOR_RegisterInterface.h"
#include "CoreSystem/Interfaces/TGOR_SingletonInterface.h"
#include "DimensionSystem/Interfaces/TGOR_SpawnerInterface.h"
#include "Camera/CameraComponent.h"
#include "TGOR_CameraComponent.generated.h"

class UTGOR_Camera;
class UTGOR_Stage;

////////////////////////////////////////////// STRUCT //////////////////////////////////////////////////////


USTRUCT(BlueprintType)
struct FTGOR_CameraModifier
{
	GENERATED_USTRUCT_BODY()
		FTGOR_CameraModifier();

	/** Camera parameters */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "!TGOR Camera")
		FVector4 Params;

	/** Camera blend speed/time */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "!TGOR Camera")
		float Weight;

	/** Camera blend speed/time */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "!TGOR Camera")
		float Blend;
};


USTRUCT(BlueprintType)
struct FTGOR_CameraHandle
{
	GENERATED_USTRUCT_BODY()
		FTGOR_CameraHandle();

		/** Register to poll for activity */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "!TGOR Camera")
		TScriptInterface<ITGOR_RegisterInterface> Register;

	/** Enabled modifiers by this handle */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "!TGOR Camera")
		TMap<UTGOR_Camera*, FTGOR_CameraModifier> Params;
};


USTRUCT(BlueprintType)
struct FTGOR_CameraQueue
{
	GENERATED_USTRUCT_BODY()
		FTGOR_CameraQueue();

	/** Content layers in order from bottom to top */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "!TGOR Camera")
		TArray<UTGOR_CoreContent*> Layers;
};


/**
 * 
 */
UCLASS(ClassGroup = (Custom), Blueprintable, meta = (BlueprintSpawnableComponent))
class CAMERASYSTEM_API UTGOR_CameraComponent : public UCameraComponent, public ITGOR_SingletonInterface, public ITGOR_SpawnerInterface
{
	GENERATED_BODY()
	
public:
	UTGOR_CameraComponent(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	virtual void BeginPlay() override;

	virtual void UpdateContent_Implementation(FTGOR_SpawnerDependencies& Dependencies) override;
	virtual TMap<int32, UTGOR_SpawnModule*> GetModuleType_Implementation() const override;

	////////////////////////////////////////////////////////////////////////////////////////////////////
public:

	/** Copies current state of this camera to another camera, resets fixed state if teleport */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Camera", Meta = (Keywords = "C++"))
		void CopyToCamera(UTGOR_CameraComponent* Camera, float Blend, bool Teleport);

	////////////////////////////////////////////////////////////////////////////////////////////////////
public:

	/** Register modifier to go active. */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Camera", Meta = (Keywords = "C++"))
		void RegisterHandle(TScriptInterface<ITGOR_RegisterInterface> Register, UTGOR_CoreContent* Content, TSubclassOf<UTGOR_Camera> Type, const FVector& Params, float Blend = -1.0);
	
	/** Unregister cameras manually. */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Camera", Meta = (Keywords = "C++"))
		void UnregisterHandle(UTGOR_CoreContent* Content, TSubclassOf<UTGOR_Camera> Type);

	/** Fast forward ease queues. */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Camera", Meta = (Keywords = "C++"))
		void FastForward();

	/** Camera setup this component spawns with. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "!TGOR Camera")
		TArray<TSubclassOf<UTGOR_Camera>> SpawnCameras;

	////////////////////////////////////////////////////////////////////////////////////////////////////
protected:

	/** Generate queues from currently registered handles. */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Camera", Meta = (Keywords = "C++"))
		void BuildCameraQueues(TMap<UTGOR_Camera*, FTGOR_CameraQueue>& Queues);

	/** Eases in a modifier, returns whether easing in is done. */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Camera|Internal", Meta = (Keywords = "C++"))
		bool EaseModifierIn(FTGOR_CameraModifier& Modifier, float DeltaTime);

	/** Eases out a modifier, returns whether easing out is done. */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Camera|Internal", Meta = (Keywords = "C++"))
		bool EaseModifierOut(FTGOR_CameraModifier& Modifier, float DeltaTime);

	/** Handles for camera modifiers */
	UPROPERTY(BlueprintReadOnly, Category = "!TGOR Camera")
		TMap<UTGOR_CoreContent*, FTGOR_CameraHandle> Handles;

	/** Easing out modifiers */
	UPROPERTY(BlueprintReadOnly, Category = "!TGOR Camera")
		TMap<UTGOR_Camera*, FTGOR_CameraModifier> EaseOuts;

	/** Set of currently supported Modifiers */
	UPROPERTY(BlueprintReadWrite, Category = "!TGOR Camera")
		TMap<UTGOR_Camera*, FVector4> Cameras;

	////////////////////////////////////////////////////////////////////////////////////////////////////
protected:

	/** Camera world rotation */
	UPROPERTY(BlueprintReadOnly, Category = "!TGOR Camera", Meta = (Keywords = "C++"))
		FQuat ViewRotation;

	/** Springarm component this camera is parented to */
	UPROPERTY(BlueprintReadOnly, Category = "!TGOR Camera", Meta = (Keywords = "C++"))
		USpringArmComponent* SpringArm;

	/** Control given to the player (Rotate Camera) */
	UPROPERTY(BlueprintReadOnly, Category = "!TGOR Camera", Meta = (Keywords = "C++"))
		float ControlThrottle;

	/** How much camera adheres to camera arm collision */
	UPROPERTY(BlueprintReadOnly, Category = "!TGOR Camera", Meta = (Keywords = "C++"))
		float CollisionStrength;

	/** Degree of smoothstep */
	UPROPERTY(BlueprintReadOnly, Category = "!TGOR Camera", Meta = (Keywords = "C++"))
		int32 SmoothStepDegree;

	/** MaterialCollection to use for lerping */
	UPROPERTY(BlueprintReadOnly, Category = "!TGOR Camera", Meta = (Keywords = "C++"))
		UMaterialParameterCollection* MaterialCollection;
	
	////////////////////////////////////////////////////////////////////////////////////////////////////
public:


	/** Rotates camera towards the given up vector */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Camera", Meta = (Keywords = "C++"))
		void RotateCameraUpside(const FVector& UpVector, float Amount, bool Control = false);

	/** Camera rotation along axis, if control is true this is only applied if not fixed */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Camera", Meta = (Keywords = "C++"))
		void RotateCamera(const FVector& Axis, float Angle, bool Control = true);

	/** Set camera rotation */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Camera", Meta = (Keywords = "C++"))
		void SetViewRotation(const FQuat& Quat);

	/** Get view rotation */
	UFUNCTION(BlueprintPure, Category = "!TGOR Camera", Meta = (Keywords = "C++"))
		FQuat GetViewRotation() const;

	/** Get desired camera location after camera arm transformations */
	UFUNCTION(BlueprintPure, Category = "!TGOR Camera", Meta = (Keywords = "C++"))
		FVector GetDesiredCameraLocation() const;

	/** Set how much camera adhers to collision */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Camera", Meta = (Keywords = "C++"))
		void SetCollisionStrength(float Strength);

	/** Get camera rotation */
	UFUNCTION(BlueprintPure, Category = "!TGOR Camera", Meta = (Keywords = "C++"))
		FQuat GetControlRotation() const;

	/** Set how much control player has over camera rotation in [0, 1] */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Camera", Meta = (Keywords = "C++"))
		void SetControlThrottle(float Throttle);

	/** Get whether camera control is currently throttled */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Camera", Meta = (Keywords = "C++"))
		float GetControlThrottle() const;

	////////////////////////////////////////////////////////////////////////////////////////////////////
public:

	/** Horizontal camera rotation around current up vector */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Camera", Meta = (Keywords = "C++"))
		void RotateCameraHorizontally(float Amount);

	/** Vertical camera rotation around control rotation right axis */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Camera", Meta = (Keywords = "C++"))
		void RotateCameraVertically(float Amount);

	/** Get camera rotation in pawn local space */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Camera", Meta = (Keywords = "C++"))
		FQuat ToLocalCameraRotation(const FTGOR_MovementPosition& Position) const;

	/** Set camera rotation from pawn local space */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Camera", Meta = (Keywords = "C++"))
		void FromLocalCameraRotation(const FTGOR_MovementPosition& Position, const FQuat& Quat);
};
