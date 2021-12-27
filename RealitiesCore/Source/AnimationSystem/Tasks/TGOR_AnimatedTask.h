// TGOR (C) // CHECKED //
#pragma once

#include "CoreMinimal.h"

#include "CoreSystem/Tasks/TGOR_Task.h"
#include "TGOR_AnimatedTask.generated.h"

class UTGOR_AnimationComponent;
class ITGOR_AnimationInterface;
class UTGOR_SubAnimInstance;
class UTGOR_Performance;

/**
* UTGOR_AnimatedTask
*/
UCLASS()
class ANIMATIONSYSTEM_API UTGOR_AnimatedTask : public UTGOR_Task
{
	GENERATED_BODY()

public:
	
	////////////////////////////////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////// ENGINE ////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////////////////////////////

	UTGOR_AnimatedTask();

	////////////////////////////////////////////////////////////////////////////////////////////////////

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "!TGOR Animation")
		TSubclassOf<UTGOR_SubAnimInstance> InstanceClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "!TGOR Animation")
		float BlendTime;

	////////////////////////////////////////////////////////////////////////////////////////////////////

	UFUNCTION(BlueprintCallable, Category = "!TGOR Animation", Meta = (Keywords = "C++"))
		UTGOR_AnimInstance* GetAnimationInstance();


#if WITH_EDITOR
	virtual void PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent) override;
#endif // WITH_EDITOR

	////////////////////////////////////////////////////////////////////////////////////////////////////
protected:

	/** Play animation on this task. */
		void PlayAnimation();

	/** Reset animation on this task. */
		void ResetAnimation();

	////////////////////////////////////////////////////////////////////////////////////////////////////
private:

	/** Get animation owner to run with */
	virtual TScriptInterface<ITGOR_AnimationInterface> GetAnimationOwner() const PURE_VIRTUAL(UTGOR_AnimatedTask::GetAnimationOwner, return nullptr; );
};