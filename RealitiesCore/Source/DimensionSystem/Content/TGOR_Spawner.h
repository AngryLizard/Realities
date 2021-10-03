// The Gateway of Realities: Planes of Existence.

#pragma once

#include "GameFramework/Actor.h"
#include "RealitiesUGC/Mod/TGOR_Insertions.h"

#include "DimensionSystem/TGOR_ActorInstance.h"

#include "DimensionSystem/Content/TGOR_SpawnModule.h"
#include "TGOR_Spawner.generated.h"

UCLASS(Blueprintable)
class DIMENSIONSYSTEM_API UTGOR_Spawner : public UTGOR_SpawnModule
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

	/** Returns first spawn module content matching type */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Content", Meta = (DeterminesOutputType = "Type", Keywords = "C++"))
		UTGOR_SpawnModule* GetModuleFromType(TSubclassOf<UTGOR_SpawnModule> Type) const;
	template<typename T>
	T* GetMFromType(TSubclassOf<T> Type) const
	{
		return Cast<T>(GetModuleFromType(Type));
	}
	template<typename T>
	T* GetMFromType() const
	{
		return GetMFromType<T>(T::StaticClass());
	}

	/** Returns spawn module content list matching type */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Content", Meta = (DeterminesOutputType = "Type", Keywords = "C++"))
		TArray<UTGOR_SpawnModule*> GetModuleListFromType(const TArray<TSubclassOf<UTGOR_SpawnModule>>& Types) const;
	template<typename T, typename S>
	TArray<TObjectPtr<T>> GetMListFromType(const TArray<TSubclassOf<S>>& Types) const
	{
		SINGLETON_RETCHK(TArray<TObjectPtr<T>>());
		UTGOR_ContentManager* ContentManager = Singleton->GetContentManager();

		TSet<TObjectPtr<T>> Out;
		for (TSubclassOf<S> Type : Types)
		{
			Out.Add(ContentManager->GetTFromType<T>(*Type));
		}
		return Out.Array();
	}

	/** Primitive of this spawner
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "!TGOR Insertion")
		TArray<TSubclassOf<UTGOR_SpawnModule>> ModuleInsertions;
	DECL_INSERTION_REQUIRED(ModuleInsertions);


	virtual void MoveInsertion(UTGOR_Content* Insertion, ETGOR_InsertionActionEnumeration Action, bool& Success) override; */
};

