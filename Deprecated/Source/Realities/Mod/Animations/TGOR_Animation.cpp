// The Gateway of Realities: Planes of Existence.


#include "TGOR_Animation.h"

#include "Realities/Animation/TGOR_AnimInstance.h"
#include "Realities/Animation/TGOR_SubAnimInstance.h"

#if WITH_EDITOR
#include "Kismet2/BlueprintEditorUtils.h"
#endif // WITH_EDITOR

UTGOR_Animation::UTGOR_Animation()
	: Super(), BlendTime(0.5f), PhysicsStrength(0.0f)
{
}

UTGOR_SubAnimInstance* UTGOR_Animation::GetAnimationInstance(USkeletalMeshComponent* SkeletonComponent, FName Slot)
{
	if (IsValid(SkeletonComponent))
	{
		UTGOR_AnimInstance* AnimInstance = Cast<UTGOR_AnimInstance>(SkeletonComponent->AnimScriptInstance);
		if (IsValid(AnimInstance))
		{
			UTGOR_SubAnimInstance* SubAnimInstance = AnimInstance->GetSubAnimInstance(Slot);
			if (IsValid(SubAnimInstance) && IsValid(SubAnimInstance->Animation) && SubAnimInstance->Animation->GetClass() == GetClass())
			{
				return(SubAnimInstance);
			}
		}
	}

	return(nullptr);
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
}

#endif // WITH_EDITOR