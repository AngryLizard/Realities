// The Gateway of Realities: Planes of Existence.

#pragma once

#include "CoreMinimal.h"

#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "TGOR_DamageNotifyState.generated.h"

class UTGOR_DamageComponent;

UCLASS()
class ACTIONSYSTEM_API UTGOR_DamageNotifyState : public UAnimNotifyState
{
	GENERATED_UCLASS_BODY()

	// The socket within our mesh component to attach to when we spawn the particle component
	UPROPERTY(EditAnywhere, Category = ActionSystem)
		FName SocketName = "";

	// Damage radius for this component
	UPROPERTY(EditAnywhere, Category = ActionSystem)
		float DamageRadius = 100.0f;

	// Damage strength for this component
	UPROPERTY(EditAnywhere, Category = ActionSystem)
		float DamageStrength = 1.0f;

	// Offset from the socket / bone location
	UPROPERTY(EditAnywhere, Category = ActionSystem)
		FVector LocationOffset;

	// Offset from the socket / bone rotation
	UPROPERTY(EditAnywhere, Category = ActionSystem)
		FRotator RotationOffset;

#if WITH_EDITORONLY_DATA
	// The following arrays are used to handle property changes during a state. Because we can't
	// store any stateful data here we can't know which emitter is ours. The best metric we have
	// is an emitter on our Mesh Component with the same template and socket name we have defined.
	// Because these can change at any time we need to track previous versions when we are in an
	// editor build. Refactor when stateful data is possible, tracking our component instead.

	UPROPERTY(transient)
		TArray<FName> PreviousSocketNames;

#endif

#if WITH_EDITOR
	virtual void PreEditChange(FProperty* PropertyAboutToChange) override;
#endif

	virtual void NotifyBegin(class USkeletalMeshComponent* MeshComp, class UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference) override;
	virtual void NotifyTick(class USkeletalMeshComponent* MeshComp, class UAnimSequenceBase* Animation, float FrameDeltaTime, const FAnimNotifyEventReference& EventReference) override;
	virtual void NotifyEnd(class USkeletalMeshComponent* MeshComp, class UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) override;

protected:

	UTGOR_DamageComponent* SpawnDamageAttached(USkeletalMeshComponent* MeshComp, float TotalDuration);
	bool ValidateParameters(USkeletalMeshComponent* MeshComp);
};
