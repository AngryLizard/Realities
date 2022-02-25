// The Gateway of Realities: Planes of Existence.

#pragma once

#include "CoreMinimal.h"
#include "Realities/Utility/TGOR_CustomEnumerations.h"

#include "Engine/StaticMesh.h"
#include "Engine/StaticMeshActor.h"
#include "Components/StaticMeshComponent.h"

#include "Realities/Mod/Loaders/TGOR_Loader.h"
#include "TGOR_StaticMeshLoader.generated.h"


////////////////////////////////////////////// STRUCTS //////////////////////////////////////////////////////

/**
* 
*/
USTRUCT(BlueprintType)
struct REALITIES_API FTGOR_StaticMeshLoaderInstance
{
	GENERATED_USTRUCT_BODY()
		FTGOR_StaticMeshLoaderInstance();
	FTGOR_StaticMeshLoaderInstance(const FTransform& Transform);

	////////////////////////////////////////////////////////////////////////////////////////////////////

	UPROPERTY(EditDefaultsOnly)
		TArray<FTransform> Transforms;
};

/**
*
*/
USTRUCT(BlueprintType)
struct REALITIES_API FTGOR_StaticMeshLoaderActors
{
	GENERATED_USTRUCT_BODY()
		FTGOR_StaticMeshLoaderActors();
	FTGOR_StaticMeshLoaderActors(AActor* Actor);

	////////////////////////////////////////////////////////////////////////////////////////////////////

	UPROPERTY(EditDefaultsOnly)
		TArray<AActor*> Actors;
};

/**
*
*/
USTRUCT(BlueprintType)
struct REALITIES_API FTGOR_StaticMeshLoaderIndices
{
	GENERATED_USTRUCT_BODY()
		FTGOR_StaticMeshLoaderIndices();
	FTGOR_StaticMeshLoaderIndices(int32 Index);

	////////////////////////////////////////////////////////////////////////////////////////////////////

	UPROPERTY(EditDefaultsOnly)
		TArray<int32> Indices;
};


/**
*
*/
UCLASS()
class REALITIES_API UTGOR_StaticMeshLoader : public UTGOR_Loader
{
	GENERATED_BODY()

public:
	UTGOR_StaticMeshLoader();

	virtual bool CanLoad(UWorld* World, const FTransform& Origin, UTGOR_Dimension* Dimension) const override;
	virtual bool Load(UWorld* World, const FTransform& Origin, UTGOR_Dimension* Dimension) override;
	virtual bool IsLoaded(UWorld* World, const FTransform& Origin, UTGOR_Dimension* Dimension) const override;
	virtual bool Unload(UWorld* World, const FTransform& Origin, UTGOR_Dimension* Dimension) override;

	////////////////////////////////////////////////////////////////////////////////////////////////////
protected:

	/** Looks for building blocks in world, faster than O(n^2) search by exploiting the map structure and only iterating through all actors once. */
	UFUNCTION(BlueprintCallable, Category = "TGOR Dimension", Meta = (Keywords = "C++"))
		void DetectBuildingBlocks(UWorld * World, const FTransform & Origin, TMap<UStaticMesh*, FTGOR_StaticMeshLoaderActors>& Loaded, TMap<UStaticMesh*, FTGOR_StaticMeshLoaderIndices>& Unloaded) const;


	////////////////////////////////////////////////////////////////////////////////////////////////////
public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TGOR Dimension", Meta = (Keywords = "C++"))
		TMap<UStaticMesh*, FTGOR_StaticMeshLoaderInstance> BuildingBlocks;

	/** Adds a building block if not already exists at the given transform */
	UFUNCTION(BlueprintCallable, Category = "TGOR Dimension", Meta = (Keywords = "C++"))
		bool AddBuildingBlock(UStaticMesh* Mesh, const FTransform& Transform);

	/** Move a building actor given a mesh and transform to a new transform */
	UFUNCTION(BlueprintCallable, Category = "TGOR Dimension", Meta = (Keywords = "C++"))
		bool MoveBuildingBlock(UStaticMesh* Mesh, const FTransform& OldTransform, const FTransform& NewTransform);

};