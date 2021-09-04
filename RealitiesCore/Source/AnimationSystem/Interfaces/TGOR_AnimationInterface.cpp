// The Gateway of Realities: Planes of Existence.


#include "TGOR_AnimationInterface.h"
#include "AnimationSystem/Content/TGOR_Animation.h"

ITGOR_AnimationInterface::ITGOR_AnimationInterface()
{
}

TSubclassOf<UTGOR_Performance> ITGOR_AnimationInterface::GetPerformanceType() const
{
	return TSubclassOf<UTGOR_Performance>();
}

UTGOR_AnimationComponent* ITGOR_AnimationInterface::GetAnimationComponent() const
{
	return nullptr;
}

UTGOR_Animation* ITGOR_AnimationInterface::GetAnimation() const
{
	UTGOR_AnimationComponent* Component = GetAnimationComponent();
	TSubclassOf<UTGOR_Performance> Performance = GetPerformanceType();
	if (IsValid(Component) && *Performance)
	{
		return Component->GetAnimation(Performance);
	}
	return nullptr;
}

UTGOR_Animation* ITGOR_AnimationInterface::GetAnimationChecked(TSubclassOf<UTGOR_Animation> Type, ETGOR_ValidEnumeration& Branches)
{
	Branches = ETGOR_ValidEnumeration::Invalid;
	UTGOR_Animation* Animation = GetAnimation();
	if (IsValid(Animation) && Animation->IsA(Type))
	{
		Branches = ETGOR_ValidEnumeration::Valid;
		return Animation;
	}
	return nullptr;
}