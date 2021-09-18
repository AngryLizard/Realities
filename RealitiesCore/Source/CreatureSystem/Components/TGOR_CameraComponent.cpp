// The Gateway of Realities: Planes of Existence.


#include "TGOR_CameraComponent.h"
#include "GameFramework/PlayerController.h"

#include "CreatureSystem/Content/Cameras/TGOR_Camera.h"
#include "CreatureSystem/Content/TGOR_Creature.h"
#include "PhysicsSystem/Components/TGOR_PhysicsComponent.h"
#include "RealitiesUtility/Utility/TGOR_Math.h"

#include "Engine.h"

FTGOR_CameraModifier::FTGOR_CameraModifier()
:	Params(FVector::ZeroVector),
	Weight(0.0f),
	Blend(0.0f)
{
}

FTGOR_CameraHandle::FTGOR_CameraHandle()
{
}

FTGOR_CameraQueue::FTGOR_CameraQueue()
{
}


UTGOR_CameraComponent::UTGOR_CameraComponent(const FObjectInitializer& ObjectInitializer)
:	Super(ObjectInitializer),
	ViewRotation(FQuat::Identity),
	SpringArm(nullptr),
	ControlThrottle(0.0f),
	SmoothStepDegree(2),
	MaterialCollection(nullptr)
{
	PrimaryComponentTick.bCanEverTick = true;
	PostProcessSettings.bOverride_VignetteIntensity = true;

	CollisionStrength = 1.0f;
}


void UTGOR_CameraComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// Update spring-arm every tick to to prevent shifting camera when the player turns
	SpringArm->SetWorldRotation(ViewRotation);
	
	APawn* Pawn = Cast<APawn>(GetOwner());
	if (IsValid(Pawn) && Pawn->IsLocallyControlled())
	{
		// Get camera queue (TODO: No need to do this every tick)
		TMap<UTGOR_Camera*, FTGOR_CameraQueue> Queues;
		BuildCameraQueues(Queues);

		// Compute base modifier for all supported cameras
		for (auto& Pair : Cameras)
		{
			const FTGOR_CameraQueue& Queue = Queues.FindOrAdd(Pair.Key);
			FTGOR_CameraModifier* Ptr = EaseOuts.Find(Pair.Key);

			// Establish initial state
			Pair.Value = Pair.Key->Initial(this, SpringArm);

			// Filter all layers that are active/lerping in
			for (UTGOR_CoreContent* Layer : Queue.Layers)
			{
				FTGOR_CameraHandle& Handle = Handles[Layer];
				FTGOR_CameraModifier& Modifier = Handle.Params[Pair.Key];

				EaseModifierIn(Modifier, DeltaTime);

				// Fast-forward if something is easing out right now TODO: transform to be smooth
				if (Ptr) Modifier.Weight = 1.0f;

				// Lerp parameters
				const FVector4 Current = Pair.Key->Filter(this, SpringArm, Modifier.Params);
				const float Blend = UTGOR_Math::SmoothStep(Modifier.Weight, SmoothStepDegree);
				Pair.Value = FMath::Lerp(Pair.Value, Current, Blend);
			}

			// Ease out if necessary
			if (Ptr)
			{
				if (EaseModifierOut(*Ptr, DeltaTime))
				{
					EaseOuts.Remove(Pair.Key);
				}
				else
				{
					// Lerp parameters
					const float Blend = UTGOR_Math::SmoothStep(Ptr->Weight, SmoothStepDegree);
					Pair.Value = FMath::Lerp(Pair.Value, Ptr->Params, Blend);
				}
			}

			// Apply filtered state to camera
			Pair.Key->Apply(this, SpringArm, Pair.Value);
		}


		// Smooth delta if framerate drops too low
		DeltaTime = FMath::Min(DeltaTime, 0.1f);

		if (UTGOR_PhysicsComponent* PhysicsComponent = GetOwner()->FindComponentByClass<UTGOR_PhysicsComponent>())
		{
			// Rotate upside
			const FVector UpVector = PhysicsComponent->ComputePhysicsUpVector();
			const float Timestep = FMath::Min(0.1f, DeltaTime) * 10.0f;
			RotateCameraUpside(UpVector, Timestep);

			// Rotate with movement base
			const FTGOR_MovementDynamic ParentSpace = PhysicsComponent->ComputeBase();
			const float Angle = ParentSpace.AngularVelocity.Size();
			if (Angle > SMALL_NUMBER)
			{
				RotateCamera(ParentSpace.AngularVelocity / Angle, Angle * DeltaTime, false);
			}
		}
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

void UTGOR_CameraComponent::UpdateContent_Implementation(UTGOR_Spawner* Spawner)
{
	if (!IsValid(SpringArm))
	{
		SpringArm = Cast<USpringArmComponent>(GetAttachParent());
	}

	Handles.Empty();
	Cameras.Empty();

	UTGOR_Creature* OwningCreature = Cast<UTGOR_Creature>(Spawner);
	if (IsValid(OwningCreature))
	{
		const TArray<UTGOR_Camera*>& List = OwningCreature->Instanced_CameraInsertions.Collection; // GetIListFromType<UTGOR_Camera>();
		for (UTGOR_Camera* Camera : List)
		{
			Cameras.Add(Camera, Camera->Initial(this, SpringArm));
		}
	}

	// EaseOuts can be added before BeginPlay, only remove unused ones
	for (auto It = EaseOuts.CreateIterator(); It; ++It)
	{
		if (!Cameras.Contains(It->Key))
		{
			It.RemoveCurrent();
		}
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void UTGOR_CameraComponent::CopyToCamera(UTGOR_CameraComponent* Camera, float Blend, bool Teleport)
{
	// Copy over modifiers
	if (IsValid(Camera))
	{
		Camera->SetViewRotation(GetComponentQuat());

		// Copy camera modifiers
		for (auto& Pair : Cameras)
		{
			// This function can be called before BeginPlay (aka before the queue is built) so we don't check compatibility yet.
			FTGOR_CameraModifier& Modifier = Camera->EaseOuts.FindOrAdd(Pair.Key);
			Modifier.Params = Pair.Value;
			Modifier.Weight = 1.0f;
			Modifier.Blend = Blend;
		}
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void UTGOR_CameraComponent::RegisterHandle(TScriptInterface<ITGOR_RegisterInterface> Register, UTGOR_CoreContent* Content, TSubclassOf<UTGOR_Camera> Type, const FVector& Params, float Blend)
{
	if (*Type)
	{
		for (auto& Pair : Cameras)
		{
			if (Pair.Key->IsA(Type))
			{
				const float NewBlend = (Blend < -SMALL_NUMBER) ? Pair.Key->DefaultBlendTime : Blend;

				// Only add to handles if content is given
				if (IsValid(Content) && Register)
				{
					// Add handle
					FTGOR_CameraHandle& Handle = Handles.FindOrAdd(Content);
					Handle.Register = Register;

					// Add modifier
					FTGOR_CameraModifier& Instance = Handle.Params.FindOrAdd(Pair.Key);
					Instance.Params = Params;
					Instance.Blend = NewBlend;
				}
				else
				{
					// Add EaseOut entry if it's non-defined
					FTGOR_CameraModifier& Instance = EaseOuts.FindOrAdd(Pair.Key);
					Instance.Params = Params;
					Instance.Weight = 1.0f;
					Instance.Blend = NewBlend;
				}
			}
		}
	}
}

void UTGOR_CameraComponent::UnregisterHandle(UTGOR_CoreContent* Content, TSubclassOf<UTGOR_Camera> Type)
{
	// Disable handle instead of removing it
	if (FTGOR_CameraHandle* Ptr = Handles.Find(Content))
	{
		for (auto Param = Ptr->Params.CreateIterator(); Param; ++Param)
		{
			if (Param->Key->IsA(Type))
			{
				// Add EaseOut entry if it's non-defined
				FTGOR_CameraModifier& Instance = EaseOuts.FindOrAdd(Param->Key);
				Instance.Weight = Param->Value.Weight;
				Instance.Blend = Param->Value.Blend;
				Instance.Params = Cameras[Param->Key];

				Param.RemoveCurrent();
			}
		}

		if (Ptr->Params.Num() == 0)
		{
			Handles.Remove(Content);
		}
	}
}

void UTGOR_CameraComponent::FastForward()
{
	EaseOuts.Empty();
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void UTGOR_CameraComponent::BuildCameraQueues(TMap<UTGOR_Camera*, FTGOR_CameraQueue>& Queues)
{
	// Poll for inactive handles to be removed, ease in modifiers
	for (auto Handle = Handles.CreateIterator(); Handle; ++Handle)
	{
		// Remove empty handles
		if (Handle->Value.Params.Num() == 0)
		{
			Handle.RemoveCurrent();
		}
		else
		{
			// Disable inactive content classes
			const TSet<UTGOR_CoreContent*> ContentContext = Handle->Value.Register->Execute_GetActiveContent(Handle->Value.Register.GetObject());
			if (!ContentContext.Contains(Handle->Key))
			{
				UnregisterHandle(Handle->Key, UTGOR_Camera::StaticClass());
			}
			else
			{
				// Blend all handles
				for (auto Param = Handle->Value.Params.CreateIterator(); Param; ++Param)
				{
					// Remove param if this component doesn't support it
					if (FVector4* Ptr = Cameras.Find(Param->Key))
					{
						// Populate layers
						FTGOR_CameraQueue& Queue = Queues.FindOrAdd(Param->Key);
						Queue.Layers.Emplace(Handle->Key);
					}
				}
			}
		}
	}
}

bool UTGOR_CameraComponent::EaseModifierIn(FTGOR_CameraModifier& Modifier, float DeltaTime)
{
	if (Modifier.Blend < SMALL_NUMBER)
	{
		Modifier.Weight = 1.0f;
		return true;
	}

	Modifier.Weight += DeltaTime / Modifier.Blend;
	if (Modifier.Weight >= 1.0f)
	{
		Modifier.Weight = 1.0f;
		return true;
	}
	return false;
}

bool UTGOR_CameraComponent::EaseModifierOut(FTGOR_CameraModifier& Modifier, float DeltaTime)
{
	if (Modifier.Blend < SMALL_NUMBER)
	{
		Modifier.Weight = 0.0f;
		return false;
	}

	Modifier.Weight -= DeltaTime / Modifier.Blend;
	if (Modifier.Weight < SMALL_NUMBER)
	{
		Modifier.Weight = 0.0f;
		return true;
	}
	return false;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void UTGOR_CameraComponent::RotateCameraUpside(const FVector& UpVector, float Amount, bool Control)
{
	// TODO: This doesn't really belong into Rotate upside
	// Adapt view rotation if not in control
	ViewRotation = FMath::Lerp(ViewRotation, GetControlRotation(), ControlThrottle).GetNormalized();

	const FQuat Rotation = ViewRotation;
	const FVector Right = Rotation.GetAxisX() ^ UpVector;
	const FVector Axis = Right ^ Rotation.GetAxisY();
	const float Angle = Axis.Size();
	if (Angle >= SMALL_NUMBER)
	{
		RotateCamera(Axis / Angle, Angle * Amount, Control);
	}
}

void UTGOR_CameraComponent::RotateCamera(const FVector& Axis, float Angle, bool Control)
{
	const float Throttle = (Control ? (1.0f - ControlThrottle) : 1.0f);
	ViewRotation = (FQuat(Axis, Angle * Throttle) * ViewRotation).GetNormalized();
}

void UTGOR_CameraComponent::SetViewRotation(const FQuat& Quat)
{
	//const FQuat ViewRotation = SpringArm->GetComponentQuat();
	ViewRotation = Quat;
}

FQuat UTGOR_CameraComponent::GetViewRotation() const
{
	return ViewRotation;
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
	if (UTGOR_PhysicsComponent* PhysicsComponent = GetOwner()->FindComponentByClass<UTGOR_PhysicsComponent>())
	{
		RotateCamera(PhysicsComponent->GetUpVector(), Amount, true);
	}
}

void UTGOR_CameraComponent::RotateCameraVertically(float Amount)
{
	const FQuat CameraRotation = GetControlRotation();
	RotateCamera(CameraRotation.GetAxisY(), Amount, true);
}

FQuat UTGOR_CameraComponent::ToLocalCameraRotation(const FTGOR_MovementPosition& Position) const
{
	const FQuat CameraRotation = GetControlRotation();
	return Position.Angular.Inverse() * CameraRotation;
}

void UTGOR_CameraComponent::FromLocalCameraRotation(const FTGOR_MovementPosition& Position, const FQuat& Quat)
{
	SetViewRotation(Position.Angular.Inverse() * Quat);
}
