#pragma once

#include "CoreMinimal.h"

#include "Realities/Base/Inventory/TGOR_Storage.h"
#include "TGOR_ItemStorage.generated.h"


/**
 * The abstract base class for all item storages.
 */
UCLASS(Blueprintable)
class REALITIES_API UTGOR_ItemStorage : public UTGOR_Storage
{
	GENERATED_BODY()

	friend struct FTGOR_ItemInstance; 
	friend class UTGOR_Item;

public:

	UTGOR_ItemStorage() {}
	virtual void Clear() override;
	virtual FString Print() const override;
	virtual void Tick(UTGOR_DimensionComponent* Owner, float DeltaSeconds) override;
	virtual bool Compatible(const UTGOR_Container* Other) const override;

	virtual TMap<UTGOR_Matter*, int32> GetMissing() const override;
	virtual TMap<UTGOR_Matter*, int32> GetComposition() const override;
	virtual int32 RemoveMatter(UTGOR_Matter* Matter, int32 Quantity) override;
	virtual int32 AddMatter(UTGOR_Matter* Matter, int32 Quantity) override;
	virtual int32 CountMatter(UTGOR_Matter* Matter) const override;
	
	virtual void Write_Implementation(FTGOR_GroupWritePackage& Package, UTGOR_Singleton* Context) const override;
	virtual bool Read_Implementation(FTGOR_GroupReadPackage& Package, UTGOR_Singleton* Context) override;
	virtual void Send_Implementation(FTGOR_NetworkWritePackage& Package, UTGOR_Singleton* Context) const override;
	virtual bool Recv_Implementation(FTGOR_NetworkReadPackage& Package, UTGOR_Singleton* Context) override;

	virtual bool Equals(const UTGOR_Storage* Other) const override;
	virtual UTGOR_Content* GetOwner() const override;

	////////////////////////////////////////////////////////////////////////////////////////////////////

	/** Return the content item that is stored inside this storage. */
	UFUNCTION(BlueprintPure, Category = "TGOR Storage", Meta = (Keywords = "C++"))
		virtual UTGOR_Item* GetItem() const;

	////////////////////////////////////////////////////////////////////////////////////////////////////

	/** Gets mass depending on current composition */
	UFUNCTION(BlueprintPure, Category = "TGOR Storage", Meta = (Keywords = "C++"))
		float GetMass() const;

	/** Check whether this storage has any matter left */
	UFUNCTION(BlueprintPure, Category = "TGOR Storage", Meta = (Keywords = "C++"))
		bool HasAnyMatter() const;

	////////////////////////////////////////////////////////////////////////////////////////////////////

	/** Empty out matter */
	UFUNCTION(BlueprintCallable, Category = "TGOR Storage", Meta = (Keywords = "C++"))
		void EmptyMatter();

	////////////////////////////////////////////////////////////////////////////////////////////////////
protected:

	/** The item of this storage. */
	UPROPERTY(VisibleAnywhere, Category = "TGOR Storage")
		UTGOR_Item* ItemRef;

	/** Current matter composition (non-defined matter is assumed to be fully present). */
	UPROPERTY(BlueprintReadOnly, Category = "TGOR Storage")
		TMap<UTGOR_Matter*, int32> CompositionChangeList;
};
