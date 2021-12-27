// The Gateway of Realities: Planes of Existence.


#include "TGOR_CameraComponent.h"
#include "GameFramework/PlayerController.h"

#include "CameraSystem/Content/TGOR_Camera.h"
#include "DimensionSystem/Content/TGOR_Spawner.h"
#include "PhysicsSystem/Components/TGOR_PhysicsComponent.h"
#include "RealitiesUtility/Utility/TGOR_Math.h"

#include "Engine.h"
#include "Net/UnrealNetwork.h"

UTGOR_CameraComponent::UTGOR_CameraComponent(const FObjectInitializer& ObjectInitializer)
:	Super(ObjectInitializer),
	MinPitch(-75.0f),
	MaxPitch(45.0f),
	SpringArm(nullptr),
	ControlThrottle(0.0f),
	MaterialCollection(nullptr)
{
	PrimaryComponentTick.bCanEverTick = true;
	PostProcessSettings.bOverride_VignetteIntensity = true;

	CollisionStrength = 1.0f;
}


void UTGOR_CameraComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	APawn* Pawn = Cast<APawn>(GetOwner());
	if (IsValid(Pawn) && Pawn->IsLocallyControlled())
	{
		SimulateCameras(DeltaTime);
	}
}

void UTGOR_CameraComponent::BeginPlay()
{
	Super::BeginPlay();

	if (!IsValid(SpringArm))
	{
		SpringArm = Cast<USpringArmComponent>(GetAttachParent());
	}
}

void UTGOR_CameraComponent::UpdateContent_Implementation(FTGOR_SpawnerDependencies& Dependencies)
{
	if (!IsValid(SpringArm))
	{
		SpringArm = Cast<USpringArmComponent>(GetAttachParent());
	}

	TMap<UTGOR_Camera*, FTGOR_CameraState> Previous;
	for (const FTGOR_CameraState& Camera : Cameras)
	{
		Previous.Emplace(Camera.Camera, Camera);
	}

	Cameras.Empty();

	// Create all camera states, copy values from previous state if applicable
	const TArray<UTGOR_Camera*>& List = Dependencies.Spawner->GetMListFromType<UTGOR_Camera>(SpawnCameras);
	for (UTGOR_Camera* Camera : List)
	{
		if (const FTGOR_CameraState* Ptr = Previous.Find(Camera))
		{
			Cameras.Emplace(*Ptr);
		}
		else
		{
			FTGOR_CameraState State;
			State.Camera = Camera;
			State.Input = Camera->Initial(this, SpringArm);
			State.InputVelocity = FVector4(0, 0, 0, 0);
			State.TargetInput = State.Input;
			State.BlendForce = 0;
			Cameras.Emplace(State);
		}
	}
}

TMap<int32, UTGOR_SpawnModule*> UTGOR_CameraComponent::GetModuleType_Implementation() const
{
	TMap<int32, UTGOR_SpawnModule*> Modules;
	const int32 Num = Cameras.Num();
	for (int32 Index = 0; Index < Num; Index++)
	{
		Modules.Emplace(Index, Cameras[Index].Camera);
	}
	return Modules;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void UTGOR_CameraComponent::CopyToCamera(UTGOR_CameraComponent* Camera, bool Teleport)
{
	// Copy over modifiers
	if (IsValid(Camera))
	{
		Camera->SetViewRotation(SpringArm->GetComponentQuat());

		// @TODO: This can be called before any spawner calls/begin play, so we can't check compatibility with spawn setup yet.
		Camera->Cameras = Cameras;
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void UTGOR_CameraComponent::ApplyCameraEffect(TSubclassOf<UTGOR_Camera> Type, const FVector& Params, float Blend)
{
	for (FTGOR_CameraState& Camera : Cameras)
	{
		if (Camera.Camera->IsA(Type))
		{
			Camera.TargetInput = Camera.Camera->Filter(this, SpringArm, Params);
			Camera.BlendForce = Blend;
		}
	}

	// Simulate cameras in case of sudden changes
	SimulateCameras(0.0f);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void UTGOR_CameraComponent::SimulateCameras(float DeltaTime)
{
	for (FTGOR_CameraState& Camera : Cameras)
	{
		// Instantly blend if no force is applied
		if (Camera.BlendForce <= 0.0f)
		{
			Camera.Input = Camera.TargetInput;
			Camera.InputVelocity = FVector4(0, 0, 0, 0);
		}
		else
		{
			// Spring dynamics
			const float Mass = 1.0f;
			const float Damping = 2.0f * FMath::Sqrt(Camera.BlendForce * Mass);
			const FVector4 Force = (Camera.TargetInput - Camera.Input) * Camera.BlendForce - Camera.InputVelocity * Damping;

			Camera.InputVelocity += Force * DeltaTime;
			Camera.Input += Camera.InputVelocity * DeltaTime;
		}

		Camera.Camera->Apply(this, SpringArm, Camera.Input);
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void UTGOR_CameraComponent::SetViewRotation(const FQuat& Quat)
{
	SpringArm->SetWorldRotation(Quat);

	FRotator Rotator = SpringArm->GetRelativeRotation();
	Rotator.Pitch = FMath::Clamp(Rotator.Pitch, -89.0f, 89.0f);
	Rotator.Roll = 0.0f;

	SpringArm->SetRelativeRotation(Rotator);
}

FQuat UTGOR_CameraComponent::GetViewRotation() const
{
	return SpringArm->GetComponentQuat();
}

FVector UTGOR_CameraComponent::GetDesiredCameraLocation() const
{
	// Get fixed springarm location
	const FTransform Transform = SpringArm->GetSocketTransform("");
	const FVector Location = Transform.TransformPosition(SpringArm->GetRelativeLocation());

	// Get desired springarm location
	FRotator DesiredRot = SpringArm->GetTargetRotation();
	FVector ArmOrigin = SpringArm->GetComponentLocation() + SpringArm->TargetOffset;
	FVector DesiredLoc = ArmOrigin;
	DesiredLoc -= DesiredRot.Vector() * SpringArm->TargetArmLength;
	DesiredLoc += FRotationMatrix(DesiredRot).TransformVector(SpringArm->SocketOffset);

	// SpringArm->GetUnfixedCameraPosition()
	return FMath::Lerp(DesiredLoc, Location, CollisionStrength);
}

void UTGOR_CameraComponent::SetCollisionStrength(float Strength)
{
	CollisionStrength = FMath::Clamp(Strength, 0.0f, 1.0f);
}

FQuat UTGOR_CameraComponent::GetControlRotation() const
{
	return GetComponentQuat();
}

void UTGOR_CameraComponent::SetControlThrottle(float Throttle)
{
	ControlThrottle = FMath::Clamp(Throttle, 0.0f, 1.0f);
}

float UTGOR_CameraComponent::GetControlThrottle() const
{
	return ControlThrottle;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void UTGOR_CameraComponent::RotateCameraHorizontally(float Amount)
{
	FRotator Rotator = SpringArm->GetRelativeRotation();
	Rotator.Yaw += Amount;

	SpringArm->SetRelativeRotation(Rotator);
}

void UTGOR_CameraComponent::RotateCameraVertically(float Amount)
{
	FRotator Rotator = SpringArm->GetRelativeRotation();

	const float OffsetPitch = GetRelativeRotation().Pitch;
	Rotator.Pitch = FMath::Clamp(Rotator.Pitch + Amount, MinPitch - OffsetPitch, MaxPitch - OffsetPitch);

	SpringArm->SetRelativeRotation(Rotator);
}

////////////////////////////////////////////////////////////////////////////////////////////////////
