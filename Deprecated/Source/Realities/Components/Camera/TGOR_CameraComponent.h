// The Gateway of Realities: Planes of Existence.

#pragma once

#include <functional>

#include "Realities/Utility/TGOR_Math.h"
#include "Realities/Utility/Error/TGOR_Error.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Engine/Classes/GameFramework/SpringArmComponent.h"

#include "Engine/Classes/Materials/MaterialParameterCollection.h"

#include "CoreMinimal.h"
#include "Realities/Interfaces/TGOR_SingletonInterface.h"
#include "Realities/Interfaces/TGOR_RegisterInterface.h"
#include "Camera/CameraComponent.h"
#include "TGOR_CameraComponent.generated.h"

class UTGOR_Camera;

////////////////////////////////////////////// STRUCT //////////////////////////////////////////////////////


USTRUCT(BlueprintType)
struct FTGOR_CameraModifier
{
	GENERATED_USTRUCT_BODY()
		FTGOR_CameraModifier();

	/** Camera parameters */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "TGOR Camera")
		FVector4 Params;

	/** Camera blend speed/time */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "TGOR Camera")
		float Weight;

	/** Camera blend speed/time */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "TGOR Camera")
		float Blend;
};


USTRUCT(BlueprintType)
struct FTGOR_CameraHandle
{
	GENERATED_USTRUCT_BODY()
		FTGOR_CameraHandle();

		/** Register to poll for activity */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "TGOR Camera")
		TScriptInterface<ITGOR_RegisterInterface> Register;

	/** Enabled modifiers by this handle */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "TGOR Camera")
		TMap<UTGOR_Camera*, FTGOR_CameraModifier> Params;
};


USTRUCT(BlueprintType)
struct FTGOR_CameraQueue
{
	GENERATED_USTRUCT_BODY()
		FTGOR_CameraQueue();

	/** Content layers in order from bottom to top */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "TGOR Camera")
		TArray<UTGOR_Content*> Layers;
};


/**
 * 
 */
UCLASS(ClassGroup = (Custom), Blueprintable, meta = (BlueprintSpawnableComponent))
class REALITIES_API UTGOR_CameraComponent : public UCameraComponent, public ITGOR_SingletonInterface
{
	GENERATED_BODY()
	
public:
	UTGOR_CameraComponent(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	virtual void BeginPlay() override;

	/** Copies current state of this camera to another camera, resets fixed state if teleport */
	UFUNCTION(BlueprintCallable, Category = "TGOR Camera", Meta = (Keywords = "C++"))
		void CopyToCamera(UTGOR_CameraComponent* Camera, float Blend, bool Teleport);

	////////////////////////////////////////////////////////////////////////////////////////////////////
public:

	/** Register modifier to go active. */
	UFUNCTION(BlueprintCallable, Category = "TGOR Camera", Meta = (Keywords = "C++"))
		void RegisterHandle(TScriptInterface<ITGOR_RegisterInterface> Register, UTGOR_Content* Content, TSubclassOf<UTGOR_Camera> Type, const FVector& Params, float Blend = -1.0);
	
	/** Unregister cameras manually. */
	UFUNCTION(BlueprintCallable, Category = "TGOR Camera", Meta = (Keywords = "C++"))
		void UnregisterHandle(UTGOR_Content* Content, TSubclassOf<UTGOR_Camera> Type);

	/** Fills modifier from spawn setup */
	UFUNCTION(BlueprintCallable, Category = "TGOR Creature", Meta = (Keywords = "C++"))
		void BuildModifiers(UTGOR_Content* Content);

	/** Move camera according to handles. */
	UFUNCTION(BlueprintCallable, Category = "TGOR Camera", Meta = (Keywords = "C++"))
		void TickCamera(float DeltaTime);

	/** Fast forward ease queues. */
	UFUNCTION(BlueprintCallable, Category = "TGOR Camera", Meta = (Keywords = "C++"))
		void FastForward();

	////////////////////////////////////////////////////////////////////////////////////////////////////
protected:

	/** Generate queues from currently registered handles. */
	UFUNCTION(BlueprintCallable, Category = "TGOR Camera", Meta = (Keywords = "C++"))
		void BuildCameraQueues(TMap<UTGOR_Camera*, FTGOR_CameraQueue>& Queues);

	/** Eases in a modifier, returns whether easing in is done. */
	UFUNCTION(BlueprintCallable, Category = "TGOR Camera", Meta = (Keywords = "C++"))
		bool EaseModifierIn(FTGOR_CameraModifier& Modifier, float DeltaTime);

	/** Eases out a modifier, returns whether easing out is done. */
	UFUNCTION(BlueprintCallable, Category = "TGOR Camera", Meta = (Keywords = "C++"))
		bool EaseModifierOut(FTGOR_CameraModifier& Modifier, float DeltaTime);

	/** Handles for camera modifiers */
	UPROPERTY(BlueprintReadOnly, Category = "TGOR Camera")
		TMap<UTGOR_Content*, FTGOR_CameraHandle> Handles;

	/** Easing out modifiers */
	UPROPERTY(BlueprintReadOnly, Category = "TGOR Camera")
		TMap<UTGOR_Camera*, FTGOR_CameraModifier> EaseOuts;

	/** Set of currently supported Modifiers */
	UPROPERTY(BlueprintReadWrite, Category = "TGOR Camera")
		TMap<UTGOR_Camera*, FVector4> Cameras;

	////////////////////////////////////////////////////////////////////////////////////////////////////
protected:

	/** Camera world rotation */
	UPROPERTY(BlueprintReadOnly, Category = "TGOR Camera", Meta = (Keywords = "C++"))
		FQuat ViewRotation;

	/** Springarm component this camera is parented to */
	UPROPERTY(BlueprintReadOnly, Category = "TGOR Camera", Meta = (Keywords = "C++"))
		USpringArmComponent* SpringArm;

	/** Control given to the player (Rotate Camera) */
	UPROPERTY(BlueprintReadOnly, Category = "TGOR Camera", Meta = (Keywords = "C++"))
		float ControlThrottle;

	/** How much camera adheres to camera arm collision */
	UPROPERTY(BlueprintReadOnly, Category = "TGOR Camera", Meta = (Keywords = "C++"))
		float CollisionStrength;

	/** Degree of smoothstep */
	UPROPERTY(BlueprintReadOnly, Category = "TGOR Camera", Meta = (Keywords = "C++"))
		int32 SmoothStepDegree;

	/** MaterialCollection to use for lerping */
	UPROPERTY(BlueprintReadOnly, Category = "TGOR Camera", Meta = (Keywords = "C++"))
		UMaterialParameterCollection* MaterialCollection;
	
	////////////////////////////////////////////////////////////////////////////////////////////////////
public:


	/** Rotates camera towards the given up vector */
	UFUNCTION(BlueprintCallable, Category = "TGOR Camera", Meta = (Keywords = "C++"))
		void RotateCameraUpside(const FVector& UpVector, float Amount, bool Control = false);

	/** Camera rotation along axis, if control is true this is only applied if not fixed */
	UFUNCTION(BlueprintCallable, Category = "TGOR Camera", Meta = (Keywords = "C++"))
		void RotateCamera(const FVector& Axis, float Angle, bool Control = true);

	/** Moves camera to a worldlocation instantly while still looking at player */
	UFUNCTION(BlueprintCallable, Category = "TGOR Camera", Meta = (Keywords = "C++"))
		void TranslateCameraTowards(const FVector& WorldLocation, float Blend);

	/** Rotate camera towards a worldlocation instantly while still looking at player */
	UFUNCTION(BlueprintCallable, Category = "TGOR Camera", Meta = (Keywords = "C++"))
		void RotateCameraTowards(const FVector& WorldLocation, float Blend);

	/** Set camera rotation */
	UFUNCTION(BlueprintCallable, Category = "TGOR Camera", Meta = (Keywords = "C++"))
		void SetViewRotation(const FQuat& Quat);

	/** Get view rotation */
	UFUNCTION(BlueprintPure, Category = "TGOR Camera", Meta = (Keywords = "C++"))
		FQuat GetViewRotation() const;

	/** Get desired camera location after camera arm transformations */
	UFUNCTION(BlueprintPure, Category = "TGOR Camera", Meta = (Keywords = "C++"))
		FVector GetDesiredCameraLocation() const;

	/** Set how much camera adhers to collision */
	UFUNCTION(BlueprintCallable, Category = "TGOR Camera", Meta = (Keywords = "C++"))
		void SetCollisionStrength(float Strength);

	/** Get camera rotation */
	UFUNCTION(BlueprintPure, Category = "TGOR Camera", Meta = (Keywords = "C++"))
		FQuat GetControlRotation() const;

	/** Set how much control player has over camera rotation in [0, 1] */
	UFUNCTION(BlueprintCallable, Category = "TGOR Camera", Meta = (Keywords = "C++"))
		void SetControlThrottle(float Throttle);

	/** Get whether camera control is currently throttled */
	UFUNCTION(BlueprintCallable, Category = "TGOR Camera", Meta = (Keywords = "C++"))
		float GetControlThrottle() const;

};
