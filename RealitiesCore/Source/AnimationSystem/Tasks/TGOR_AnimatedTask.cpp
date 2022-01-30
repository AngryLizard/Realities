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
	: Super(), BlendTime(0.5f), AnimRootMotionTranslationScale(0.01f)
{
	InstanceClass = UTGOR_SubAnimInstance::StaticClass();
}

////////////////////////////////////////////////////////////////////////////////////////////////////

UTGOR_AnimInstance* UTGOR_AnimatedTask::GetAnimationInstance()
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

void UTGOR_AnimatedTask::PlayAnimation()
{
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

FTGOR_MovementPosition UTGOR_AnimatedTask::TickAnimationRootMotion(float DeltaTime)
{
	FTGOR_MovementPosition Position;
	if (FMath::IsNearlyZero(DeltaTime))
	{
		return Position;
	}

	TScriptInterface<ITGOR_AnimationInterface> Owner = GetAnimationOwner();
	if (!Owner)
	{
		return Position;
	}

	UTGOR_AnimationComponent* Component = Owner->GetAnimationComponent();
	if (!IsValid(Component))
	{
		return Position;
	}

	// Grab root motion now that we have ticked the pose
	if (Component->IsPlayingRootMotion())
	{
		FRootMotionMovementParams RootMotion = Component->ConsumeRootMotion();
		if (RootMotion.bHasRootMotion)
		{
			RootMotion.ScaleRootMotionTranslation(AnimRootMotionTranslationScale);

			const FTransform Transform = Component->ConvertLocalRootMotionToWorld(RootMotion.GetRootMotionTransform());
			Position.Linear = Transform.GetTranslation();
			Position.Angular = Transform.GetRotation();
		}
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