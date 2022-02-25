///////////////////////////////////////////////////
// The Gateway of Realities: Planes of Existence //
// Copyright 2016-2017                           //
///////////////////////////////////////////////////

#pragma once

#include "Realities/Utility/Datastructures/TGOR_Time.h"

#include "Realities/Interfaces/TGOR_RegisterInterface.h"
#include "Realities/Components/TGOR_Component.h"
#include "TGOR_StatComponent.generated.h"


////////////////////////////////////////////// STRUCT //////////////////////////////////////////////////////

USTRUCT(BlueprintType)
struct FTGOR_RegenerationHandle
{
	GENERATED_USTRUCT_BODY()

	/** Register to poll for activity */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "TGOR Vitals")
		TScriptInterface<ITGOR_RegisterInterface> Register;

	/** Regen rate */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "TGOR Vitals")
		float Regen;
};

USTRUCT(BlueprintType)
struct FTGOR_StatState
{
	GENERATED_USTRUCT_BODY()

	/** Time when regeneration starts */
	UPROPERTY(BlueprintReadWrite, Category = "TGOR Vitals")
		FTGOR_Time RegenTime;

	/** Current stat value. */
	UPROPERTY(BlueprintReadWrite, Category = "TGOR Vitals")
		float Stat;
};


DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FStatChangeDelegate, float, Amount);

/**
* UTGOR_HealthComponent adds health functionality to an actor.
*/
UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class REALITIES_API UTGOR_StatComponent : public UTGOR_Component
{
	GENERATED_BODY()

public:	
	UTGOR_StatComponent();
	virtual void BeginPlay() override;
	virtual void GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const override;

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	

	////////////////////////////////////////////////////////////////////////////////////////////////////
protected:

	UPROPERTY(BlueprintAssignable, Category = "TGOR Vitals")
		FStatChangeDelegate OnStatChanged;

	////////////////////////////////////////////////////////////////////////////////////////////////////

	/** Current stat value. */
	UPROPERTY(ReplicatedUsing = ReplicatedStat, BlueprintReadOnly, Category = "TGOR Vitals")
		FTGOR_StatState State;

	/** Replicate stat value */
	UFUNCTION()
		void ReplicatedStat(const FTGOR_StatState& Old);

	////////////////////////////////////////////////////////////////////////////////////////////////////

	/** Returns total regen rate. */
	UFUNCTION(BlueprintCallable, Category = "TGOR Creature", Meta = (Keywords = "C++"))
		float GetTotalRegen() const;

	/** Register regeneration to go active. */
	UFUNCTION(BlueprintCallable, Category = "TGOR Creature", Meta = (Keywords = "C++"))
		void RegisterHandle(TScriptInterface<ITGOR_RegisterInterface> Register, UTGOR_Content* Content, float Regen);

	/** Unregister manually. */
	UFUNCTION(BlueprintCallable, Category = "TGOR Creature", Meta = (Keywords = "C++"))
		void UnRegisterHandle(UTGOR_Content* Content);

	////////////////////////////////////////////////////////////////////////////////////////////////////
protected:

	/** Handles for automatic bodypart loader */
	UPROPERTY(BlueprintReadOnly, Category = "TGOR Bodypart")
		TMap<UTGOR_Content*, FTGOR_RegenerationHandle> Handles;

	////////////////////////////////////////////////////////////////////////////////////////////////////
protected:

	/** Timeout after stat change until regen starts. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "TGOR Vitals")
		float RegenTimeout;

	/** Maximum stat value. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "TGOR Vitals")
		float StatMax;

	////////////////////////////////////////////////////////////////////////////////////////////////////
public:

	/** Get the current ratio of stat to maximum [0,1]. */
	UFUNCTION(BlueprintPure, Category = "TGOR Vitals", Meta = (Keywords = "C++"))
		float GetStatRatio();

	/** Set the actor's maximum health value. */
	UFUNCTION(BlueprintCallable, Category = "TGOR Vitals", Meta = (Keywords = "C++"))
		void SetStatMax(float Amount);

	/** Change the actor's current stat by the specified amount. */
	UFUNCTION(BlueprintCallable, Category = "TGOR Vitals", Meta = (Keywords = "C++"))
		virtual void ChangeStat(float Amount);

	/** General switch enabling/disabling regen regardless of handles. */
	UFUNCTION(BlueprintCallable, Category = "TGOR Vitals", Meta = (Keywords = "C++"))
		virtual bool CanRegen() const;

	////////////////////////////////////////////////////////////////////////////////////////////////////

	private:
};
