// TGOR (C) // CHECKED //
#pragma once

#include "CoreMinimal.h"

#include "TGOR_PilotTask.h"
#include "TGOR_LinearPilotTask.generated.h"

/**
*
*/
USTRUCT(BlueprintType)
struct DIMENSIONSYSTEM_API FTGOR_LinearParenting
{
	GENERATED_USTRUCT_BODY();
	bool operator==(const FTGOR_LinearParenting& Other) const;
	bool operator!=(const FTGOR_LinearParenting& Other) const;

	/** Parent component */
	UPROPERTY()
		TWeakObjectPtr<UTGOR_MobilityComponent> Component;

	/** Parent index (exact functionality depends on parent) */
	UPROPERTY()
		FTGOR_Index Index;
};

/**
*
*/
UCLASS(Blueprintable)
class DIMENSIONSYSTEM_API UTGOR_LinearPilotTask : public UTGOR_PilotTask
{
	GENERATED_BODY()

	friend class UTGOR_MobilityComponent;

public:

	////////////////////////////////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////// ENGINE ////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////////////////////////////

	UTGOR_LinearPilotTask();
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	////////////////////////////////////////////////////////////////////////////////////////////////////
public:

	virtual UTGOR_MobilityComponent* GetParent() const override;
	virtual void Unparent() override;

	virtual void Register() override;
	virtual void Unregister() override;

	////////////////////////////////////////////////////////////////////////////////////////////////////

	/** Compute parent world space recursively */
	UFUNCTION(BlueprintPure, Category = "!TGOR Movement", Meta = (Keywords = "C++"))
		FTGOR_MovementPosition ComputeParentPosition() const;

	/** Compute parent world space recursively */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Movement", Meta = (Keywords = "C++"))
		FTGOR_MovementSpace ComputeParentSpace() const;

	////////////////////////////////////////////////////////////////////////////////////////////////////
public:

	/** Simulates this component to match a given dynamic */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Movement", Meta = (Keywords = "C++"))
		virtual void SimulateDynamic(const FTGOR_MovementDynamic& Dynamic);

	/** Simulates this component to match a given dynamic */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Movement", Meta = (Keywords = "C++"))
		virtual void SimulatePosition(const FTGOR_MovementPosition& Position);

	////////////////////////////////////////////////////////////////////////////////////////////////////
public:

	/** Parent this task to a given mobility. Does not preserve current world transform. */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Movement", Meta = (Keywords = "C++"))
		void Parent(UTGOR_MobilityComponent* Parent, int32 Index);

	/** Whether this task can parent to a given mobility */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Movement", Meta = (Keywords = "C++"))
		bool CanParent(const UTGOR_MobilityComponent* Parent, int32 Index) const;

	////////////////////////////////////////////////////////////////////////////////////////////////////
private:

	/** Parenting data */
	UPROPERTY(ReplicatedUsing = RepNotifyParenting)
		FTGOR_LinearParenting Parenting;

	/** Properly call reparent */
	UFUNCTION()
		void RepNotifyParenting(const FTGOR_LinearParenting& Old)
		{
			FTGOR_LinearParenting New = Parenting;
			Parenting = Old;
			Parent(New.Component.Get(), New.Index);
		}

};
