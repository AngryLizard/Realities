// TGOR (C) // CHECKED //
#pragma once

#include "CoreMinimal.h"

#include "SocketSystem/Components/TGOR_SocketComponent.h"
#include "TGOR_StorageComponent.generated.h"


////////////////////////////////////////////// DECL //////////////////////////////////////////////////////

class UTGOR_ItemStorage;
class UTGOR_Item;

///////////////////////////////////////////// DELEGATES ///////////////////////////////////////////////////

/**
 * TGOR_StorageComponent manages exchanging matter with items
 */
UCLASS(ClassGroup = (Custom), Blueprintable, meta = (BlueprintSpawnableComponent))
class INVENTORYSYSTEM_API UTGOR_StorageComponent : public UTGOR_SocketComponent
{
	GENERATED_BODY()

public:

	////////////////////////////////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////// ENGINE ////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////////////////////////////

	UTGOR_StorageComponent();

	void BeginPlay() override;

	//////////////////////////////////////////// IMPLEMENTABLES ////////////////////////////////////////

	////////////////////////////////////////////////////////////////////////////////////////////////////
protected:

	/** input sockets to consider for processing */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "!TGOR Inventory")
		TArray<TSubclassOf<UTGOR_NamedSocket>> InputSockets;

	////////////////////////////////////////////////////////////////////////////////////////////////////
public:

	/** Get socket you can attach an item to */
	UFUNCTION(BlueprintPure, Category = "!TGOR Inventory", Meta = (Keywords = "C++"))
		UTGOR_NamedSocket* GetEmptySocketFor(UTGOR_Item* Item) const;

	////////////////////////////////////////////////////////////////////////////////////////////////////
public:

	/** Get all attached items */
	UFUNCTION(BlueprintPure, Category = "!TGOR Inventory", Meta = (Keywords = "C++"))
		TArray<UTGOR_Item*> GetAttachedItems() const;

	/** Get all attached item containers */
	UFUNCTION(BlueprintPure, Category = "!TGOR Inventory", Meta = (Keywords = "C++"))
		TArray<UTGOR_ItemComponent*> GetAttachedContainers() const;

	/** Get currently missing matter. */
	UFUNCTION(BlueprintPure, Category = "!TGOR Inventory", Meta = (Keywords = "C++"))
		TMap<UTGOR_Matter*, int32> GetMissing() const;

	/** Get currently available matter. */
	UFUNCTION(BlueprintPure, Category = "!TGOR Inventory", Meta = (Keywords = "C++"))
		TMap<UTGOR_Matter*, int32> GetComposition() const;

	////////////////////////////////////////////////////////////////////////////////////////////////////
public:

	/** Remove matter quantities by given composition, returns what has actually been removed */
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "!TGOR Inventory", Meta = (Keywords = "C++"))
		TMap<UTGOR_Matter*, int32> RemoveComposition(const TArray<UTGOR_ItemComponent*>& Containers, TMap<UTGOR_Matter*, int32> Composition);

	/** Remove matter quantities by given composition, returns how much matter has actually been removed */
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "!TGOR Inventory", Meta = (Keywords = "C++"))
		int32 RemoveMatter(const TArray<UTGOR_ItemComponent*>& Containers, UTGOR_Matter* Matter, int32 Quantity);

	/** Add matter quantities by given composition, returns leftover matter that couldn't be added */
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "!TGOR Inventory", Meta = (Keywords = "C++"))
		TMap<UTGOR_Matter*, int32> AddComposition(const TArray<UTGOR_ItemComponent*>& Containers, TMap<UTGOR_Matter*, int32> Composition);

	/** Add matter quantities by given composition, returns leftover matter that couldn't be added */
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "!TGOR Inventory", Meta = (Keywords = "C++"))
		int32 AddMatter(const TArray<UTGOR_ItemComponent*>& Containers, UTGOR_Matter* Matter, int32 Quantity);

	/////////////////////////////////////////////// INTERNAL ///////////////////////////////////////////

	////////////////////////////////////////////////////////////////////////////////////////////////////
private:
	UPROPERTY()
		FRandomStream ContainerPermutator;

private:

};
