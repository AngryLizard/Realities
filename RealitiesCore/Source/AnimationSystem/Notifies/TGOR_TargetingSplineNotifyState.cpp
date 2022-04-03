// Copyright Epic Games, Inc. All Rights Reserved.

#include "TGOR_TargetingSplineNotifyState.h"
#include "Components/SkeletalMeshComponent.h"
#include "Particles/ParticleSystemComponent.h"
#include "Kismet/GameplayStatics.h"

UTGOR_TargetingSplineNotifyState::UTGOR_TargetingSplineNotifyState(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void UTGOR_TargetingSplineNotifyState::NotifyTick(USkeletalMeshComponent* MeshComp, class UAnimSequenceBase* Animation, float FrameDeltaTime, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyTick(MeshComp, Animation, FrameDeltaTime, EventReference);
}
