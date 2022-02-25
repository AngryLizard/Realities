#pragma once

#include "CoreMinimal.h"

#include "Realities/Base/Inventory/TGOR_Container.h"
#include "TGOR_Storage.generated.h"

class UTGOR_Item;
class UTGOR_Matter;

/**
 * The abstract base class for all storages.
 */
UCLASS(Abstract, Blueprintable)
class REALITIES_API UTGOR_Storage : public UTGOR_Container
{
	GENERATED_BODY()

public:

	UTGOR_Storage() {}
	virtual void Clear() override;
	virtual FString Print() const override;
	virtual void Tick(UTGOR_DimensionComponent* Owner, float DeltaSeconds) override;

	virtual TMap<UTGOR_Matter*, int32> GetMissing() const override;
	virtual TMap<UTGOR_Matter*, int32> GetComposition() const override;
	virtual int32 RemoveMatter(UTGOR_Matter* Matter, int32 Quantity) override;
	virtual int32 AddMatter(UTGOR_Matter* Matter, int32 Quantity) override;
	virtual int32 CountMatter(UTGOR_Matter* Matter) const override;

	virtual bool Contains(const UTGOR_Storage* Other) const override;
	virtual bool Purge(const UTGOR_Storage* Other) override;

	virtual bool PurgeDirt() override;
	virtual bool HasNotify() override;

	virtual void Write_Implementation(FTGOR_GroupWritePackage& Package, UTGOR_Singleton* Context) const override;
	virtual bool Read_Implementation(FTGOR_GroupReadPackage& Package, UTGOR_Singleton* Context) override;
	virtual void Send_Implementation(FTGOR_NetworkWritePackage& Package, UTGOR_Singleton* Context) const override;
	virtual bool Recv_Implementation(FTGOR_NetworkReadPackage& Package, UTGOR_Singleton* Context) override;

	////////////////////////////////////////////////////////////////////////////////////////////////////

	/** Compare this storage with another storage. */
	UFUNCTION(BlueprintCallable, Category = "TGOR Storage", Meta = (Keywords = "C++"))
		virtual bool Equals(const UTGOR_Storage* Other) const;

	/** Merge this storage with another storage. */
	UFUNCTION(BlueprintCallable, Category = "TGOR Storage", Meta = (Keywords = "C++"))
		virtual bool Merge(const UTGOR_Storage* Other, ETGOR_NetvarEnumeration Priority);

	/** Get owning content of this storage */
	UFUNCTION(BlueprintCallable, Category = "TGOR Storage", Meta = (Keywords = "C++"))
		virtual UTGOR_Content* GetOwner() const;

	////////////////////////////////////////////////////////////////////////////////////////////////////

	/** Build module objects. */
	UFUNCTION(BlueprintCallable, Category = "TGOR Storage", Meta = (Keywords = "C++"))
		void BuildModules(const FTGOR_StorageDeclaration& Setup);

	/** Get first module that matches module type. */
	UFUNCTION(BlueprintCallable, Category = "TGOR Storage", Meta = (DeterminesOutputType = "Type", ExpandEnumAsExecs = "Branches", Keywords = "C++"))
		UTGOR_Module* GetModuleChecked(TSubclassOf<UTGOR_Module> Type, ETGOR_FetchEnumeration& Branches);

	/** Get first module that matches module type. */
	UFUNCTION(BlueprintPure, Category = "TGOR Storage", Meta = (DeterminesOutputType = "Type", Keywords = "C++"))
		UTGOR_Module* GetModule(TSubclassOf<UTGOR_Module> Type) const;

	template<typename T> T* GetModule() const
	{
		return Cast<T>(GetModule(T::StaticClass()));
	}

	////////////////////////////////////////////////////////////////////////////////////////////////////
protected:

	/** Modules of this storage with same identifiers as defined in item. The identifiers are mostly used for file storage. */
	UPROPERTY(VisibleAnywhere, Category = "TGOR Storage")
		TMap<FName, UTGOR_Module*> Modules;

};
