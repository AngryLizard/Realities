// The Gateway of Realities: Planes of Existence.

#pragma once

#include "CoreMinimal.h"

#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "TGOR_MotionWarpingNotifyState.generated.h"


UCLASS()
class ANIMATIONSYSTEM_API UTGOR_MotionWarpingNotifyState : public UAnimNotifyState
{
	GENERATED_UCLASS_BODY()

	/** Name used to find the sync point for this modifier */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FName SyncPointName = NAME_None;
};
