#pragma once

#include "CoreMinimal.h"
#include "RealitiesUtility/Structures/TGOR_Index.h"
#include "Realities/Base/Instances/Creature/TGOR_MovementInstance.h"

#include "Realities/Base/Inventory/Modules/Mobility/TGOR_EuclideanModule.h"
#include "TGOR_SplineModule.generated.h"

class UTGOR_MobilityComponent;

/**
 *
 */
UCLASS(BlueprintType, Blueprintable)
class REALITIES_API UTGOR_SplineModule : public UTGOR_EuclideanModule
{
	GENERATED_BODY()

public:

	UTGOR_SplineModule();
	virtual bool Equals(const UTGOR_Module* Other) const override;
	virtual bool Merge(const UTGOR_Module* Other, ETGOR_NetvarEnumeration Priority) override;

	virtual void Write_Implementation(FTGOR_GroupWritePackage& Package, UTGOR_Singleton* Context) const override;
	virtual bool Read_Implementation(FTGOR_GroupReadPackage& Package, UTGOR_Singleton* Context) override;
	virtual void Send_Implementation(FTGOR_NetworkWritePackage& Package, UTGOR_Singleton* Context) const override;
	virtual bool Recv_Implementation(FTGOR_NetworkReadPackage& Package, UTGOR_Singleton* Context) override;

	virtual void Clear() override;
	virtual FString Print() const override;

	////////////////////////////////////////////////////////////////////////////////////////////////////

	/** Reset base to current position of owner component */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Storage", Meta = (Keywords = "C++"))
		void ResetToComponent(UTGOR_MobilityComponent* Owner);

	////////////////////////////////////////////////////////////////////////////////////////////////////

	/** Compute world space recursively */
	UFUNCTION(BlueprintPure, Category = "!TGOR Storage", Meta = (Keywords = "C++"))
		FTGOR_MovementPosition ComputePosition() const;

	/** Compute parent world space recursively */
	UFUNCTION(BlueprintPure, Category = "!TGOR Storage", Meta = (Keywords = "C++"))
		FTGOR_MovementPosition ComputeParentPosition() const;

	////////////////////////////////////////////////////////////////////////////////////////////////////

	/** Compute world space recursively */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Storage", Meta = (Keywords = "C++"))
		FTGOR_MovementSpace ComputeSpace() const;

	/** Compute parent world space recursively */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Storage", Meta = (Keywords = "C++"))
		FTGOR_MovementSpace ComputeParentSpace() const;

	////////////////////////////////////////////////////////////////////////////////////////////////////

	/** Simulates this component to match to a given dynamic*/
	UFUNCTION(BlueprintCallable, Category = "!TGOR Storage", Meta = (Keywords = "C++"))
		void SimulateDynamic(const FTGOR_MovementDynamic& Dynamic);

	/** Simulates this component to match a indexed part to a given dynamic */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Storage", Meta = (Keywords = "C++"))
		void SimulateDynamicWith(UTGOR_MobilityComponent* Owner, int32 Index, const FTGOR_MovementDynamic& Dynamic);

	////////////////////////////////////////////////////////////////////////////////////////////////////

	/** Get socket name for parenting */
	UFUNCTION(BlueprintPure, Category = "!TGOR Storage", Meta = (Keywords = "C++"))
		FName GetSocketName() const;

	/** Get parent component */
	UFUNCTION(BlueprintPure, Category = "!TGOR Storage", Meta = (Keywords = "C++"))
		UTGOR_MobilityComponent* GetParent() const;

	/** Get parent index */
	UFUNCTION(BlueprintPure, Category = "!TGOR Storage", Meta = (Keywords = "C++"))
		int32 GetParentIndex() const;

	////////////////////////////////////////////////////////////////////////////////////////////////////

	/** Compare */
	UFUNCTION(BlueprintPure, Category = "!TGOR Storage", Meta = (Keywords = "C++"))
		float Compare(UTGOR_SplineModule* Other, const FTGOR_MovementThreshold& Threshold) const;

	/** Lerp to another module */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Storage", Meta = (Keywords = "C++"))
		void SetLerp(UTGOR_SplineModule* Other, float Alpha);

	/** Set attached socket parent */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Storage", Meta = (Keywords = "C++"))
		bool SetParent(UTGOR_MobilityComponent* Owner, UTGOR_MobilityComponent* Parent, int32 Index);

	////////////////////////////////////////////////////////////////////////////////////////////////////
private:

	/** Parent component */
	UPROPERTY()
		TWeakObjectPtr<UTGOR_MobilityComponent> Component;

	/** Parent index (exact functionality depends on parent) */
	UPROPERTY()
		FTGOR_Index ParentIndex;

	/** Dynamic relative to parent */
	UPROPERTY()
		FTGOR_MovementDynamic Local;
};

