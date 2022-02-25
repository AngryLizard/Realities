#pragma once

#include "CoreMinimal.h"

#include "Realities/Base/Inventory/TGOR_Storage.h"
#include "TGOR_ProcessStorage.generated.h"

class UTGOR_ProcessComponent;

/**
 * The abstract base class for all science storages.
 */
UCLASS(Blueprintable)
class REALITIES_API UTGOR_ProcessStorage : public UTGOR_Storage
{
	GENERATED_BODY()

	friend struct FTGOR_ProcessInstance; 
	friend class UTGOR_Process;

public:
	
	UTGOR_ProcessStorage() {}
	virtual void Clear() override;
	virtual FString Print() const override;
	virtual void Tick(UTGOR_DimensionComponent* Owner, float DeltaSeconds) override;
	virtual bool Compatible(const UTGOR_Container* Other) const override;

	virtual void Write_Implementation(FTGOR_GroupWritePackage& Package, UTGOR_Singleton* Context) const override;
	virtual bool Read_Implementation(FTGOR_GroupReadPackage& Package, UTGOR_Singleton* Context) override;
	virtual void Send_Implementation(FTGOR_NetworkWritePackage& Package, UTGOR_Singleton* Context) const override;
	virtual bool Recv_Implementation(FTGOR_NetworkReadPackage& Package, UTGOR_Singleton* Context) override;

	virtual bool Equals(const UTGOR_Storage* Other) const override;
	virtual UTGOR_Content* GetOwner() const override;

	////////////////////////////////////////////////////////////////////////////////////////////////////

	/** Return the content item that is stored inside this storage. */
	UFUNCTION(BlueprintPure, Category = "TGOR Storage", Meta = (Keywords = "C++"))
		virtual UTGOR_Process* GetProcess() const;

	/** Do processing with generated antimatter amount. */
	UFUNCTION(BlueprintCallable, Category = "TGOR Storage", Meta = (Keywords = "C++"))
		bool Process(UTGOR_ProcessComponent* Owner, float Antimatter);

	////////////////////////////////////////////////////////////////////////////////////////////////////
protected:

	/** The process of this storage. */
	UPROPERTY(VisibleAnywhere, Category = "TGOR Storage")
		UTGOR_Process* ProcessRef;

	/** Antimatter currently accumulated for this process. */
	UPROPERTY(VisibleAnywhere, Category = "TGOR Storage")
		float Accumulation;
};
