// TGOR (C) // CHECKED //
#pragma once

#include "CoreMinimal.h"
#include "RealitiesUtility/Structures/TGOR_Time.h"

#include "../TGOR_MatterInstance.h"

#include "CoreSystem/Storage/TGOR_SaveInterface.h"
#include "DimensionSystem/Interfaces/TGOR_SpawnerInterface.h"
#include "TGOR_ItemRegisterComponent.h"
#include "TGOR_MatterComponent.generated.h"


////////////////////////////////////////////// DECL //////////////////////////////////////////////////////

class UTGOR_Matter;
class UTGOR_Segment;


///////////////////////////////////////////// DELEGATES ///////////////////////////////////////////////////

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FMatterStorageDelegate);

/**
 * TGOR_MatterComponent allows storing matter
 */
UCLASS(ClassGroup = (Custom), Blueprintable, meta = (BlueprintSpawnableComponent))
class INVENTORYSYSTEM_API UTGOR_MatterComponent : public UTGOR_DimensionComponent, public ITGOR_SaveInterface, public ITGOR_SpawnerInterface
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

	virtual void UpdateContent_Implementation(FTGOR_SpawnerDependencies& Dependencies) override;
	virtual TMap<int32, UTGOR_SpawnModule*> GetModuleType_Implementation() const override;

	//////////////////////////////////////////// IMPLEMENTABLES ////////////////////////////////////////

	UPROPERTY(BlueprintAssignable, Category = "!TGOR Inventory")
		FSlotUpdateDelegate OnMatterChanged;

	////////////////////////////////////////////////////////////////////////////////////////////////////
public:

	/** Currently included slots. */
	UPROPERTY(ReplicatedUsing = RepNotifyMatterContainers, BlueprintReadOnly, Category = "!TGOR Inventory")
		FTGOR_MatterContainers MatterContainers;

	/**  */
	UFUNCTION()
		void RepNotifyMatterContainers();

	////////////////////////////////////////////////////////////////////////////////////////////////////
public:

	/** Segment types this component spawns with. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "!TGOR Stats")
		TArray<TSubclassOf<UTGOR_Segment>> SpawnSegments;

	/** Get first Segment of given type. */
	UFUNCTION(BlueprintPure, Category = "!TGOR Inventory", Meta = (Keywords = "C++"))
		UTGOR_Segment* GetSegment(TSubclassOf<UTGOR_Segment> Filter) const;

	////////////////////////////////////////////////////////////////////////////////////////////////////
public:

	/** Get currently available matter. */
	UFUNCTION(BlueprintPure, Category = "!TGOR Inventory", Meta = (Keywords = "C++"))
		TMap<UTGOR_Matter*, int32> GetComposition(TSubclassOf<UTGOR_Segment> Filter);

	/** Get currently missing matter from full capacity */
	UFUNCTION(BlueprintPure, Category = "!TGOR Inventory", Meta = (Keywords = "C++"))
		TMap<UTGOR_Matter*, int32> GetMissing(TSubclassOf<UTGOR_Segment> Filter);

	/** Get matter storage capacity of this container */
	UFUNCTION(BlueprintPure, Category = "!TGOR Inventory", Meta = (Keywords = "C++"))
		TMap<UTGOR_Matter*, int32> GetCapacity(TSubclassOf<UTGOR_Segment> Filter);

	/** Gets by this container supported matter types */
	UFUNCTION(BlueprintPure, Category = "!TGOR Inventory", Meta = (Keywords = "C++"))
		TSet<UTGOR_Matter*> GetSupported(TSubclassOf<UTGOR_Segment> Filter);

	/** Remove matter quantities by given composition, returns what actuall got removed */
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "!TGOR Inventory", Meta = (Keywords = "C++"))
		TMap<UTGOR_Matter*, int32> PayStorageMatter(TSubclassOf<UTGOR_Segment> Filter, const TMap<UTGOR_Matter*, int32>& Composition);

	/** Add matter quantities by given composition, returns leftovers */
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "!TGOR Inventory", Meta = (Keywords = "C++"))
		TMap<UTGOR_Matter*, int32> InvestStorageMatter(TSubclassOf<UTGOR_Segment> Filter, const TMap<UTGOR_Matter*, int32>& Composition);

};
