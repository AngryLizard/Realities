// The Gateway of Realities: Planes of Existence.

#pragma once

#include "GameFramework/Actor.h"
#include "RealitiesUGC/Mod/TGOR_Insertions.h"

#include "DimensionSystem/TGOR_ActorInstance.h"

#include "CoreSystem/Content/TGOR_CoreContent.h"
#include "TGOR_Spawner.generated.h"

class UTGOR_SpawnModule;

UCLASS(Blueprintable)
class DIMENSIONSYSTEM_API UTGOR_Spawner : public UTGOR_CoreContent
{
	GENERATED_BODY()

public:
	UTGOR_Spawner();
	virtual bool Validate_Implementation() override;

	/** Class this spawnable spawns as */
	UFUNCTION(BlueprintNativeEvent, BlueprintPure, Category = "!TGOR Creature", Meta = (Keywords = "C++"))
		TSubclassOf<AActor> GetSpawnClass() const;
		virtual TSubclassOf<AActor> GetSpawnClass_Implementation() const;

	////////////////////////////////////////////////////////////////////////////////////////////////////
public:

	/** Returns first content matching type */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Content", Meta = (DeterminesOutputType = "Type", Keywords = "C++"))
		UTGOR_Content* GetModuleFromType(TSubclassOf<UTGOR_SpawnModule> Type) const;
	template<typename T>
	T* GetMFromType(TSubclassOf<T> Type) const
	{
		return(Cast<T>(GetModuleFromType(Type)));
	}
	template<typename T>
	T* GetMFromType() const
	{
		return(GetMFromType<T>(T::StaticClass()));
	}


	/** Primitive of this spawner
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "!TGOR Insertion")
		TArray<TSubclassOf<UTGOR_SpawnModule>> ModuleInsertions;
	DECL_INSERTION_REQUIRED(ModuleInsertions);


	virtual void MoveInsertion(UTGOR_Content* Insertion, ETGOR_InsertionActionEnumeration Action, bool& Success) override; */
};

