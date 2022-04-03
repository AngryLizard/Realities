// The Gateway of Realities: Planes of Existence.

#pragma once

#include "CoreMinimal.h"

#include "Animation/AnimNotifies/AnimNotifyState_TimedParticleEffect.h"
#include "TGOR_TargetingSplineNotifyState.generated.h"

class UParticleSystem;
class USkeletalMeshComponent;

UCLASS(Blueprintable, meta = (DisplayName = "Targeting Particle Effect"))
class ANIMATIONSYSTEM_API UTGOR_TargetingSplineNotifyState : public UAnimNotifyState_TimedParticleEffect
{
	GENERATED_UCLASS_BODY()

	virtual void NotifyTick(class USkeletalMeshComponent* MeshComp, class UAnimSequenceBase* Animation, float FrameDeltaTime, const FAnimNotifyEventReference& EventReference) override;
};
