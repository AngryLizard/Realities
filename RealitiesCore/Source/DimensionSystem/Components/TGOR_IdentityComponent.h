// The Gateway of Realities: Planes of Existence.

#pragma once

#include "DimensionSystem/TGOR_ActorInstance.h"
#include "DimensionSystem/TGOR_DimensionInstance.h"

#include "CoreSystem/Interfaces/TGOR_RegisterInterface.h"
#include "CoreSystem/Storage/TGOR_SaveInterface.h"
#include "CoreSystem/Components/TGOR_Component.h"
#include "DimensionSystem/Interfaces/TGOR_DimensionInterface.h"
#include "DimensionSystem/Interfaces/TGOR_SpawnerInterface.h"
#include "TGOR_IdentityComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FIdentityDelegate, UTGOR_Spawner*, Spawner);

/**
* TGOR_IdentityComponent gives an actor an identity inside a dimension and allows loading/storing data
*/
UCLASS(ClassGroup = (Custom), Blueprintable, meta = (BlueprintSpawnableComponent))
class DIMENSIONSYSTEM_API UTGOR_IdentityComponent : public UTGOR_Component, public ITGOR_DimensionInterface, public ITGOR_SaveInterface, public ITGOR_RegisterInterface
{
	GENERATED_BODY()

	friend class UTGOR_Loader;
	friend class UTGOR_WorldData;
	friend class UTGOR_DimensionData;
	friend class UTGOR_DimensionEditorFunctionLibrary;

public:	
	UTGOR_IdentityComponent();
	virtual void PostInitProperties() override;
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	void GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const override;

	virtual bool PreAssemble(UTGOR_DimensionData* Dimension) override;
	virtual bool Assemble(UTGOR_DimensionData* Dimension) override;
	virtual bool PostAssemble(UTGOR_DimensionData* Dimension) override;
	virtual TSet<UTGOR_CoreContent*> GetActiveContent_Implementation() const override;

	virtual void Write_Implementation(FTGOR_GroupWritePackage& Package, UTGOR_Singleton* Context) const override;
	virtual bool Read_Implementation(FTGOR_GroupReadPackage& Package, UTGOR_Singleton* Context) override;

	////////////////////////////////////////////////////////////////////////////////////////////////////
public:

	UPROPERTY(BlueprintAssignable, Category = "!TGOR Influence")
		FIdentityDelegate OnIdentityUpdate;

	/** Get current actor identifier */
	UFUNCTION(BlueprintPure, Category = "!TGOR Dimension", Meta = (Keywords = "C++"))
		int32 GetActorIdentifier() const;

	/** Get current actor spawner */
	UFUNCTION(BlueprintPure, Category = "!TGOR Dimension", Meta = (Keywords = "C++"))
		UTGOR_Spawner* GetActorSpawner() const;

	/** Set current spawner (can be done safely during PreAssembly) */
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "!TGOR Dimension", Meta = (Keywords = "C++"))
		void SetActorSpawner(UTGOR_Spawner* Spawner, bool ForceUpdate = false);

	/** Get current actor dimension */
	UFUNCTION(BlueprintPure, Category = "!TGOR Dimension", Meta = (Keywords = "C++"))
		UTGOR_Dimension* GetActorDimension() const;
	
	/** Switch whether this component is ignored during storage */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "!TGOR System")
		bool IgnoreStorage;

	/** Spawner to be used if actor is placed in the level */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "!TGOR System")
		TSubclassOf<UTGOR_Spawner> DefaultSpawner;

	/** Get spawner interface objects attached to this identity */
	UFUNCTION(BlueprintPure, Category = "!TGOR System", Meta = (Keywords = "C++"))
		TArray<UObject*> GetSpawnerObjects() const;

	////////////////////////////////////////////////////////////////////////////////////////////////////
public:

	/** Get current actor world identifier (INDEX_NONE if not tracked) */
	UFUNCTION(BlueprintPure, Category = "!TGOR Dimension", Meta = (Keywords = "C++"))
		int32 GetWorldIdentifier() const;

	/** Track this actor in world and return identifier (returns current identifier if already tracked) */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Dimension", Meta = (Keywords = "C++"))
		int32 CreateWorldIdentifier();

	////////////////////////////////////////////////////////////////////////////////////////////////////	
protected:
	
	/** Actor identification in current dimension */
	UPROPERTY(VisibleAnywhere, ReplicatedUsing = OnIdentityRepNotify, Category = "!TGOR Dimension")
		FTGOR_SpawnIdentity DimensionIdentity;

	UFUNCTION()
		void OnIdentityRepNotify(const FTGOR_SpawnIdentity& Old);

	/** Actor identification in current world (or -1 if not tracked) */
	UPROPERTY(VisibleAnywhere, Replicated, Category = "!TGOR Dimension")
		int32 WorldIdentity;

	/** Associated Spawn objects */
	UPROPERTY(BlueprintReadOnly, Category = "!TGOR System")
		TArray<TScriptInterface<ITGOR_SpawnerInterface>> SpawnerObjects;

	/** Associated Dimension objects */
	UPROPERTY(BlueprintReadOnly, Category = "!TGOR Dimension")
		TArray<TScriptInterface<ITGOR_DimensionInterface>> DimensionObjects;

	/** Associated Save objects */
	UPROPERTY(BlueprintReadOnly, Category = "!TGOR System")
		TMap<FString, TScriptInterface<ITGOR_SaveInterface>> SaveObjects;

	////////////////////////////////////////////////////////////////////////////////////////////////////
public:

	/** Get actor from connection */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Dimension", Meta = (WorldContext = "WorldContextObject", DeterminesOutputType = "Class", ExpandEnumAsExecs = "Branches", Keywords = "C++"))
		static AActor* GetActorFromSelection(UObject* WorldContextObject, const FTGOR_ConnectionSelection& Selection, TSubclassOf<AActor> Class, ETGOR_FetchEnumeration& Branches);

	/** Get component from connection */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Dimension", Meta = (WorldContext = "WorldContextObject", DeterminesOutputType = "Class", ExpandEnumAsExecs = "Branches", Keywords = "C++"))
		static UActorComponent* GetComponentFromSelection(UObject* WorldContextObject, const FTGOR_ConnectionSelection& Selection, TSubclassOf<UActorComponent> Class, ETGOR_FetchEnumeration& Branches);

};
