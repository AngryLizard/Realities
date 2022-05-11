// The Gateway of Realities: Planes of Existence.

#include "TGOR_DamageNotifyState.h"
#include "Animation/AnimTypes.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "ActionSystem/Components/TGOR_DamageComponent.h"

#include "AnimationUtils.h"

UTGOR_DamageNotifyState::UTGOR_DamageNotifyState(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void UTGOR_DamageNotifyState::NotifyBegin(USkeletalMeshComponent* MeshComp, class UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference)
{
	// Only spawn if we've got valid params
	if (ValidateParameters(MeshComp))
	{
		UTGOR_DamageComponent* NewComponent = SpawnDamageAttached(MeshComp, TotalDuration);
	}
	Super::NotifyBegin(MeshComp, Animation, TotalDuration, EventReference);
}

void UTGOR_DamageNotifyState::NotifyTick(USkeletalMeshComponent* MeshComp, class UAnimSequenceBase* Animation, float FrameDeltaTime, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyTick(MeshComp, Animation, FrameDeltaTime, EventReference);
}

void UTGOR_DamageNotifyState::NotifyEnd(USkeletalMeshComponent* MeshComp, class UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	TArray<USceneComponent*> Children;
	MeshComp->GetChildrenComponents(false, Children);

	for (USceneComponent* Component : Children)
	{
		if (UTGOR_DamageComponent* DamageComponent = Cast<UTGOR_DamageComponent>(Component))
		{
			bool bSocketMatch = (DamageComponent->GetAttachSocketName() == SocketName);

#if WITH_EDITORONLY_DATA
			// In editor someone might have changed our parameters while we're ticking; so check 
			// previous known parameters too.
			bSocketMatch |= PreviousSocketNames.Contains(DamageComponent->GetAttachSocketName());
#endif

			if (bSocketMatch)
			{
				// Destroy the component.
				DamageComponent->DestroyComponent();

#if WITH_EDITORONLY_DATA
				// No longer need to track previous values as we've found our component
				// and removed it.
				PreviousSocketNames.Empty();
#endif
				// Removed a component, no need to continue
				break;
			}
		}
	}

	Super::NotifyEnd(MeshComp, Animation, EventReference);
}

UTGOR_DamageComponent* UTGOR_DamageNotifyState::SpawnDamageAttached(USkeletalMeshComponent* MeshComp, float TotalDuration)
{
	if (MeshComp == nullptr)
	{
		UE_LOG(LogScript, Warning, TEXT("UTGOR_DamageNotifyState::SpawnDamageAttached: NULL MeshComp specified!"));
	}
	else if (UWorld* const World = MeshComp->GetWorld())
	{
		UTGOR_DamageComponent* DamageComponent = NewObject<UTGOR_DamageComponent>(MeshComp->GetOwner());
		if (DamageComponent != nullptr)
		{
			DamageComponent->SetupAttachment(MeshComp, SocketName);
			DamageComponent->SetRelativeLocation_Direct(LocationOffset);
			DamageComponent->SetRelativeRotation_Direct(RotationOffset);
			DamageComponent->RegisterComponentWithWorld(World);

			DamageComponent->InitialiseDamage(MeshComp, DamageRadius, TotalDuration, DamageStrength);

			// Notify the texture streamer so that PSC gets managed as a dynamic component.
			IStreamingManager::Get().NotifyPrimitiveUpdated(DamageComponent);
		}
	}
	return nullptr;
}

bool UTGOR_DamageNotifyState::ValidateParameters(USkeletalMeshComponent* MeshComp)
{
	bool bValid = true;

	if (!MeshComp->DoesSocketExist(SocketName) && MeshComp->GetBoneIndex(SocketName) == INDEX_NONE)
	{
		bValid = false;
	}

	return bValid;
}

#if WITH_EDITOR
void UTGOR_DamageNotifyState::PreEditChange(FProperty* PropertyAboutToChange)
{
	Super::PreEditChange(PropertyAboutToChange);

	if (PropertyAboutToChange && PropertyAboutToChange->GetName() == GET_MEMBER_NAME_STRING_CHECKED(UTGOR_DamageNotifyState, SocketName) && SocketName != FName(TEXT("None")))
	{
		PreviousSocketNames.Add(SocketName);
	}
}
#endif
