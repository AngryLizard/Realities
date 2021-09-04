// TGOR (C) // CHECKED //
#pragma once

#include "CoreMinimal.h"
#include "DimensionSystem/TGOR_PilotInstance.h"

#include "TGOR_SocketPilotTask.h"
#include "TGOR_PipelineSocketTask.generated.h"

////////////////////////////////////////////// DECL //////////////////////////////////////////////////////

class UTGOR_PipelineSocket;
class UTGOR_PipelineComponent;

/**
*
*/
USTRUCT(BlueprintType)
struct SOCKETSYSTEM_API FTGOR_PipelineSocketParenting
{
	GENERATED_USTRUCT_BODY();
	FTGOR_PipelineSocketParenting();
	bool operator==(const FTGOR_PipelineSocketParenting& Other) const;
	bool operator!=(const FTGOR_PipelineSocketParenting& Other) const;

	/** Socket type */
	UPROPERTY()
		UTGOR_PipelineSocket* Socket;

	/** Parent component */
	UPROPERTY()
		TWeakObjectPtr<UTGOR_PipelineComponent> Component;
};

/**
*
*/
USTRUCT(BlueprintType)
struct SOCKETSYSTEM_API FTGOR_PipelineSocketState
{
	GENERATED_USTRUCT_BODY();

	FTGOR_PipelineSocketState();
	void SetLerpState(const FTGOR_PipelineSocketState& Other, float Alpha);
	float CompareState(const FTGOR_PipelineSocketState& Other, const FTGOR_MovementThreshold& Threshold) const;

	/** Distance on pipeline */
	UPROPERTY()
		float SocketDistance;

	/** Speed on pipeline relative to socket direction */
	UPROPERTY()
		float SocketSpeed;

	/** Direction on the pipeline */
	UPROPERTY()
		float SocketDirection;
};


/**
*
*/
UCLASS(Blueprintable)
class SOCKETSYSTEM_API UTGOR_PipelineSocketTask : public UTGOR_SocketPilotTask
{
	GENERATED_BODY()

		friend class UTGOR_PipelineComponent;

public:

	////////////////////////////////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////// ENGINE ////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////////////////////////////

	UTGOR_PipelineSocketTask();
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	////////////////////////////////////////////////////////////////////////////////////////////////////
public:

	virtual UTGOR_MobilityComponent* GetParent() const override;
	virtual FTGOR_MovementPosition ComputePosition() const override;
	virtual FTGOR_MovementSpace ComputeSpace() const override;
	virtual void Update(float DeltaTime) override;
	virtual void Unparent() override;

	virtual void Register() override;
	virtual void Unregister() override;

	virtual UTGOR_Socket* GetSocket() const override;

	////////////////////////////////////////////////////////////////////////////////////////////////////

	/** Get whether direction is pointing forward*/
	UFUNCTION(BlueprintPure, Category = "!TGOR Storage", Meta = (Keywords = "C++"))
		bool IsForward() const;

	/** Get current socket distance on the spline */
	UFUNCTION(BlueprintPure, Category = "!TGOR Storage", Meta = (Keywords = "C++"))
		float GetDistance() const;

	/** Get current socket speed on the spline */
	UFUNCTION(BlueprintPure, Category = "!TGOR Storage", Meta = (Keywords = "C++"))
		float GetSpeed() const;

	/** Get visibility blend (distance from a closed end-point) */
	UFUNCTION(BlueprintPure, Category = "!TGOR Storage", Meta = (Keywords = "C++"))
		float GetBlend(float Threshold) const;

	/** Get transform of this socket */
	UFUNCTION(BlueprintPure, Category = "!TGOR Storage", Meta = (Keywords = "C++"))
		FTGOR_MovementDynamic GetDynamic() const;

	////////////////////////////////////////////////////////////////////////////////////////////////////

	/** Change distance on pipeline according to current speed */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Storage", Meta = (Keywords = "C++"))
		bool Simulate(UTGOR_PilotComponent* Attachee, float Timestep);

	////////////////////////////////////////////////////////////////////////////////////////////////////

	/** Parent this task to a given socket */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Movement", Meta = (Keywords = "C++"))
		void Parent(UTGOR_PipelineComponent* Parent, UTGOR_PipelineSocket* Socket);

	/** Whether this task can parent to a given socket */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Movement", Meta = (Keywords = "C++"))
		bool CanParent(const UTGOR_PipelineComponent* Parent, UTGOR_PipelineSocket* Socket) const;

	////////////////////////////////////////////////////////////////////////////////////////////////////
private:

	/** Parenting data */
	UPROPERTY(ReplicatedUsing = RepNotifyParenting)
		FTGOR_PipelineSocketParenting Parenting;

	/** Properly call reparent */
	UFUNCTION()
		void RepNotifyParenting(const FTGOR_PipelineSocketParenting& Old)
	{
		FTGOR_PipelineSocketParenting New = Parenting;
		Parenting = Old;
		Parent(New.Component.Get(), New.Socket);
	}

	/** State on pipeline */
	UPROPERTY(Replicated)
		FTGOR_PipelineSocketState State;

};
