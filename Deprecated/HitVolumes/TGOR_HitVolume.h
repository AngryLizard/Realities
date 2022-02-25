// TGOR (C) // CHECKED //
#pragma once

#include "Base/Instances/Dimension/TGOR_ElementInstance.h"
#include "Utility/Datastructures/TGOR_Time.h"

#include "CoreMinimal.h"
#include "Actors/TGOR_Actor.h"
#include "TGOR_HitVolume.generated.h"

/**
* ATGOR_HitVolume defines actors that are used for damage detection
*/
UCLASS(Blueprintable)
class REALITIES_API ATGOR_HitVolume : public ATGOR_Actor
{
	GENERATED_BODY()
	
public:

	////////////////////////////////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////// ENGINE ////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////////////////////////////

	ATGOR_HitVolume(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
	virtual void Tick(float DeltaSeconds) override;

	//////////////////////////////////////////////// IMPLEMENTABLES /////////////////////////////////////////

	/** Firsttime setup */
	UFUNCTION(BlueprintCallable, Category = "TGOR HitVolume", Meta = (Keywords = "C++"))
		virtual void Build(const FTGOR_ElementInstance& State, const FVector& Focus);

	/** Moves this actor according to a timestamp and current aimfocus, returns hit results */
	UFUNCTION(BlueprintCallable, Category = "TGOR HitVolume", Meta = (Keywords = "C++"))
		virtual void Update(const FTGOR_ElementInstance& State, const FVector& Focus);

	/** Tick function, remove if false */
	UFUNCTION(BlueprintCallable, Category = "TGOR HitVolume", Meta = (Keywords = "C++"))
		virtual bool VolumeTick(float DeltaSeconds);

	/** Terminates this volume to a given time */
	UFUNCTION(BlueprintCallable, Category = "TGOR HitVolume", Meta = (Keywords = "C++"))
		virtual void Terminate(FTGOR_Time TerminationTimestamp);

	////////////////////////////////////////////// COMPONENTS //////////////////////////////////////////////////////
private:

	/** */
	
	////////////////////////////////////////////////////////////////////////////////////////////////////
public:

	/** Initialises Hitvolume parameters  */
	UFUNCTION(BlueprintCallable, Category = "TGOR HitVolume", Meta = (Keywords = "C++"))
		void Init(const TArray<AActor*>& CreationIgnore, AActor* CreationParent, FTGOR_Time CreationTimestamp, int32 CreationIdentifier);

	/** Averages from hitresults */
	UFUNCTION(BlueprintPure, Category = "TGOR HitVolume", Meta = (Keywords = "C++"))
		FVector GetAverage(const FVector& Default, const TArray<FHitResult>& Hitresults);

	/** Streamlines applying damage to a list of hits */
	UFUNCTION(BlueprintCallable, Category = "TGOR HitVolume", Meta = (Keywords = "C++"))
		bool ApplyMultiDamage(const TArray<FHitResult>& Hitresults, int32 Segment, float Factor);

	/** Streamlines applying damage to a hit */
	UFUNCTION(BlueprintCallable, Category = "TGOR HitVolume", Meta = (Keywords = "C++"))
		bool ApplySingleDamage(const FTGOR_ElementInstance& State, const FHitResult& Hitresults, int32 Segment, float Factor);

	////////////////////////////////////////////////////////////////////////////////////////////////////
protected:

	/** Identifier this hitvolume spawned from */
	UPROPERTY(BlueprintReadOnly, Category = "TGOR HitVolume")
		int32 Identifier;

	/** Time this volume spawned */
	UPROPERTY(BlueprintReadOnly, Category = "TGOR HitVolume")
		FTGOR_Time StartTime;

	/** instigator actor */
	UPROPERTY(BlueprintReadOnly, Category = "TGOR HitVolume")
		AActor* Parent;
	
	/** Actors to ignore */
	UPROPERTY(BlueprintReadOnly, Category = "TGOR HitVolume")
		TArray<AActor*> Ignore;

	/** Damage this damage field does (As last registered by Update, cached for after termination) */
	UPROPERTY(BlueprintReadWrite, Category = "TGOR HitVolume")
		FTGOR_ElementInstance DamageState;

	/////////////////////////////////////////////// INTERNAL ///////////////////////////////////////////

protected:

private:

};
