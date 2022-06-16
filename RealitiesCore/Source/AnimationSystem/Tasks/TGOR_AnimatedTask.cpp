// The Gateway of Realities: Planes of Existence.

#include "TGOR_AnimatedTask.h"
#include "AnimationSystem/Instances/TGOR_SubAnimInstance.h"
#include "AnimationSystem/Interfaces/TGOR_AnimationInterface.h"
#include "AnimationSystem/Components/TGOR_AnimationComponent.h"
#include "AnimationSystem/Content/TGOR_Performance.h"

#if WITH_EDITOR
#include "Kismet2/BlueprintEditorUtils.h"
#endif // WITH_EDITOR


UTGOR_AnimatedTask::UTGOR_AnimatedTask()
	: Super()
{
}

////////////////////////////////////////////////////////////////////////////////////////////////////

UTGOR_SubAnimInstance* UTGOR_AnimatedTask::GetAnimationInstance()
{
	TScriptInterface<ITGOR_AnimationInterface> Owner = GetAnimationOwner();
	if (Owner)
	{
		UTGOR_AnimationComponent* Component = Owner->GetAnimationComponent();
		TSubclassOf<UTGOR_Performance> Slot = Owner->GetPerformanceType();
		if (IsValid(Component) && *Slot)
		{
			UTGOR_Performance* Performance = nullptr;
			for (UTGOR_Performance* PerformanceSlot : Component->PerformanceSlots)
			{
				if (PerformanceSlot && PerformanceSlot->IsA(Slot))
				{
					Performance = PerformanceSlot;
				}
			}

			if (IsValid(Performance) && Component->AnimInstance.IsValid())
			{
				UTGOR_SubAnimInstance* SubAnimInstance = Component->AnimInstance->GetSubAnimInstance(Performance);
				if (IsValid(SubAnimInstance) && SubAnimInstance->AnimatedTask.IsValid() && SubAnimInstance->AnimatedTask == this)
				{
					return SubAnimInstance;
				}
			}
		}
	}
	return nullptr;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void UTGOR_AnimatedTask::StopTaskMontage(const UAnimMontage* Montage)
{
	UTGOR_SubAnimInstance* SubAnimInstance = GetAnimationInstance();
	if (SubAnimInstance)
	{
		SubAnimInstance->Montage_Stop(BlendTime, Montage);
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void UTGOR_AnimatedTask::PlayAnimation()
{
	if (!*InstanceClass)
	{
		return;
	}

	TScriptInterface<ITGOR_AnimationInterface> Owner = GetAnimationOwner();
	if (Owner)
	{
		UTGOR_AnimationComponent* Component = Owner->GetAnimationComponent();
		TSubclassOf<UTGOR_Performance> Slot = Owner->GetPerformanceType();
		if (IsValid(Component) && *Slot)
		{
			Component->SwitchAnimation(Slot, this);
		}
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void UTGOR_AnimatedTask::ResetAnimation()
{
	if (!*InstanceClass)
	{
		return;
	}

	TScriptInterface<ITGOR_AnimationInterface> Owner = GetAnimationOwner();
	if (Owner)
	{
		UTGOR_AnimationComponent* Component = Owner->GetAnimationComponent();
		TSubclassOf<UTGOR_Performance> Slot = Owner->GetPerformanceType();
		if (IsValid(Component) && *Slot)
		{
			Component->SwitchAnimation(Slot, nullptr);
		}
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void UTGOR_AnimatedTask::ConsumeRootMotion(float DeltaTime)
{
	if (FMath::IsNearlyZero(DeltaTime))
	{
		return;
	}

	TScriptInterface<ITGOR_AnimationInterface> Owner = GetAnimationOwner();
	if (!Owner)
	{
		return;
	}

	UTGOR_AnimationComponent* Component = Owner->GetAnimationComponent();
	if (!IsValid(Component))
	{
		return;
	}

	// Grab root motion
	if (Component->IsPlayingRootMotion())
	{
		// Extract root motion from relevant animation instance
		FRootMotionMovementParams RootMotion;
		if (bConsumeAllRootMotion)
		{
			RootMotion = Component->ConsumeRootMotion();
		}
		else
		{
			UTGOR_SubAnimInstance* SubAnimInstance = GetAnimationInstance();
			if (SubAnimInstance)
			{
				float InterpAlpha;
				if (Component->bExternalTickRateControlled)
				{
					InterpAlpha = Component->ExternalInterpolationAlpha;
				}
				else if(Component->ShouldUseUpdateRateOptimizations() && Component->AnimUpdateRateParams->OptimizeMode == FAnimUpdateRateParameters::EOptimizeMode::LookAheadMode)
				{
					// Same as Component->AnimUpdateRateParams->GetRootMotionInterp but can actually be linked (thanks EPIC for not adding dll linkage to the struct, MEAN! >:)
					InterpAlpha = FMath::Clamp(Component->AnimUpdateRateParams->ThisTickDelta / (Component->AnimUpdateRateParams->TickedPoseOffestTime + Component->AnimUpdateRateParams->ThisTickDelta), 0.f, 1.f);
				}
				else
				{
					InterpAlpha = 1.f;
				}

				RootMotion = SubAnimInstance->ConsumeExtractedRootMotion(InterpAlpha);
			}
		}

		if (RootMotion.bHasRootMotion)
		{
			RootMotion.ScaleRootMotionTranslation(AnimRootMotionTranslationScale);

			FTransform LocalTransform = RootMotion.GetRootMotionTransform();
			if (bProjectZAxis)
			{
				LocalTransform.SetTranslation(LocalTransform.GetTranslation() * FVector(1, 1, 0));
			}
			const FTransform Transform = ConvertLocalRootMotionToWorld(LocalTransform, Component, DeltaTime);
			RootMotionDelta.Linear = Transform.GetTranslation();
			RootMotionDelta.Angular = Transform.GetRotation();
			RootMotionDelta.LinearVelocity = RootMotionDelta.Linear / DeltaTime;
			RootMotionDelta.AngularVelocity = RootMotionDelta.Angular.GetRotationAxis() * (RootMotionDelta.Angular.GetAngle() / DeltaTime);
		}
	}
}

FTGOR_MovementPosition UTGOR_AnimatedTask::TickAnimationRootMotion(FTGOR_MovementSpace& Space, float DeltaTime)
{
	FTGOR_MovementPosition Position;
	if (bTransformRootMotionToLinearVelocity)
	{
		const FVector VelocityDelta = RootMotionDelta.LinearVelocity - Space.RelativeLinearVelocity;
		Space.RelativeLinearVelocity += VelocityDelta;
		Space.LinearVelocity += VelocityDelta;
		Position.Linear = FVector::ZeroVector;
	}
	else
	{
		Position.Linear = RootMotionDelta.Linear;
		RootMotionDelta.Linear = FVector::ZeroVector;
	}

	if(bTransformRootMotionToAngularVelocity)
	{
		const FVector VelocityDelta = RootMotionDelta.AngularVelocity - Space.RelativeAngularVelocity;
		Space.RelativeAngularVelocity += VelocityDelta;
		Space.AngularVelocity += VelocityDelta;
		Position.Angular = FQuat::Identity;
	}
	else
	{
		Position.Angular = RootMotionDelta.Angular;
		RootMotionDelta.Angular = FQuat::Identity;
	}
	return Position;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
#if WITH_EDITOR

void UTGOR_AnimatedTask::PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent)
{
	FProperty* ChangedProperty = PropertyChangedEvent.Property;

	if (ChangedProperty)
	{
		if (ChangedProperty->GetFName() == GET_MEMBER_NAME_CHECKED(UTGOR_AnimatedTask, InstanceClass))
		{
			UBlueprint* Blueprint = UBlueprint::GetBlueprintFromClass(GetClass());
			if (Blueprint)
			{
				FBlueprintEditorUtils::RefreshAllNodes(Blueprint);
			}
		}
	}
	Super::PostEditChangeProperty(PropertyChangedEvent);
}

#endif // WITH_EDITOR

////////////////////////////////////////////////////////////////////////////////////////////////////

FTransform UTGOR_AnimatedTask::ConvertLocalRootMotionToWorld(const FTransform& LocalRootMotionTransform, UTGOR_AnimationComponent* Component, float DeltaSeconds)
{
	return Component->ConvertLocalRootMotionToWorld(LocalRootMotionTransform);
}
