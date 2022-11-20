// TGOR (C) // CHECKED //
#pragma once

#include "CoreMinimal.h"

#include "TGOR_MovementTask.h"
#include "TGOR_MontageMovementTask.generated.h"

////////////////////////////////////////////// DECL //////////////////////////////////////////////////////

class UTGOR_AttachedPilotTask;
class UTGOR_PilotComponent;


///////////////////////////////////////////// STRUCT /////////////////////////////////////////////////////

/** The possible states of a Root Motion Modifier */
UENUM(BlueprintType)
enum class ETGOR_RootMotionModifierState : uint8
{
	/** The modifier is waiting for the animation to hit the warping window */
	Waiting,

	/** The modifier is active and currently affecting the final root motion */
	Active,

	/** The modifier has been marked for removal. Usually because the warping window is done */
	MarkedForRemoval,

	/** The modifier will remain in the list (as long as the window is active) but will not modify the root motion */
	Disabled
};

USTRUCT()
struct MOVEMENTSYSTEM_API FTGOR_RootMotionModifier
{
	GENERATED_BODY()

public:

	/** Source of the root motion we are warping */
	UPROPERTY()
		TWeakObjectPtr<const UAnimSequenceBase> Animation = nullptr;

	/** Start time of the warping window */
	UPROPERTY()
		float StartTime = 0.f;

	/** End time of the warping window */
	UPROPERTY()
		float EndTime = 0.f;

	/** Name used to find the sync point for this modifier */
	UPROPERTY()
		FName SyncPointName = NAME_None;

	/** Previous playback time of the animation */
	UPROPERTY()
		float PreviousPosition = 0.f;

	/** Current playback time of the animation */
	UPROPERTY()
		float CurrentPosition = 0.f;

	/** Current blend weight of the animation */
	UPROPERTY()
		float Weight = 0.f;

	/** Current state */
	UPROPERTY()
		ETGOR_RootMotionModifierState State = ETGOR_RootMotionModifierState::Waiting;

	/** Sync Point used by this modifier as target for the warp. Cached during the Update */
	UPROPERTY()
		FTGOR_MovementPosition CachedSyncPoint;

	void Update(UTGOR_MontageMovementTask* Task);
	FTransform ProcessRootMotion(UTGOR_MontageMovementTask* Task, UTGOR_AnimationComponent* Component, const FTransform& InRootMotion, float DeltaSeconds) const;

protected:

	FQuat WarpRotation(const FTGOR_MovementPosition& Position, const FTransform& RootMotionDelta, const FTransform& RootMotionTotal, float DeltaSeconds) const;
};


/**
* Montage Movement includes functionality to handle motionwarping (other movements can also handle root motion but not motion warping)
*/
UCLASS(Blueprintable)
class MOVEMENTSYSTEM_API UTGOR_MontageMovementTask : public UTGOR_MovementTask
{
	GENERATED_BODY()

		friend struct FTGOR_RootMotionModifier;

public:

	////////////////////////////////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////// ENGINE ////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////////////////////////////

	UTGOR_MontageMovementTask();

	virtual bool Initialise() override;
	virtual bool Invariant(const FTGOR_MovementSpace& Space, const FTGOR_MovementExternal& External) const override;
	virtual void Update(const FTGOR_MovementSpace& Space, const FTGOR_MovementExternal& External, const FTGOR_MovementTick& Tick, FTGOR_MovementOutput& Output) override;

	virtual void PrepareStart() override;
	virtual void Interrupt() override;

	////////////////////////////////////////////////////////////////////////////////////////////////////
protected:

	UPROPERTY(Transient)
		TWeakObjectPtr<UTGOR_PilotComponent> RootComponent;

	UPROPERTY(Transient)
		TWeakObjectPtr<UTGOR_AttachedPilotTask> AttachedTask;


	////////////////////////////////////////////////////////////////////////////////////////////////////
protected:

	/** Returns the list of root motion modifiers */
	FORCEINLINE const TArray<TSharedPtr<FTGOR_RootMotionModifier>>& GetRootMotionModifiers() const { return RootMotionModifiers; }

	/** Find the SyncPoint associated with a specified name */
	FORCEINLINE const FTGOR_MovementPosition* FindSyncPoint(const FName& SyncPointName) const { return SyncPoints.Find(SyncPointName); }

	/** Adds or update the sync point associated with a specified name */
	UFUNCTION(BlueprintCallable, Category = "Motion Warping")
		void AddOrUpdateSyncPoint(FName Name, const FTGOR_MovementPosition& SyncPoint);

	/** Removes sync point associated with the specified key  */
	UFUNCTION(BlueprintCallable, Category = "Motion Warping")
		int32 RemoveSyncPoint(FName Name);

	/** Check if we contain a RootMotionModifier for the supplied animation and time range */
	bool ContainsModifier(const UAnimSequenceBase* Animation, float StartTime, float EndTime) const;

	/** Add a new modifier */
	void AddRootMotionModifier(TSharedPtr<FTGOR_RootMotionModifier> Modifier);

	/** Mark all the modifiers as Disable */
	void DisableAllRootMotionModifiers();

	////////////////////////////////////////////////////////////////////////////////////////////////////
protected:

	/** List of root motion modifiers */
	TArray<TSharedPtr<FTGOR_RootMotionModifier>> RootMotionModifiers;

	UPROPERTY(Transient)
		TMap<FName, FTGOR_MovementPosition> SyncPoints;

	void Update();
	FTransform ProcessRootMotionPreConvertToWorld(const FTransform& InRootMotion, UTGOR_AnimationComponent* Component, float DeltaSeconds);
	FTransform ProcessRootMotionPostConvertToWorld(const FTransform& InRootMotion, UTGOR_AnimationComponent* Component, float DeltaSeconds);
	const FAnimMontageInstance* GetMontageInstance() const;

	virtual FTransform ConvertLocalRootMotionToWorld(const FTransform& LocalRootMotionTransform, UTGOR_AnimationComponent* Component, float DeltaSeconds) override;
};
