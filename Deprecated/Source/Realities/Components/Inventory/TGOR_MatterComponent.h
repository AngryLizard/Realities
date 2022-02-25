// TGOR (C) // CHECKED //
#pragma once

#include "CoreMinimal.h"
#include "Realities/Utility/Datastructures/TGOR_Time.h"
#include "Realities/Utility/TGOR_CustomEnumerations.h"

#include "Realities/Interfaces/TGOR_SaveInterface.h"
#include "Realities/Components/Inventory/TGOR_ItemRegisterComponent.h"
#include "TGOR_MatterComponent.generated.h"


////////////////////////////////////////////// DECL //////////////////////////////////////////////////////

class UTGOR_Matter;

///////////////////////////////////////////// DELEGATES ///////////////////////////////////////////////////

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FMatterStorageDelegate);

/**
 * TGOR_MatterComponent allows storing matter
 */
UCLASS(ClassGroup = (Custom), Blueprintable, meta = (BlueprintSpawnableComponent))
class REALITIES_API UTGOR_MatterComponent : public UTGOR_ItemRegisterComponent
{
	GENERATED_BODY()

public:

	////////////////////////////////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////// ENGINE ////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////////////////////////////

	UTGOR_MatterComponent();

	void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	void GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const override;

	virtual bool CanStoreItemAt(const FTGOR_SlotIndex& Index, UTGOR_ItemStorage* Storage) const override;
	virtual int32 GetContainerCapacity(const FTGOR_ContainerIndex& Index) const override;

	//////////////////////////////////////////// IMPLEMENTABLES ////////////////////////////////////////

	UPROPERTY(BlueprintAssignable, Category = "TGOR Inventory")
		FSlotUpdateDelegate OnMatterChanged;

	////////////////////////////////////////////////////////////////////////////////////////////////////
protected:

	/** Max number of supported matter providers. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "TGOR Inventory")
		int32 MaxMatterCount;

	/** What matter items are permitted in matter container */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "TGOR Inventory")
		FTGOR_ItemRestriction MatterRestriction;

	/** Matter storage containers this component starts with on spawn */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "TGOR Inventory")
		TArray<TSubclassOf<UTGOR_Item>> ContainerSpawn;

	////////////////////////////////////////////////////////////////////////////////////////////////////
public:

	/** Get currently available matter. */
	UFUNCTION(BlueprintPure, Category = "TGOR Inventory", Meta = (Keywords = "C++"))
		TMap<UTGOR_Matter*, int32> GetComposition() const;

	/** Get currently missing matter from full capacity */
	UFUNCTION(BlueprintPure, Category = "TGOR Inventory", Meta = (Keywords = "C++"))
		TMap<UTGOR_Matter*, int32> GetMissing() const;

	/** Gets by this container supported matter types */
	UFUNCTION(BlueprintPure, Category = "TGOR Inventory", Meta = (Keywords = "C++"))
		TSet<UTGOR_Matter*> GetSupportedMatter() const;

	/** Get matter storage capacity of this container */
	UFUNCTION(BlueprintPure, Category = "TGOR Storage", Meta = (Keywords = "C++"))
		int32 GetStorageCapacity(UTGOR_Matter* Matter) const;

	/** Remove matter quantities by given composition, returns what actuall got removed */
	UFUNCTION(BlueprintCallable, Category = "TGOR Inventory", Meta = (Keywords = "C++"))
		TMap<UTGOR_Matter*, int32> PayStorageMatter(const TMap<UTGOR_Matter*, int32>& Composition);

	/** Add matter quantities by given composition, returns leftovers */
	UFUNCTION(BlueprintCallable, Category = "TGOR Inventory", Meta = (Keywords = "C++"))
		TMap<UTGOR_Matter*, int32> InvestStorageMatter(const TMap<UTGOR_Matter*, int32>& Composition);

	/////////////////////////////////////////////// INTERNAL ///////////////////////////////////////////

protected:

private:

};
