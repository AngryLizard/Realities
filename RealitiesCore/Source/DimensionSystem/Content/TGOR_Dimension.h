// The Gateway of Realities: Planes of Existence.

#pragma once

#include "CoreMinimal.h"
#include "Engine/LevelStreamingDynamic.h"

#include "../Regions/TGOR_MainRegionInstance.h"

#include "CoreSystem/Content/TGOR_CoreContent.h"
#include "TGOR_Dimension.generated.h"

class UTGOR_Loader;
class UTGOR_WorldData;


UENUM(BlueprintType)
enum class ETGOR_CosmeticInstruction : uint8
{
	ShowHide,
	Remove,
	DoNothing
};

/**
 * 
 */
UCLASS()
class DIMENSIONSYSTEM_API UTGOR_Dimension : public UTGOR_CoreContent
{
	GENERATED_BODY()

public:
	UTGOR_Dimension();
	virtual bool Validate_Implementation() override;

	////////////////////////////////////////////////////////////////////////////////////////////////////
public:

	/** Determine spawn location */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Dimension", Meta = (Keywords = "C++"))
		bool AllocateSpace(FVector& Location);

	/** Dereference spawn location */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Dimension", Meta = (Keywords = "C++"))
		bool DeallocateSpace(const FVector& Location);
	
	/** Stub from LevelStreaming */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Dimension", Meta = (Keywords = "C++"))
		static ULevelStreamingDynamic* LoadLevelInstanceBySoftObjectPtr(UObject* WorldContextObject, const TSoftObjectPtr<UWorld> Level, const FString& UniqueId, const FVector Location, const FRotator Rotation, bool& bOutSuccess);

	/** Stub from LevelStreaming */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Dimension", Meta = (Keywords = "C++"))
		static ULevelStreamingDynamic* LoadLevelInstance_Internal(UWorld* World, const FString& LongPackageName, const FString& UniqueId, const FVector Location, const FRotator Rotation, bool& bOutSuccess);

	/** Create level inside dimension */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Dimension", Meta = (Keywords = "C++"))
		UTGOR_DimensionData* Create(const FString& UniqueId, const FVector& Location);

	/** Load dimension after level has loaded */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Dimension", Meta = (Keywords = "C++"))
		bool Load(UTGOR_DimensionData* Dimension, const FString& Path, const TArray<int32>& Options);

	////////////////////////////////////////////////////////////////////////////////////////////////////

	/** Dimension size */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "!TGOR Dimension")
		FVector Bounds;

	/** Dimension bounds offset */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "!TGOR Dimension")
		FVector Offset;

	/** Whether this dimension is a Reality or MiniDimension */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "!TGOR Dimension")
		bool Reality;

	/** Whether this dimension gets deleted whithout storing to file */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "!TGOR Dimension")
		bool Volatile;

	/** Countdown for when dimensions gets force unloaded even with players inside */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "!TGOR Dimension")
		float Lifeline;

	////////////////////////////////////////////////////////////////////////////////////////////////////
protected:

	/** Cosmetic types shouldn't be replicated, will get hidden for any dimensions that the local player is not inside */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "!TGOR Dimension")
		TArray<TSubclassOf<AActor>> CosmeticTypes;

	////////////////////////////////////////////////////////////////////////////////////////////////////
public:

	/** Linked map */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "!TGOR Dimension")
		TSoftObjectPtr<UWorld> World;

	/** Available connections in this dimension */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "!TGOR Dimension")
		TArray<FName> PublicConnections;

	/** Called after level has finished loading and before Dimension gets assembled */
	UFUNCTION(BlueprintImplementableEvent, Category = "!TGOR Dimension", Meta = (Keywords = "C++"))
		void PreAssemble(UTGOR_DimensionData* Dimension);

	/** Called after loaders have been executed and before actors get assembled */
	UFUNCTION(BlueprintImplementableEvent, Category = "!TGOR Dimension", Meta = (Keywords = "C++"))
		void Assemble(UTGOR_DimensionData* Dimension);

	/** Called after Dimension has fully loaded */
	UFUNCTION(BlueprintImplementableEvent, Category = "!TGOR Dimension", Meta = (Keywords = "C++"))
		void PostAssemble(UTGOR_DimensionData* Dimension);

	/** Called for every filtered actor that is set hidden/shown */
	UFUNCTION(BlueprintImplementableEvent, Category = "!TGOR Dimension", Meta = (Keywords = "C++"))
		ETGOR_CosmeticInstruction OnCosmeticChange(AActor* Actor, bool Hidden);

	/** Set cosmetic actors shown/hidden */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Dimension", Meta = (Keywords = "C++"))
		void SetCosmeticHidden(UTGOR_DimensionData* Dimension, bool Hidden);

	/** Set actor cull distance to dimension extend. */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Dimension", Meta = (Keywords = "C++"))
		void SetCullDistance(AActor* Actor) const;

	////////////////////////////////////////////////////////////////////////////////////////////////////
public:

};
