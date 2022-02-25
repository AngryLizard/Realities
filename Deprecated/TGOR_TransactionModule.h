#pragma once

#include "CoreMinimal.h"
#include "Realities/Base/Instances/System/TGOR_ActionInstance.h"

#include "Realities/Base/Inventory/Modules/TGOR_SlotModule.h"
#include "TGOR_TransactionModule.generated.h"

/**
 * 
 */
UCLASS(BlueprintType, Blueprintable)
class REALITIES_API UTGOR_TransactionModule : public UTGOR_SlotModule
{
	GENERATED_BODY()

public:

	UTGOR_TransactionModule();
	virtual bool Equals(const UTGOR_Module* Other) const override;
	virtual bool Merge(const UTGOR_Module* Other, ETGOR_NetvarEnumeration Priority) override;

	virtual void Write_Implementation(FTGOR_GroupWritePackage& Package, UTGOR_Singleton* Context) const override;
	virtual bool Read_Implementation(FTGOR_GroupReadPackage& Package, UTGOR_Singleton* Context) override;
	virtual void Send_Implementation(FTGOR_NetworkWritePackage& Package, UTGOR_Singleton* Context) const override;
	virtual bool Recv_Implementation(FTGOR_NetworkReadPackage& Package, UTGOR_Singleton* Context) override;

	virtual void Clear() override;
	virtual FString Print() const override;

	////////////////////////////////////////////////////////////////////////////////////////////////////

	/** Set target slot value. */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Storage", Meta = (Keywords = "C++"))
		void SetTargetSlotIdentifier(int32 Identifier, bool Flag);

	/** Get target slot value. */
	UFUNCTION(BlueprintPure, Category = "!TGOR Storage", Meta = (Keywords = "C++"))
		int32 GetTargetSlotIdentifier() const;

	/** Get target slot flag. */
	UFUNCTION(BlueprintPure, Category = "!TGOR Storage", Meta = (Keywords = "C++"))
		bool GetTargetSlotFlag() const;

	////////////////////////////////////////////////////////////////////////////////////////////////////
private:

	/** Currently stored target index */
	UPROPERTY()
		int32 TargetIdentifier;

	/** Currently stored target flag */
	UPROPERTY()
		bool TargetFlag;
};