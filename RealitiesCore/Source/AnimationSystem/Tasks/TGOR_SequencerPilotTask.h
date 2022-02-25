// TGOR (C) // CHECKED //
#pragma once

#include "CoreMinimal.h"

#include "DimensionSystem/Tasks/TGOR_PilotTask.h"
#include "TGOR_SequencerPilotTask.generated.h"

class UTGOR_SequencerComponent;

/**
* 
*
*/
USTRUCT(BlueprintType)
struct ANIMATIONSYSTEM_API FTGOR_SequencerParenting
{
	GENERATED_USTRUCT_BODY();
	bool operator==(const FTGOR_SequencerParenting& Other) const;
	bool operator!=(const FTGOR_SequencerParenting& Other) const;

	/** Parent component */
	UPROPERTY()
		TWeakObjectPtr<UTGOR_SequencerComponent> Component;

	/** Pose component */
	UPROPERTY()
		TWeakObjectPtr<USkeletalMeshComponent> Pose;
};


/**
* Uses owning actor's transform. Useful for e.g. sequences where the owning actor's location is controlled by an external system.
*/
UCLASS(Blueprintable)
class ANIMATIONSYSTEM_API UTGOR_SequencerPilotTask : public UTGOR_PilotTask
{
	GENERATED_BODY()

public:

	////////////////////////////////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////// ENGINE ////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////////////////////////////

	UTGOR_SequencerPilotTask();
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	////////////////////////////////////////////////////////////////////////////////////////////////////
public:

	virtual UTGOR_MobilityComponent* GetParent() const override;
	virtual void Unparent() override;

	virtual void Register() override;
	virtual void Unregister() override;

	virtual FTGOR_MovementPosition ComputePosition() const override;
	virtual FTGOR_MovementSpace ComputeSpace() const override;

	////////////////////////////////////////////////////////////////////////////////////////////////////
public:

	/** Parent this task to a given pose. Does not preserve current world transform. */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Movement", Meta = (Keywords = "C++"))
		void Parent(UTGOR_SequencerComponent* Parent, USkeletalMeshComponent* Pose);

	/** Whether this task can parent to a given pose */
	UFUNCTION(BlueprintPure, Category = "!TGOR Movement", Meta = (Keywords = "C++"))
		bool CanParent(const UTGOR_SequencerComponent* Parent, const USkeletalMeshComponent* Pose) const;

	/** Pose this pilot is attached to */
	UFUNCTION(BlueprintPure, Category = "!TGOR Movement", Meta = (Keywords = "C++"))
		USkeletalMeshComponent* GetPose() const;

	/** Sets local offset to be used for pose. */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Movement", Meta = (Keywords = "C++"))
		void SetPoseOffset(const FTGOR_MovementPosition& Offset);

	/** Whether to compute relative velocity/acceleration */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "!TGOR Movement")
		bool ComputeRelativeDynamics = true;

	////////////////////////////////////////////////////////////////////////////////////////////////////
private:

	UPROPERTY(Transient)
		FTGOR_MovementPosition LocalOffset;

	/** Parenting data */
	UPROPERTY(ReplicatedUsing = RepNotifyParenting)
		FTGOR_SequencerParenting Parenting;

	/** Properly call reparent */
	UFUNCTION()
		void RepNotifyParenting(const FTGOR_SequencerParenting& Old)
		{
			FTGOR_SequencerParenting New = Parenting;
			Parenting = Old;
			Parent(New.Component.Get(), New.Pose.Get());
		}
};
