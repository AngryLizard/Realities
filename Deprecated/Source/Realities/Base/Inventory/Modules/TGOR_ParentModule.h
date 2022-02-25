#pragma once

#include "CoreMinimal.h"
#include "Realities/Utility/Structs/TGOR_Index.h"
#include "Realities/Base/Instances/Creature/TGOR_MovementInstance.h"

#include "Realities/Base/Inventory/Modules/TGOR_SlotModule.h"
#include "TGOR_ParentModule.generated.h"

class UTGOR_MobilityComponent;

/**
 *
 */
UCLASS(BlueprintType, Blueprintable)
class REALITIES_API UTGOR_ParentModule : public UTGOR_Module
{
	GENERATED_BODY()

public:

	UTGOR_ParentModule();
	virtual bool Equals(const UTGOR_Module* Other) const override;
	virtual bool Merge(const UTGOR_Module* Other, ETGOR_NetvarEnumeration Priority) override;

	virtual void Write_Implementation(FTGOR_GroupWritePackage& Package, UTGOR_Singleton* Context) const override;
	virtual bool Read_Implementation(FTGOR_GroupReadPackage& Package, UTGOR_Singleton* Context) override;
	virtual void Send_Implementation(FTGOR_NetworkWritePackage& Package, UTGOR_Singleton* Context) const override;
	virtual bool Recv_Implementation(FTGOR_NetworkReadPackage& Package, UTGOR_Singleton* Context) override;

	virtual void Clear() override;
	virtual FString Print() const override;

	/** Get socket name for parenting */
	UFUNCTION(BlueprintPure, Category = "TGOR Storage", Meta = (Keywords = "C++"))
		FName GetSocketName() const;

	/** Get parent component */
	UFUNCTION(BlueprintPure, Category = "TGOR Storage", Meta = (Keywords = "C++"))
		UTGOR_MobilityComponent* GetParent() const;

	/** Get transform of this socket */
	UFUNCTION(BlueprintPure, Category = "TGOR Storage", Meta = (Keywords = "C++"))
		FTGOR_MovementPosition GetTransform() const;

	////////////////////////////////////////////////////////////////////////////////////////////////////

	/** Set attached socket parent */
	UFUNCTION(BlueprintCallable, Category = "TGOR Storage", Meta = (Keywords = "C++"))
		void SetParent(UTGOR_MobilityComponent* Parent, FTGOR_Index Index);

	////////////////////////////////////////////////////////////////////////////////////////////////////
private:

	/** Parent component */
	UPROPERTY()
		TWeakObjectPtr<UTGOR_MobilityComponent> Component;

	/** Parent index */
	UPROPERTY()
		FTGOR_Index BoneIndex;
};