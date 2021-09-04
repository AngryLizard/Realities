///////////////////////////////////////////////////
// The Gateway of Realities: Planes of Existence //
// Copyright 2016-2017                           //
///////////////////////////////////////////////////

#pragma once

#include "RealitiesUtility/Structures/TGOR_Time.h"
#include "RealitiesUtility/Structures/TGOR_Percentage.h"
#include "AttributeSystem/TGOR_AttributeInstance.h"
#include "CoreSystem/Storage/TGOR_Serialisation.h"

#include "CoreSystem/Storage/TGOR_SaveInterface.h"
#include "CoreSystem/Interfaces/TGOR_RegisterInterface.h"
#include "DimensionSystem/Interfaces/TGOR_SpawnerInterface.h"
#include "AttributeSystem/Interfaces/TGOR_AttributeInterface.h"
#include "CoreSystem/Components/TGOR_Component.h"
#include "DimensionSystem/Content/TGOR_Energy.h"

#include "TGOR_StatComponent.generated.h"

////////////////////////////////////////////// DECL //////////////////////////////////////////////////////

class UTGOR_Stat;
class UTGOR_Effect;
class UTGOR_Matter;
class UTGOR_Segment;
class UTGOR_Response;
class UTGOR_Simulation;

////////////////////////////////////////////// STRUCT //////////////////////////////////////////////////////


USTRUCT(BlueprintType)
struct FTGOR_StatProperties
{
	GENERATED_USTRUCT_BODY()

	/** Whether the buffer has been updated externally. */
	UPROPERTY(BlueprintReadWrite, Category = "!TGOR Stats")
		bool HadBufferUpdate;

	/** Last time this stat got updated. */
	UPROPERTY(BlueprintReadWrite, Category = "!TGOR Stats")
		FTGOR_Time LastUpdate;

	/** Effect content to query from. */
	UPROPERTY(BlueprintReadWrite, Category = "!TGOR Stats")
		TArray<UTGOR_Effect*> EffectTable;

	/** Response content to query from. */
	UPROPERTY(BlueprintReadWrite, Category = "!TGOR Stats")
		TArray<UTGOR_Response*> ResponseTable;
};

USTRUCT(BlueprintType)
struct FTGOR_StatOutput
{
	GENERATED_USTRUCT_BODY()

	/** Stat content. */
	UPROPERTY(BlueprintReadWrite, Category = "!TGOR Stats")
		UTGOR_Stat* Stat;

	/** Current stat value in [0, 1]. */
	UPROPERTY(BlueprintReadWrite, Category = "!TGOR Stats")
		float Current;

	/** Current stat value in [0, 1] with current buffer state. */
	UPROPERTY(BlueprintReadWrite, Category = "!TGOR Stats")
		float Buffered;

	/** Whether this given stat has had external stat changes. */
	UPROPERTY(BlueprintReadWrite, Category = "!TGOR Stats")
		bool HadBufferUpdate;

	/** Currently active effects. */
	UPROPERTY(BlueprintReadWrite, Category = "!TGOR Stats")
		TSet<UTGOR_Effect*> Active;

	/** Currently inactive effects. */
	UPROPERTY(BlueprintReadWrite, Category = "!TGOR Stats")
		TSet<UTGOR_Effect*> Inactive;
};

USTRUCT(BlueprintType)
struct FTGOR_StatState
{
	GENERATED_USTRUCT_BODY()
	SERIALISE_INIT_HEADER;

	void Write(FTGOR_GroupWritePackage& Package, UTGOR_Singleton* Context) const;
	void Read(FTGOR_GroupReadPackage& Package, UTGOR_Singleton* Context);
	void Send(FTGOR_NetworkWritePackage& Package, UTGOR_Singleton* Context) const;
	void Recv(FTGOR_NetworkReadPackage& Package, UTGOR_Singleton* Context);

	/** Current stat values. */
	UPROPERTY(BlueprintReadWrite, Category = "!TGOR Stats")
		TMap<UTGOR_Stat*, FTGOR_Percentage> Stats;
};

template<>
struct TStructOpsTypeTraits<FTGOR_StatState> : public TStructOpsTypeTraitsBase2<FTGOR_StatState>
{
	enum { WithNetSerializer = true };
};


DECLARE_DYNAMIC_MULTICAST_DELEGATE(FStatChangeDelegate);

/**
* UTGOR_HealthComponent adds health functionality to an actor.
*/
UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class SIMULATIONSYSTEM_API UTGOR_StatComponent : public UTGOR_Component, public ITGOR_SaveInterface, public ITGOR_SpawnerInterface, public ITGOR_AttributeInterface, public ITGOR_RegisterInterface
{
	GENERATED_BODY()

public:	
	UTGOR_StatComponent();
	virtual void BeginPlay() override;
	virtual void GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const override;

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	virtual void UpdateAttributes_Implementation(const UTGOR_AttributeComponent* Component) override;
	virtual float GetAttribute_Implementation(UTGOR_Attribute* Attribute, float Default) const override;
	virtual bool IsContentActive_Implementation(UTGOR_CoreContent* Content) const override;
	virtual TSet<UTGOR_CoreContent*> GetActiveContent_Implementation() const override;
	virtual void UpdateContent_Implementation(UTGOR_Spawner* Spawner) override;

	////////////////////////////////////////////////////////////////////////////////////////////////////
public:

	/** Simulation type this stat spawns with. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "!TGOR Stats")
		TSubclassOf<UTGOR_Simulation> TargetSimulation;

	////////////////////////////////////////////////////////////////////////////////////////////////////
protected:

	UPROPERTY(BlueprintAssignable, Category = "!TGOR Stats")
		FStatChangeDelegate OnStatChanged;

	////////////////////////////////////////////////////////////////////////////////////////////////////

	/** Current stat value. */
	UPROPERTY(ReplicatedUsing = ReplicatedStat, BlueprintReadOnly, Category = "!TGOR Stats")
		FTGOR_StatState State;

	/** Replicate stat value */
	UFUNCTION()
		void ReplicatedStat(const FTGOR_StatState& Old);

	/** Update stat values from buffer. */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Stats", Meta = (Keywords = "C++"))
		void UpdateStatValues();

	/** Update effects */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Stats", Meta = (Keywords = "C++"))
		void UpdateEffects();

	////////////////////////////////////////////////////////////////////////////////////////////////////
protected:

	/** Current stat attributes */
	UPROPERTY()
		FTGOR_AttributeInstance Attributes;

	////////////////////////////////////////////////////////////////////////////////////////////////////
public:

	/** Get the current state of all stats. */
	UFUNCTION(BlueprintPure, Category = "!TGOR Stats", Meta = (Keywords = "C++"))
		TArray<FTGOR_StatOutput> GetStats() const;

	/** Get the current state of a given stats. */
	UFUNCTION(BlueprintPure, Category = "!TGOR Stats", Meta = (Keywords = "C++"))
		float GetStatValue(UTGOR_Stat* Stat) const;

	/** Add/Subtract stat value (negative values allowed), returns residual if out of range. */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Stats", Meta = (Keywords = "C++"))
		void AddStatValue(UTGOR_Stat* Stat, float Value);

	////////////////////////////////////////////////////////////////////////////////////////////////////
public:

	/** Process energy of a given quantity from a given Segment, return residual quantity. */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Stats", Meta = (Keywords = "C++"))
		int32 ProcessEnergy(UTGOR_Segment* Segment, UTGOR_Energy* Energy, int32 Quantity);

	/** Process matter composition from a given Segment, return residual composition. */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Stats", Meta = (Keywords = "C++"))
		TMap<UTGOR_Matter*, int32> ProcessMatter(UTGOR_Segment* Segment, const TMap<UTGOR_Matter*, int32>& Composition);

	////////////////////////////////////////////////////////////////////////////////////////////////////
private:

	/** Currently buffered stat values. */
	UPROPERTY()
		TMap<UTGOR_Stat*, float> StatBuffer;

	/** Currently available stat properties. */
	UPROPERTY()
		TMap<UTGOR_Stat*, FTGOR_StatProperties> StatTable;

	/** Currently active effects */
	UPROPERTY()
		TSet<UTGOR_CoreContent*> ActiveEffects;
};
