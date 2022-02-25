#pragma once

#include "CoreMinimal.h"

#include "Realities/Base/Inventory/Modules/TGOR_Module.h"
#include "TGOR_MatterModule.generated.h"


/**
 * 
 */
UCLASS(BlueprintType, Blueprintable)
class REALITIES_API UTGOR_MatterModule : public UTGOR_Module
{
	GENERATED_BODY()

public:

	UTGOR_MatterModule();
	virtual bool Equals(const UTGOR_Module* Other) const override;
	virtual bool Merge(const UTGOR_Module* Other, ETGOR_NetvarEnumeration Priority) override;

	virtual void Write_Implementation(FTGOR_GroupWritePackage& Package, UTGOR_Singleton* Context) const override;
	virtual bool Read_Implementation(FTGOR_GroupReadPackage& Package, UTGOR_Singleton* Context) override;
	virtual void Send_Implementation(FTGOR_NetworkWritePackage& Package, UTGOR_Singleton* Context) const override;
	virtual bool Recv_Implementation(FTGOR_NetworkReadPackage& Package, UTGOR_Singleton* Context) override;

	virtual void Clear() override;
	virtual FString Print() const override;

	virtual TMap<UTGOR_Matter*, int32> GetMissing() const override;
	virtual TMap<UTGOR_Matter*, int32> GetComposition() const override;
	virtual int32 RemoveMatter(UTGOR_Matter* Matter, int32 Quantity) override;
	virtual int32 AddMatter(UTGOR_Matter* Matter, int32 Quantity) override;
	virtual int32 CountMatter(UTGOR_Matter* Matter) const override;

	////////////////////////////////////////////////////////////////////////////////////////////////////

	/** Gets by this storage supported matter types */
	UFUNCTION(BlueprintPure, Category = "TGOR Storage", Meta = (Keywords = "C++"))
		TSet<UTGOR_Matter*> GetSupportedMatter() const;

	/** Remove matter quantities by given composition, return leftover storage to pay */
	UFUNCTION(BlueprintCallable, Category = "TGOR Storage", Meta = (Keywords = "C++"))
		TMap<UTGOR_Matter*, int32> PayStorageMatter(const TMap<UTGOR_Matter*, int32>& Composition);

	/** Add matter quantities by given composition */
	UFUNCTION(BlueprintCallable, Category = "TGOR Storage", Meta = (Keywords = "C++"))
		TMap<UTGOR_Matter*, int32> InvestStorageMatter(const TMap<UTGOR_Matter*, int32>& Composition);

	////////////////////////////////////////////////////////////////////////////////////////////////////

	/** Add matter capacity */
	UFUNCTION(BlueprintCallable, Category = "TGOR Storage", Meta = (Keywords = "C++"))
		void AddStorageCapacity(UTGOR_Matter* Matter, int32 Capacity);

	/** Get matter storage capacity */
	UFUNCTION(BlueprintPure, Category = "TGOR Storage", Meta = (Keywords = "C++"))
		int32 GetStorageCapacity(UTGOR_Matter* Matter) const;

	////////////////////////////////////////////////////////////////////////////////////////////////////
private:

	/** Matter storage capacity of this module */
	UPROPERTY()
		TMap<UTGOR_Matter*, int32> StorageCapacity;

	////////////////////////////////////////////////////////////////////////////////////////////////////
private:

	/** Currently stored matter (non-defined matter is assumed to be 0). */
	UPROPERTY(VisibleAnywhere, Category = "TGOR Storage")
		TMap<UTGOR_Matter*, int32> StorageChangeList;
};