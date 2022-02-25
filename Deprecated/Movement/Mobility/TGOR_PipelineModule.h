#pragma once

#include "CoreMinimal.h"
#include "Realities/Base/Instances/Creature/TGOR_MovementInstance.h"

#include "Realities/Base/Inventory/Modules/TGOR_SlotModule.h"
#include "TGOR_PipelineModule.generated.h"

class UTGOR_PipelineComponent;

/**
 * 
 */
UCLASS(BlueprintType, Blueprintable)
class REALITIES_API UTGOR_PipelineModule : public UTGOR_Module
{
	GENERATED_BODY()

public:

	UTGOR_PipelineModule();
	virtual bool Equals(const UTGOR_Module* Other) const override;
	virtual bool Merge(const UTGOR_Module* Other, ETGOR_NetvarEnumeration Priority) override;

	virtual void Write_Implementation(FTGOR_GroupWritePackage& Package, UTGOR_Singleton* Context) const override;
	virtual bool Read_Implementation(FTGOR_GroupReadPackage& Package, UTGOR_Singleton* Context) override;
	virtual void Send_Implementation(FTGOR_NetworkWritePackage& Package, UTGOR_Singleton* Context) const override;
	virtual bool Recv_Implementation(FTGOR_NetworkReadPackage& Package, UTGOR_Singleton* Context) override;

	virtual void Clear() override;
	virtual FString Print() const override;


	////////////////////////////////////////////////////////////////////////////////////////////////////

	/** Compute world space recursively */
	UFUNCTION(BlueprintPure, Category = "!TGOR Storage", Meta = (Keywords = "C++"))
		FTGOR_MovementPosition ComputePosition() const;

	/** Compute world space recursively */
	UFUNCTION(BlueprintPure, Category = "!TGOR Storage", Meta = (Keywords = "C++"))
		FTGOR_MovementSpace ComputeSpace() const;

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

	/** Get parent component */
	UFUNCTION(BlueprintPure, Category = "!TGOR Storage", Meta = (Keywords = "C++"))
		UTGOR_PipelineComponent* GetParent() const;

	/** Get transform of this socket */
	UFUNCTION(BlueprintPure, Category = "!TGOR Storage", Meta = (Keywords = "C++"))
		FTGOR_MovementDynamic GetDynamic() const;

	////////////////////////////////////////////////////////////////////////////////////////////////////

	/** Change distance on pipeline according to current speed */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Storage", Meta = (Keywords = "C++"))
		bool Simulate(UTGOR_PilotComponent* Attachee, float Timestep);

	////////////////////////////////////////////////////////////////////////////////////////////////////

	/** Set attached socket state */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Storage", Meta = (Keywords = "C++"))
		void SetState(float Distance, float Speed);

	/** Set attached socket parent */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Storage", Meta = (Keywords = "C++"))
		void SetParent(UTGOR_PipelineComponent* Parent);

	/** Set attached socket name */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Storage", Meta = (Keywords = "C++"))
		void SetSocketDirection(bool Forward);

	////////////////////////////////////////////////////////////////////////////////////////////////////

	/** Compare */
	UFUNCTION(BlueprintPure, Category = "!TGOR Storage", Meta = (Keywords = "C++"))
		float Compare(UTGOR_PipelineModule* Other, const FTGOR_MovementThreshold& Threshold) const;

	/** Set attached socket parent */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Storage", Meta = (Keywords = "C++"))
		void SetLerp(UTGOR_PipelineModule* Other, float Alpha);

	////////////////////////////////////////////////////////////////////////////////////////////////////
private:

	/** Parent component */
	UPROPERTY()
		TWeakObjectPtr<UTGOR_PipelineComponent> Component;

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