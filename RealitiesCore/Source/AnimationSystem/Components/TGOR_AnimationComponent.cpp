// The Gateway of Realities: Planes of Existence.

#include "TGOR_AnimationComponent.h"

#include "CoreSystem/TGOR_Singleton.h"
#include "AnimationSystem/Components/TGOR_AttachComponent.h"
#include "DimensionSystem/Content/TGOR_Primitive.h"
#include "AnimationSystem/Instances/TGOR_AnimInstance.h"
#include "AnimationSystem/Instances/TGOR_SubAnimInstance.h"
#include "AnimationSystem/Content/TGOR_Performance.h"
#include "AnimationSystem/Content/TGOR_Animation.h"
#include "AnimationSystem/Content/TGOR_Archetype.h"
#include "AnimationSystem/Content/TGOR_RigParam.h"

#include "CoreSystem/Storage/TGOR_Package.h"
#include "Net/UnrealNetwork.h"


UTGOR_AnimationComponent::UTGOR_AnimationComponent()
	: Super()
{
	SetIsReplicatedByDefault(true);

	SpawnArchetype = UTGOR_Archetype::StaticClass();
}

void UTGOR_AnimationComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void UTGOR_AnimationComponent::FinalizeBoneTransform()
{
	Super::FinalizeBoneTransform();

}

void UTGOR_AnimationComponent::GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION(UTGOR_AnimationComponent, AnimationSetup, COND_None);
}

void UTGOR_AnimationComponent::UpdateContent_Implementation(FTGOR_SpawnerDependencies& Dependencies)
{
	ITGOR_SpawnerInterface::UpdateContent_Implementation(Dependencies);

	FTGOR_AnimationInstance Setup;
	Setup.Archetype = Dependencies.Spawner->GetMFromType<UTGOR_Archetype>(SpawnArchetype);
	ApplyAnimationSetup(Setup);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

bool UTGOR_AnimationComponent::ApplyAnimationSetup(FTGOR_AnimationInstance Setup)
{
	// TODO: Keep running animations of same type
	PerformanceSlots.Empty();
	AnimInstance = nullptr;

	// Run setup
	AnimationSetup.Archetype = Setup.Archetype;
	AnimationSetup.Params.Empty();

	// Initialise animBP
	if (IsValid(AnimationSetup.Archetype))
	{
		// Keep previous animBP if not defined
		if (*AnimationSetup.Archetype->InstanceClass)
		{
			SetAnimInstanceClass(AnimationSetup.Archetype->InstanceClass);
		}

		for (UTGOR_Performance* Performance : AnimationSetup.Archetype->Instanced_PerformanceInsertions.Collection)
		{
			PerformanceSlots.Emplace(Performance, nullptr);
		}

		for (UTGOR_RigParam* Param : AnimationSetup.Archetype->Instanced_RigParamInsertions.Collection)
		{
			AnimationSetup.Params.Emplace(Param, Param->DefaultValue);
		}

		// Adapt supported slot params
		for (const auto& Pair : Setup.Params)
		{
			FTGOR_Normal* Ptr = AnimationSetup.Params.Find(Pair.Key);
			if (Ptr)
			{
				(*Ptr) = FMath::Clamp(Pair.Value.Value, 0.0f, 1.0f);
			}
		}

		// This is needed for in-editor previews
		FEditorScriptExecutionGuard ScriptGuard;

		// Initialise animinstance
		AnimInstance = Cast<UTGOR_AnimInstance>(GetAnimInstance());
		if (AnimInstance.IsValid())
		{
			AnimInstance->ClearQueues();

			// Initialise archetype
			AnimInstance->AssignArchetype(AnimationSetup.Archetype, AnimationSetup.Params);
		}


	}

	return true;
}

bool UTGOR_AnimationComponent::SwitchAnimation(TSubclassOf<UTGOR_Performance> PerformanceType, TSubclassOf<UTGOR_Animation> AnimationType)
{
	if (AnimInstance.IsValid())
	{
		for (const auto& Pair : PerformanceSlots)
		{
			if (Pair.Key->IsA(PerformanceType))
			{
				if (IsValid(Pair.Value))
				{
					if (*AnimationType)
					{
						// There is already an animation running but we chose a different one
						if (!Pair.Value->IsA(AnimationType))
						{
							AnimInstance->AssignAnimationInstance(Pair.Key, AnimationSetup.Archetype->Instanced_AnimationInsertions.GetOfType(AnimationType));
							return true;
						}
					}
					else
					{
						// There is already an animation running and we want to cancel it
						AnimInstance->AssignAnimationInstance(Pair.Key, nullptr);
						return true;
					}
				}
				else if (*AnimationType)
				{
					// There is no animation running but we want to run one
					AnimInstance->AssignAnimationInstance(Pair.Key, AnimationSetup.Archetype->Instanced_AnimationInsertions.GetOfType(AnimationType));
					return true;
				}
			}
		}
	}
	return false;
}

UTGOR_Animation* UTGOR_AnimationComponent::GetAnimation(TSubclassOf<UTGOR_Performance> PerformanceType) const
{
	for (const auto& Pair : PerformanceSlots)
	{
		if (Pair.Key->IsA(PerformanceType))
		{
			return Pair.Value;
		}
	}
	return nullptr;
}

void UTGOR_AnimationComponent::OnReplicateAnimationSetup()
{
	ApplyAnimationSetup(AnimationSetup);
}
