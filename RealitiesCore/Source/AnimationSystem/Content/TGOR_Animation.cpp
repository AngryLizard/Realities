// The Gateway of Realities: Planes of Existence.


#include "TGOR_Animation.h"

#include "AnimationSystem/Instances/TGOR_AnimInstance.h"
#include "AnimationSystem/Instances/TGOR_SubAnimInstance.h"
#include "AnimationSystem/Interfaces/TGOR_AnimationInterface.h"
#include "AnimationSystem/Components/TGOR_AnimationComponent.h"
#include "AnimationSystem/Content/TGOR_Performance.h"

#if WITH_EDITOR
#include "Kismet2/BlueprintEditorUtils.h"
#endif // WITH_EDITOR

UTGOR_Animation::UTGOR_Animation()
	: Super(), BlendTime(0.5f), PhysicsStrength(0.0f)
{
	InstanceClass = UTGOR_SubAnimInstance::StaticClass();
}

bool UTGOR_Animation::Validate_Implementation()
{
	if (!*InstanceClass)
	{
		ERRET("No animation instance defined", Error, false);
	}

	return Super::Validate_Implementation();
}

UTGOR_AnimInstance* UTGOR_Animation::GetAnimationInstance(TScriptInterface<ITGOR_AnimationInterface> Owner)
{
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
				if (IsValid(SubAnimInstance) && SubAnimInstance->AnimatedTask.IsValid())
				{
					return SubAnimInstance;
				}
			}
		}
	}
	return nullptr;
}


#if WITH_EDITOR

void UTGOR_Animation::PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent)
{
	FProperty* ChangedProperty = PropertyChangedEvent.Property;

	if (ChangedProperty)
	{
		if (ChangedProperty->GetFName() == GET_MEMBER_NAME_CHECKED(UTGOR_Animation, InstanceClass))
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