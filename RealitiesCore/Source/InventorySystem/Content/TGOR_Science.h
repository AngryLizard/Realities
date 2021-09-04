// The Gateway of Realities: Planes of Existence.

#pragma once

#include "TGOR_MatterProcess.h"
#include "TGOR_Science.generated.h"

class UTGOR_ScienceStorage;
class UTGOR_ScienceComponent;
class UTGOR_ItemComponent;
class UTGOR_Item;
//class UTGOR_Matter; // for some reason this lets me not use content templates and I have to include the header in here. Why why why why why why why why. (see * in cpp)

/**
 * 
 */
UCLASS()
class INVENTORYSYSTEM_API UTGOR_Science : public UTGOR_MatterProcess
{
	GENERATED_BODY()
	
public:
	UTGOR_Science();

	virtual float Process(UTGOR_ProcessComponent* Owner, UTGOR_ProcessStorage* Storage, float Antimatter) override;
	virtual bool CanProcess(UTGOR_ProcessComponent* Owner, UTGOR_ProcessStorage* Storage) const override;
	virtual void BuildStorage(UTGOR_ProcessStorage* Storage) override;
	virtual void PostBuildResource() override;

	////////////////////////////////////////////////////////////////////////////////////////////////////
public:

	/** Tick this science with given storage and owner, return actually used antimatter after process has concluded. */
	UFUNCTION(BlueprintImplementableEvent, Category = "!TGOR Inventory", Meta = (Keywords = "C++"))
		float OnProcess(UTGOR_StorageComponent* Sockets, UTGOR_ProcessStorage* Storage, float Antimatter);

	/** Check whether this science is still valid to be processed. */
	UFUNCTION(BlueprintImplementableEvent, Category = "!TGOR Inventory", Meta = (Keywords = "C++"))
		bool OnCanProcess(UTGOR_StorageComponent* Sockets, UTGOR_ProcessStorage* Storage) const;

	////////////////////////////////////////////////////////////////////////////////////////////////////
protected:

	/** Check whether a given container is a valid input to this process. */
	UFUNCTION(BlueprintPure, Category = "!TGOR Inventory", Meta = (Keywords = "C++"))
		bool IsValidInput(UTGOR_ItemComponent* Container) const;

	/** Check whether a given container is a valid input to this process. */
	UFUNCTION(BlueprintPure, Category = "!TGOR Inventory", Meta = (Keywords = "C++"))
		TArray<UTGOR_ItemComponent*> FilterInput(const TArray<UTGOR_ItemComponent*>& Containers) const;

	/** */
	UFUNCTION(BlueprintPure, Category = "!TGOR Inventory", Meta = (Keywords = "C++"))
		const TMap<UTGOR_Matter*, int32>& GetInputCapacity() const;

	////////////////////////////////////////////////////////////////////////////////////////////////////
public:

	/** Main body of this spawner */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "!TGOR Insertion")
		TArray<TSubclassOf<UTGOR_Item>> ItemInsertions;
	DECL_INSERTION(ItemInsertions);

	/** Main body of this spawner */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "!TGOR Insertion")
		TMap<TSubclassOf<UTGOR_Matter>, int32> MatterInsertions;
	DECL_INSERTION(MatterInsertions);

	virtual void MoveInsertion(UTGOR_Content* Insertion, ETGOR_InsertionActionEnumeration Action, bool& Success) override;

};
