#pragma once

#include "CoreMinimal.h"
#include "Realities/Base/Instances/System/TGOR_ActionInstance.h"

#include "Realities/Base/Inventory/Modules/TGOR_Module.h"
#include "TGOR_SlotModule.generated.h"

/**
 * 
 */
UCLASS(BlueprintType, Blueprintable)
class REALITIES_API UTGOR_SlotModule : public UTGOR_Module
{
	GENERATED_BODY()

public:

	UTGOR_SlotModule();
	virtual bool Equals(const UTGOR_Module* Other) const override;
	virtual bool Merge(const UTGOR_Module* Other, ETGOR_NetvarEnumeration Priority) override;

	virtual void Write_Implementation(FTGOR_GroupWritePackage& Package, UTGOR_Singleton* Context) const override;
	virtual bool Read_Implementation(FTGOR_GroupReadPackage& Package, UTGOR_Singleton* Context) override;
	virtual void Send_Implementation(FTGOR_NetworkWritePackage& Package, UTGOR_Singleton* Context) const override;
	virtual bool Recv_Implementation(FTGOR_NetworkReadPackage& Package, UTGOR_Singleton* Context) override;

	virtual void Clear() override;
	virtual FString Print() const override;

	////////////////////////////////////////////////////////////////////////////////////////////////////

	/** Set source slot value. */
	UFUNCTION(BlueprintCallable, Category = "TGOR Action", Meta = (Keywords = "C++"))
		void SetSlot(const FTGOR_SlotIndex& Index);

	/** Get source slot value. */
	UFUNCTION(BlueprintPure, Category = "TGOR Action", Meta = (Keywords = "C++"))
		FTGOR_SlotIndex GetSlot() const;

	////////////////////////////////////////////////////////////////////////////////////////////////////
private:

	/** Currently stored source index */
	UPROPERTY()
		FTGOR_SlotIndex SlotIndex;
};