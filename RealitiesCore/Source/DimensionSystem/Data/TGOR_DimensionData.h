// The Gateway of Realities: Planes of Existence.

#pragma once

#include "CoreMinimal.h"

#include "DimensionSystem/Regions/Voronoi/SearchActor.h"

#include "DimensionSystem/TGOR_DimensionInstance.h"
#include "DimensionSystem/TGOR_ActorInstance.h"

#include "CoreSystem/Data/TGOR_Data.h"
#include "TGOR_DimensionData.generated.h"

#define DIMENSION_ERR "Dimension not found or not loaded"
#define DIMENSION_CHK if (!DimensionData.IsValid() || !IsValid(DimensionData.Get()->GetMainRegion())){ERROR(DIMENSION_ERR, Error)}
#define DIMENSION_RETCHK(T) if (!DimensionData.IsValid() || !IsValid(DimensionData.Get()->GetMainRegion())){ERRET(DIMENSION_ERR, Error, T)}

#define DIMENSIONBASE_ERR "DimensionData not found, make sure actor has an IdentityComponent"
#define DIMENSIONBASE_CHK if (!DimensionData.IsValid()){ERROR(DIMENSIONBASE_ERR, Error)}
#define DIMENSIONBASE_RETCHK(T) if (!DimensionData.IsValid()){ERRET(DIMENSIONBASE_ERR, Error, T)}

#define REGIONCONTROLLER_ERR "MainRegion not found"
#define REGIONCONTROLLER_CHK if (!IsValid(MainRegion)){ERROR(REGIONCONTROLLER_ERR, Error)}
#define REGIONCONTROLLER_RETCHK(T) if (!IsValid(MainRegion)){ERRET(REGIONCONTROLLER_ERR, Error, T)}

class ATGOR_GateActor;
class ATGOR_DimensionActor;
class ATGOR_MainRegionActor;
class UTGOR_DimensionComponent;
class UTGOR_ConnectionComponent;
class UTGOR_IdentityComponent;
class ATGOR_PhysicsVolume;

/**
 * 
 */
UCLASS(BlueprintType)
class DIMENSIONSYSTEM_API UTGOR_DimensionData : public UTGOR_Data
{
	GENERATED_BODY()

	friend class UTGOR_Dimension;
	friend class UTGOR_WorldData;
	friend class ITGOR_DimensionInterface;

	friend class ATGOR_LevelVolume;
	friend class ATGOR_MainRegionActor;
	friend class ATGOR_GateActor;

public:
	UTGOR_DimensionData();
	UWorld* GetWorld() const;

	virtual void Write_Implementation(FTGOR_GroupWritePackage& Package, UTGOR_Singleton* Context) const override;
	virtual bool Read_Implementation(FTGOR_GroupReadPackage& Package, UTGOR_Singleton* Context) override;
	
	////////////////////////////////////////////////////////////////////////////////////////////////////
protected:

	/** Spawn an identity in this dimension */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Dimension", Meta = (Keywords = "C++"))
		UTGOR_IdentityComponent* SpawnIdentity(UTGOR_Spawner* Spawner, const FVector& Location, const FRotator& Rotation);

	/** True if dimension was already assembled */
	UPROPERTY()
		bool DimensionAssembled;

	/** True if actors were already assembled */
	UPROPERTY()
		bool ActorsAssembled;

	////////////////////////////////////////////////////////////////////////////////////////////////////
public:

	/** Find whether dimensions have been assembled */
	UFUNCTION(BlueprintPure, Category = "!TGOR Dimension|Internal", Meta = (Keywords = "C++"))
		bool IsDimensionAssembled() const;

	/** Find whether actors have been assembled */
	UFUNCTION(BlueprintPure, Category = "!TGOR Dimension|Internal", Meta = (Keywords = "C++"))
		bool IsActorsAssembled() const;

	////////////////////////////////////////////////////////////////////////////////////////////////////
public:

		/** Constructs a search actor */
		bool Construct(Voronoi::SearchActor& Actor);

	////////////////////////////////////////////////////////////////////////////////////////////////////

	/** Assembles all dimensions */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Dimension|Internal", Meta = (Keywords = "C++"))
		void AssembleDimension();

	/** Assembles all actors */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Dimension|Internal", Meta = (Keywords = "C++"))
		void AssembleActors();

	////////////////////////////////////////////////////////////////////////////////////////////////////
protected:

	/** Minidimensions connected if this is a reality. */
	UPROPERTY(BlueprintReadOnly, Category = "!TGOR Dimension")
		TArray<FName> Children;

	/** Current Gate */
	UPROPERTY()
		ATGOR_GateActor* MainGate;

	/** Current region controller */
	UPROPERTY()
		ATGOR_MainRegionActor* MainRegion;

	/** Associated levelstreaming (doesn't exist for persistent level) */
	UPROPERTY()
		ULevelStreaming* LevelStreaming;

	/** This dimension's identifier */
	UPROPERTY(EditAnywhere, Category = "!TGOR Dimension")
		FName Identifier;

	/** Associated level */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "!TGOR Dimension")
		ULevel* LoadedLevel;

	/** This dimension's origin location */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "!TGOR Dimension")
		FVector Origin;

	////////////////////////////////////////////////////////////////////////////////////////////////////

	/** This dimension't main volume */
	UPROPERTY()
		ATGOR_LevelVolume* LevelVolume;

	/** Dimension connections */
	UPROPERTY()
		TMap<FName, UTGOR_ConnectionComponent*> Connections;

	/** Associated Dimension objects mapped to identifiers */
	UPROPERTY()
		TMap<int32, FTGOR_SpawnInstance> DimensionObjects;

	/** Objects that weren't spawned that got removed */
	UPROPERTY()
		TArray<int32> DeletedObjects;

	////////////////////////////////////////////////////////////////////////////////////////////////////
public:

	/** Get dimension origin in world space */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Dimension", Meta = (Keywords = "C++"))
		FVector GetDimensionOrigin() const;

	/** Get actor identifier randomly (not necessarily unique). */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Dimension|Internal", Meta = (Keywords = "C++"))
		static int32 GetRandomActorIdentifier();

	/** Get actor identifier not used by any other actor yet. */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Dimension|Internal", Meta = (Keywords = "C++"))
		int32 GetUniqueActorIdentifier() const;

	/** Get dimension object from identifier. */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Dimension|Internal", Meta = (ExpandEnumAsExecs = "Branches", Keywords = "C++"))
		UTGOR_IdentityComponent* GetDimensionObject(int32 ActorIdentifier, ETGOR_FetchEnumeration& Branches) const;

	/** Spawns a dimension object, adds and assembles it */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Dimension|Internal", Meta = (ExpandEnumAsExecs = "Branches", Keywords = "C++"))
		UTGOR_IdentityComponent* AddDimensionObject(int32 ActorIdentifier, UTGOR_Spawner* Spawner, const FVector& Location, const FRotator& Rotation, ETGOR_FetchEnumeration& Branches);

	/** Registers already existing dimension object, adds and assembles it */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Dimension|Internal", Meta = (Keywords = "C++"))
		void RegisterDimensionObject(UTGOR_IdentityComponent* DimensionObject);

	/** Moves a dimension object to another dimension */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Dimension|Internal", Meta = (Keywords = "C++"))
		void SwapDimensionObject(int32 ActorIdentifier, UTGOR_DimensionData* Other);

	/** Removes a dimension object from the pool, adds map-actors to the removal list (does not actually despawn the actor) */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Dimension|Internal", Meta = (Keywords = "C++"))
		void RemoveDimensionObject(int32 ActorIdentifier);


	/** Get the main level volume */
	UFUNCTION(BlueprintPure, Category = "!TGOR Dimension", Meta = (Keywords = "C++"))
		ATGOR_LevelVolume* GetLevelVolume() const;

	/** Get main gate */
	UFUNCTION(BlueprintPure, Category = "!TGOR Dimension", Meta = (Keywords = "C++"))
		ATGOR_GateActor* GetMainGate() const;

	/** Get main region */
	UFUNCTION(BlueprintPure, Category = "!TGOR Dimension", Meta = (Keywords = "C++"))
		ATGOR_MainRegionActor* GetMainRegion() const;

	/** Get own dimension */
	UFUNCTION(BlueprintPure, Category = "!TGOR Dimension", Meta = (Keywords = "C++"))
		FName GetIdentifier() const;


	/** Adds a connection */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Dimension", Meta = (Keywords = "C++"))
		void AddConnection(const FName& ConnectionName, UTGOR_ConnectionComponent* ConnectionComponent);

	/** Get a connection */
	UFUNCTION(BlueprintPure, BlueprintAuthorityOnly, Category = "!TGOR Dimension", Meta = (Keywords = "C++"))
		UTGOR_ConnectionComponent* GetConnection(const FName& ConnectionName) const;

	/** Return connections of given type */
	UFUNCTION(BlueprintPure, BlueprintAuthorityOnly, Category = "!TGOR Dimension", Meta = (DeterminesOutputType = "Class", Keywords = "C++"))
		UTGOR_ConnectionComponent* GetConnectionOfType(const FName& ConnectionName, TSubclassOf<UTGOR_ConnectionComponent> Class) const;

	template<typename T> TArray<T*> GetCOfType(const FName& ConnectionName, TSubclassOf<T> Type) const
	{
		return Cast<T>(GetConnectionOfType(ConnectionName, Type));
	}
	template<typename T> TArray<T*> GetCOfType(const FName& ConnectionName) const
	{
		return GetCListOfType<T>(T::StaticClass());
	}

	/** Return connection of given type */
	UFUNCTION(BlueprintPure, BlueprintAuthorityOnly, Category = "!TGOR Dimension", Meta = (DeterminesOutputType = "Class", Keywords = "C++"))
		TArray<UTGOR_ConnectionComponent*> GetConnectionsOfType(TSubclassOf<UTGOR_ConnectionComponent> Class) const;

	template<typename T> TArray<T*> GetCListOfType(TSubclassOf<T> Type) const
	{
		TArray<T*> Output;
		TArray<UTGOR_ConnectionComponent*> List = GetConnectionsOfType(Type);
		for (UTGOR_ConnectionComponent* Connection : List)
		{
			Output.Emplace(Cast<T>(Connection));
		}
		return Output;
	}
	template<typename T> TArray<T*> GetCListOfType() const
	{
		return GetCListOfType<T>(T::StaticClass());
	}


	////////////////////////////////////////////////////////////////////////////////////////////////////

};