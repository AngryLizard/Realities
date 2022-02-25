// The Gateway of Realities: Planes of Existence.


#include "TGOR_Animator.h"

#include "Realities/AnimationSystem/Instances/TGOR_AnimInstance.h"
#include "Realities/AnimationSystem/Instances/TGOR_SubAnimInstance.h"
#include "Realities/AnimationSystem/Interfaces/TGOR_AnimationInterface.h"
#include "Realities/AnimationSystem/Components/TGOR_AnimationComponent.h"
#include "Realities/AnimationSystem/Content/TGOR_Performance.h"

#if WITH_EDITOR
#include "Kismet2/BlueprintEditorUtils.h"
#endif // WITH_EDITOR

UTGOR_Animator::UTGOR_Animator()
	: Super()
{
}

bool UTGOR_Animator::Validate()
{
	if (!*GetInstanceClass())
	{
		ERRET("No animation instance defined", Error, false);
	}

	return Super::Validate();
}

TSubclassOf<UTGOR_AnimInstance> UTGOR_Animator::GetInstanceClass() const
{
	return UTGOR_AnimInstance::StaticClass();
}

UTGOR_AnimInstance* UTGOR_Animator::GetAnimationInstance(TScriptInterface<ITGOR_AnimationInterface> Owner)
{
	return nullptr;
}
