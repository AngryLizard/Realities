// TGOR (C) // CHECKED //
#pragma once

#include "CoreMinimal.h"

#include "DimensionSystem/TGOR_MobilityInstance.h"

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
		float BlendTime = 0.5f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "!TGOR Animation")
		float AnimRootMotionTranslationScale = 1.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "!TGOR Animation")
		bool bTransformRootMotionToLinearVelocity = false;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "!TGOR Animation")
		bool bTransformRootMotionToAngularVelocity = false;

	// Whether to consume only subanim instance or all root motion
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "!TGOR Animation")
		bool bConsumeAllRootMotion = false;

	// Whether to project local root motion to XY-plane
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "!TGOR Animation")
		bool bProjectZAxis = false;

	////////////////////////////////////////////////////////////////////////////////////////////////////

	UFUNCTION(BlueprintCallable, Category = "!TGOR Animation", Meta = (Keywords = "C++"))
		UTGOR_SubAnimInstance* GetAnimationInstance();

	UFUNCTION(BlueprintCallable, Category = "!TGOR Animation", Meta = (Keywords = "C++"))
		void StopTaskMontage(const UAnimMontage* Montage = NULL);

#if WITH_EDITOR
	virtual void PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent) override;
#endif // WITH_EDITOR


	/** Tick animation and extract root motion */
	virtual FTGOR_MovementPosition TickAnimationRootMotion(FTGOR_MovementSpace& Space, float DeltaTime);

	////////////////////////////////////////////////////////////////////////////////////////////////////
protected:

	/** Play animation on this task. */
		void PlayAnimation();

	/** Reset animation on this task. */
		void ResetAnimation();

	/** Extract root motion from current animation over a given timeframe */
		void ConsumeRootMotion(float DeltaTime);

	/** Overrideable root motion conversion */
		virtual FTransform ConvertLocalRootMotionToWorld(const FTransform& LocalRootMotionTransform, UTGOR_AnimationComponent* Component, float DeltaSeconds);

	////////////////////////////////////////////////////////////////////////////////////////////////////
private:

	/** Rootmotion left to consume */
	FTGOR_MovementDynamic RootMotionDelta;

	/** Get animation owner to run with */
	virtual TScriptInterface<ITGOR_AnimationInterface> GetAnimationOwner() const PURE_VIRTUAL(UTGOR_AnimatedTask::GetAnimationOwner, return nullptr; );
};
