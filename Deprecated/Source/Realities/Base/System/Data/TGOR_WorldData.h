// The Gateway of Realities: Planes of Existence.

#pragma once

#include "CoreMinimal.h"
#include "EngineDefines.h"
#include "Realities/Utility/Error/TGOR_Error.h"
#include "Realities/Utility/Datastructures/TGOR_SpatialTree.h"
#include "Realities/Base/Instances/Dimension/TGOR_DimensionDataInstance.h"
#include "Templates/SubclassOf.h"

#include "Realities/Interfaces/TGOR_SingletonInterface.h"
#include "Realities/Base/System/Data/TGOR_Data.h"
#include "TGOR_WorldData.generated.h"

#define SPATIAL_MIN_BOTTOM 250000
#define SPATIAL_BOUNDS (HALF_WORLD_MAX1 / 4)
#define SPATIAL_LOCATION(F) FVector(-SPATIAL_BOUNDS, -SPATIAL_BOUNDS, F)
#define SPATIAL_SIZE(F)		FVector(SPATIAL_BOUNDS * 2.0f, SPATIAL_BOUNDS * 2.0f, F)

////////////////////////////////////////////// DECL //////////////////////////////////////////////////////

class ATGOR_Pawn;
class UTGOR_Dimension;
class UTGOR_DimensionData;
class ATGOR_PhysicsVolume;

////////////////////////////////////////////// ENUM //////////////////////////////////////////////////////

/**
* TGOR_WorldData allows storing dimensions and handles storing/loading all registered actors
*/
UCLASS(BlueprintType)
class REALITIES_API UTGOR_WorldData : public UTGOR_Data
{
	GENERATED_BODY()
	
public:
	UTGOR_WorldData();
	UWorld* GetWorld() const override;

	virtual void Write_Implementation(FTGOR_GroupWritePackage& Package, UTGOR_Singleton* Context) const override;
	virtual bool Read_Implementation(FTGOR_GroupReadPackage& Package, UTGOR_Singleton* Context) override;

	////////////////////////////////////////////////////////////////////////////////////////////////////
protected:

	/** True if resources were already assembled (Mods already loaded) */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "TGOR Dimension")
		bool ContentManagerAssembled;

public:

	/** Get dimension directory */
	UFUNCTION(BlueprintPure, Category = "TGOR Dimension", Meta = (Keywords = "C++"))
		FString GetDimensionDirectory() const;

	/** Get identity component with a given identifier */
	UFUNCTION(BlueprintPure, Category = "TGOR Dimension", Meta = (ExpandEnumAsExecs = "Branches", Keywords = "C++"))
		UTGOR_IdentityComponent* GetIdentity(const FTGOR_DimensionIdentifier& Identifier, ETGOR_FetchEnumeration& Branches) const;

		template<typename T> T* GetIdentityActor(const FTGOR_DimensionIdentifier& Identifier) const
		{
			ETGOR_FetchEnumeration State;
			UTGOR_IdentityComponent* Identity = GetIdentity(Identifier, State);
			if (IsValid(Identity))
			{
				return Cast<T>(Identity->GetOwner());
			}
			return nullptr;
		}

	/** Get identifier of an actor if they have a registered identity component */
	UFUNCTION(BlueprintPure, Category = "TGOR Dimension", Meta = (ExpandEnumAsExecs = "Branches", Keywords = "C++"))
		FTGOR_DimensionIdentifier GetIdentifier(AActor* Actor, ETGOR_FetchEnumeration& Branches) const;

	/** Get identity component with a given identifier */
	UFUNCTION(BlueprintPure, Category = "TGOR Dimension", Meta = (DeterminesOutputType = "ComponentClass", ExpandEnumAsExecs = "Branches", Keywords = "C++"))
		UActorComponent* GetIdentityComponent(TSubclassOf<UActorComponent> ComponentClass, const FTGOR_ComponentIdentifier& Identifier, ETGOR_FetchEnumeration& Branches) const;

		template<typename T> T* GetIdentityComponent(const FTGOR_ComponentIdentifier& Identifier) const
		{
			ETGOR_FetchEnumeration State;
			return Cast<T>(GetIdentityComponent(T::StaticClass(), Identifier, State));
		}


	/** Get identifier of a component if their owner has a registered identity component */
	UFUNCTION(BlueprintPure, Category = "TGOR Dimension", Meta = (ExpandEnumAsExecs = "Branches", Keywords = "C++"))
		FTGOR_ComponentIdentifier GetComponentIdentifier(UActorComponent* Component, ETGOR_FetchEnumeration& Branches) const;

	////////////////////////////////////////////////////////////////////////////////////////////////////

	 /** Saves all currently open dimensions and this world to file */
	UFUNCTION(BlueprintCallable, Category = "TGOR Dimension", Meta = (Keywords = "C++"))
		void SaveWorldToFile();

	/** Loads all currently open dimensions and this world from file */
	UFUNCTION(BlueprintCallable, Category = "TGOR Dimension", Meta = (Keywords = "C++"))
		void LoadWorldFromFile();

	////////////////////////////////////////////////////////////////////////////////////////////////////
public:

	 /** Finds dimension that holds given position */
	 UFUNCTION(BlueprintCallable, Category = "TGOR Dimension", Meta = (ExpandEnumAsExecs = "Branches", Keywords = "C++"))
		UTGOR_DimensionData* GetDimensionFromLocation(const FVector& Location, ETGOR_FetchEnumeration& Branches);

	 /** Allocates space for minidimensions */
	 UFUNCTION(BlueprintCallable, Category = "TGOR Dimension", Meta = (ExpandEnumAsExecs = "Branches", Keywords = "C++"))
		 FVector AllocateMiniDimension(const FVector& Bounds, ETGOR_FetchEnumeration& Branches);

	 /** Allocates space for realdimensions */
	 UFUNCTION(BlueprintCallable, Category = "TGOR Dimension", Meta = (ExpandEnumAsExecs = "Branches", Keywords = "C++"))
		 FVector AllocateRealDimension(const FVector& Bounds, ETGOR_FetchEnumeration& Branches);

	 /** Deallocates space for minidimensions */
	 UFUNCTION(BlueprintCallable, Category = "TGOR Dimension", Meta = (ExpandEnumAsExecs = "Branches", Keywords = "C++"))
		 void DeallocateMiniDimension(const FVector& Location, ETGOR_FetchEnumeration& Branches);

	 /** Deallocates space for realdimensions */
	 UFUNCTION(BlueprintCallable, Category = "TGOR Dimension", Meta = (ExpandEnumAsExecs = "Branches", Keywords = "C++"))
		 void DeallocateRealDimension(const FVector& Location, ETGOR_FetchEnumeration& Branches);
	 	 
	 ////////////////////////////////////////////////////////////////////////////////////////////////////

	 /** Initiates resources */
	 UFUNCTION(BlueprintCallable, Category = "TGOR Dimension", Meta = (Keywords = "C++"))
		void AssembleContentManager(UWorld* World);

	 /** Initiates persistent level and spawns home level if required */
	 UFUNCTION(BlueprintCallable, Category = "TGOR Dimension", Meta = (Keywords = "C++"))
		 void AssemblePersistent(UTGOR_Dimension* Dimension, bool IsServer);
	 
	 ////////////////////////////////////////////////////////////////////////////////////////////////////

	 /** Create unique dimension name from an existing identifier */
	 UFUNCTION(BlueprintCallable, Category = "TGOR Dimension", Meta = (Keywords = "C++"))
		 FName GetUniqueIdentifier(const FString& Identifier);

	 /** Creates entry if doesn't exist yet */
	 UFUNCTION(BlueprintCallable, Category = "TGOR Dimension", Meta = (Keywords = "C++"))
		 bool AddIdentifier(TSubclassOf<UTGOR_Dimension> Class, const FName& Identifier);
		 bool AddIdentifier(UTGOR_Dimension* Content, const FName& Identifier);

	 /** Gets identifier of any dimension with a given dimension class and suffix, creates a new one otherwise */
	 UFUNCTION(BlueprintCallable, Category = "TGOR Dimension", Meta = (ExpandEnumAsExecs = "Branches", Keywords = "C++"))
		 FName EnsureIdentifier(TSubclassOf<UTGOR_Dimension> Class, const FString& Suffix, ETGOR_PushEnumeration& Branches);

	 /** Add or replace dimension */
	 UFUNCTION(BlueprintCallable, Category = "TGOR Dimension", Meta = (Keywords = "C++"))
		 bool SetIdentifier(UTGOR_Dimension* Content, const FName& Identifier);

	 /** Checks whether an entry exists */
	 UFUNCTION(BlueprintPure, Category = "TGOR Dimension", Meta = (Keywords = "C++"))
		 bool HasIdentifier(const FName& Identifier) const;

	 /** Check whether a slot is currently associated with a reality (or false if doesn't exist) */
	 UFUNCTION(BlueprintPure, Category = "TGOR Dimension", Meta = (Keywords = "C++"))
		 bool IsReality(const FName& Identifier) const;

	 /** Sets owner of a dimension */
	 UFUNCTION(BlueprintPure, Category = "TGOR Dimension", Meta = (Keywords = "C++"))
		 void SetOwner(const FName& Identifier, const FTGOR_DimensionIdentifier& Owner);

	 /** Gets owner of a dimension */
	 UFUNCTION(BlueprintPure, Category = "TGOR Dimension", Meta = (Keywords = "C++"))
		 FTGOR_DimensionIdentifier GetOwner(const FName& Identifier) const;

	 ////////////////////////////////////////////////////////////////////////////////////////////////////

	 /** Get identifier of persistent dimension */
	 UFUNCTION(BlueprintPure, Category = "TGOR Dimension", Meta = (Keywords = "C++"))
		 FName GetPersistentIdentifier() const;

	 /** Get identifier of first loaded reality dimension or persistent if there is none */
	 UFUNCTION(BlueprintPure, Category = "TGOR Dimension", Meta = (Keywords = "C++"))
		 FName GetRealityIdentifier() const;
	 
	 /** Gets dimension with given identifier or nullptr if not exists */
	 UFUNCTION(BlueprintCallable, Category = "TGOR Dimension", Meta = (ExpandEnumAsExecs = "Branches", Keywords = "C++"))
		 UTGOR_DimensionData* GetDimension(const FName& Identifier, ETGOR_FetchEnumeration& Branches);

	 /** Gets dimension with given identifier or nullptr if not exists */
	 UFUNCTION(BlueprintCallable, Category = "TGOR Dimension", Meta = (Keywords = "C++"))
		 UTGOR_Dimension* GetDimensionContent(const FName& Identifier) const;

	 /** Returns the location of a dimension in the world */
	 UFUNCTION(BlueprintCallable, Category = "TGOR Dimension", Meta = (Keywords = "C++"))
		 FVector GetDimensionLocation(const FName& Identifier) const;

	 /** Returns whether the dimension is open to be teleported to */
	 UFUNCTION(BlueprintCallable, Category = "TGOR Dimension", Meta = (Keywords = "C++"))
		 bool IsOpen(const FName& Identifier) const;

	 /** Returns whether the dimension is done loading or is in the process of loading */
	 UFUNCTION(BlueprintCallable, Category = "TGOR Dimension", Meta = (Keywords = "C++"))
		 bool HasBeenSpawned(const FName& Identifier) const;

	 /** Returns whether the dimension is done loading and is visible in the world */
	 UFUNCTION(BlueprintCallable, Category = "TGOR Dimension", Meta = (Keywords = "C++"))
		 bool HasFinishedLoading(const FName& Identifier) const;

	 /** Returns whether the dimension is done unloading */
	 UFUNCTION(BlueprintCallable, Category = "TGOR Dimension", Meta = (Keywords = "C++"))
		 bool HasFinishedUnLoading(const FName& Identifier) const;

	 /** Returns whether the dimension is currently unloading */
	 UFUNCTION(BlueprintCallable, Category = "TGOR Dimension", Meta = (Keywords = "C++"))
		 bool IsUnloading(const FName& Identifier) const;

	 /** Progress on loading a dimension */
	 UFUNCTION(BlueprintCallable, Category = "TGOR Dimension", Meta = (Keywords = "C++"))
		 bool ProgressLoadDimension(const FName& Identifier, bool IsServer);

	 /** Show cosmetics of a dimension */
	 UFUNCTION(BlueprintCallable, Category = "TGOR Dimension", Meta = (Keywords = "C++"))
		 bool SetDimensionCosmeticsHidden(const FName& Identifier, bool Hidden);

	 /** Closes a dimension (cannot be teleported to, usually done during unloading phase) */
	 UFUNCTION(BlueprintCallable, Category = "TGOR Dimension", Meta = (ExpandEnumAsExecs = "Branches", Keywords = "C++"))
		 void CloseDimension(const FName& Identifier, ETGOR_ComputeEnumeration& Branches);

	 /** Progress on unloading a dimension */
	 UFUNCTION(BlueprintCallable, Category = "TGOR Dimension", Meta = (Keywords = "C++"))
		 bool ProgressUnloadDimension(const FName& Identifier, bool IsServer);

	 ////////////////////////////////////////////////////////////////////////////////////////////////////
	 	 
	 /** Unload all realities (but not minidimensions) */
	 UFUNCTION(BlueprintCallable, Category = "TGOR Dimension", Meta = (Keywords = "C++"))
		 void UnloadAllRealities();

	 ////////////////////////////////////////////////////////////////////////////////////////////////////
protected:

	 /** Dimension register */
	 UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "TGOR Dimension")
		 TMap<FName, FTGOR_DimensionDataInstance> Dimensions;

	 /** Current locally active dimension (dimension player is inside of) */
	 UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "TGOR Dimension")
		 FName ActiveIdentifier;

	 /** Identifier of persistent dimension */
	 UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "TGOR Dimension")
		 FName PersistentIdentifier;

	 /** Volume tree root */
	 UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "TGOR Dimension")
		 ATGOR_PhysicsVolume* MainVolume;

	 ////////////////////////////////////////////////////////////////////////////////////////////////////
public:

	/** Get currently active dimension identifier */
	UFUNCTION(BlueprintPure, Category = "TGOR Dimension", Meta = (Keywords = "C++"))
		FName GetActiveIdentifier() const;

	/** Get main volume */
	UFUNCTION(BlueprintPure, Category = "TGOR Dimension", Meta = (Keywords = "C++"))
		ATGOR_PhysicsVolume* GetMainVolume() const;

	 /** Save all loaded dimensions */
	 UFUNCTION(BlueprintCallable, Category = "TGOR Dimension", Meta = (Keywords = "C++"))
		 void SwitchActiveDimension(const FName& Identifier);

	 /** Returns currently loaded dimensions */
	 UFUNCTION(BlueprintCallable, Category = "TGOR Dimension", Meta = (Keywords = "C++"))
		 void GetLoadedDimensions(TArray<FName>& Identifiers) const;

private:

	UPROPERTY()
		TWeakObjectPtr<UWorld> PersistentWorld;

	/** Height of reality sector */
	UPROPERTY()
		float RealityHeight;

	/** Height of minidimension sector */
	UPROPERTY()
		float MiniDimensionHeight;


	CTGOR_SpatialRoot RealDimensions = CTGOR_SpatialRoot(SPATIAL_LOCATION(-SPATIAL_MIN_BOTTOM), SPATIAL_SIZE(SPATIAL_BOUNDS + SPATIAL_MIN_BOTTOM), 2);
	CTGOR_SpatialRoot MiniDimensions = CTGOR_SpatialRoot(SPATIAL_LOCATION(-SPATIAL_BOUNDS), SPATIAL_SIZE(SPATIAL_MIN_BOTTOM), 8);
};
