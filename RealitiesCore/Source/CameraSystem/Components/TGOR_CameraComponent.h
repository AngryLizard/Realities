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
struct FTGOR_CameraState
{
	GENERATED_USTRUCT_BODY()

	/** Camera content belonging to this state */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "!TGOR Camera")
		UTGOR_Camera* Camera = nullptr;

	/** Current camera input */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "!TGOR Camera")
		FVector4 Input = FVector4(0,0,0,0);

	/** Current blend velocity */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "!TGOR Camera")
		FVector4 InputVelocity = FVector4(0, 0, 0, 0);

	/** Current camera parameters */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "!TGOR Camera")
		FVector4 TargetInput = FVector4(0, 0, 0, 0);

	/** Current blend Force (Spring constant) */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "!TGOR Camera")
		float BlendForce = 0;
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
		void CopyToCamera(UTGOR_CameraComponent* Camera, bool Teleport);

	////////////////////////////////////////////////////////////////////////////////////////////////////
public:

	/** Set camera parameter, <= 0 for no blending. */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Camera", Meta = (Keywords = "C++"))
		void ApplyCameraEffect(TSubclassOf<UTGOR_Camera> Type, const FVector& Params, float Blend = -1.0);
	
	/** Camera setup this component spawns with. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "!TGOR Camera")
		TArray<TSubclassOf<UTGOR_Camera>> SpawnCameras;

	/** Minimum camera pitch in degrees. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "!TGOR Camera")
		float MinPitch;

	/** Maximum camera pitch in degrees. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "!TGOR Camera")
		float MaxPitch;

	////////////////////////////////////////////////////////////////////////////////////////////////////
protected:

	/** Currently running camera effects in the order they're applied */
	UPROPERTY(BlueprintReadWrite, Category = "!TGOR Camera")
		TArray<FTGOR_CameraState> Cameras;

	/** Simulate camera states */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Camera", Meta = (Keywords = "C++"))
		void SimulateCameras(float DeltaTime);

	////////////////////////////////////////////////////////////////////////////////////////////////////
protected:

	/** Springarm component this camera is parented to */
	UPROPERTY(BlueprintReadOnly, Category = "!TGOR Camera", Meta = (Keywords = "C++"))
		USpringArmComponent* SpringArm;

	/** Control given to the player (Rotate Camera) */
	UPROPERTY(BlueprintReadOnly, Category = "!TGOR Camera", Meta = (Keywords = "C++"))
		float ControlThrottle;

	/** How much camera adheres to camera arm collision */
	UPROPERTY(BlueprintReadOnly, Category = "!TGOR Camera", Meta = (Keywords = "C++"))
		float CollisionStrength;

	/** MaterialCollection to use for lerping */
	UPROPERTY(BlueprintReadOnly, Category = "!TGOR Camera", Meta = (Keywords = "C++"))
		UMaterialParameterCollection* MaterialCollection;
	
	////////////////////////////////////////////////////////////////////////////////////////////////////
public:

	/** Get desired camera location after camera arm transformations */
	UFUNCTION(BlueprintPure, Category = "!TGOR Camera", Meta = (Keywords = "C++"))
		FVector GetDesiredCameraLocation() const;

	/** Set how much camera adhers to collision */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Camera", Meta = (Keywords = "C++"))
		void SetCollisionStrength(float Strength);

	/** Set camera rotation */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Camera", Meta = (Keywords = "C++"))
		void SetViewRotation(const FQuat& Quat);

	/** Get camera rotation */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Camera", Meta = (Keywords = "C++"))
		FQuat GetViewRotation() const;

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

	/** Horizontal camera rotation around current up vector in degrees */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Camera", Meta = (Keywords = "C++"))
		void RotateCameraHorizontally(float Amount);

	/** Vertical camera rotation around control rotation right axis  in degrees */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Camera", Meta = (Keywords = "C++"))
		void RotateCameraVertically(float Amount);

};
