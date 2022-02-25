// TGOR (C) // CHECKED //
#pragma once

#include "CoreMinimal.h"
#include "Utility/TGOR_CustomEnumerations.h"
#include "Components/Combat/TGOR_HitVolumeComponent.h"

#include "Actors/Combat/HitVolumes/TGOR_PatternHitVolume.h"
#include "TGOR_PredefinedPatternHitVolume.generated.h"


/**
 * ATGOR_PredefinedPatternHitVolume defines hitvolumes that manually define specific volume locations
 */
UCLASS(Blueprintable)
class REALITIES_API ATGOR_PredefinedPatternHitVolume : public ATGOR_PatternHitVolume
{
	GENERATED_BODY()
	
public:

	////////////////////////////////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////// ENGINE ////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////////////////////////////

	ATGOR_PredefinedPatternHitVolume(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
	virtual void UpdateVolume(int32 Index, float Time) override;
	virtual bool IsRunning(float Time) override;
	virtual int32 BuildVolumes() override;

	//////////////////////////////////////////////// IMPLEMENTABLES /////////////////////////////////////////

	/** Gets called when a volume gets initialised (Careful, only lower index volumes are initialised by now) */
	UFUNCTION(BlueprintImplementableEvent, Category = "TGOR HitVolume", Meta = (Keywords = "C++"))
		void BuildPredefinedVolume(int32 Index, UTGOR_HitVolumeComponent* Volume);

	/** Updates single volume that has been initialised in PatternContainer */
	UFUNCTION(BlueprintImplementableEvent, Category = "TGOR HitVolume", Meta = (Keywords = "C++"))
		void UpdatePredefinedVolume(int32 Index, UTGOR_HitVolumeComponent* Volume, float Time);

	/** Terminates single volume that has been initialised in PatternContainer */
	UFUNCTION(BlueprintImplementableEvent, Category = "TGOR HitVolume", Meta = (Keywords = "C++"))
		void TerminatePredefinedVolume(int32 Index, UTGOR_HitVolumeComponent* Volume, float Time);

	////////////////////////////////////////////// COMPONENTS //////////////////////////////////////////////////////
private:

	/** Container to hold all volumes (need to be UTGOR_HitVolumeComponent) */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "TGOR HitVolume", Meta = (AllowPrivateAccess = "true"))
		USceneComponent* PatternContainer;

public:

	FORCEINLINE USceneComponent* GetPatternContainer() const { return PatternContainer; }

	////////////////////////////////////////////////////////////////////////////////////////////////////
protected:

	/** Time at which this volume disappears */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "TGOR HitVolume")
		float LifeTime;
	
	/** List of found pattern volumes in PatternContainer */
	UPROPERTY(BlueprintReadOnly, Category = "TGOR HitVolume")
		TArray<UTGOR_HitVolumeComponent*> PatternVolumes;

	////////////////////////////////////////////////////////////////////////////////////////////////////
protected:

	/** Get volume at index */
	UFUNCTION(BlueprintCallable, Category = "TGOR HitVolume", Meta = (ExpandEnumAsExecs = "Branches", Keywords = "C++"))
		UTGOR_HitVolumeComponent* GetVolumeAt(int32 Index, ETGOR_FetchEnumeration& Branches);

	/** Applies default settings to component at Index if available */
	UFUNCTION(BlueprintCallable, Category = "TGOR HitVolume", Meta = (ExpandEnumAsExecs = "Branches", Keywords = "C++"))
		void ApplyDefault(int32 Index, UTGOR_HitVolumeComponent* Volume, ETGOR_BoolEnumeration& Branches);

	/** Returns velocity vector for parabola particle with given lifetime  */
	UFUNCTION(BlueprintPure, Category = "TGOR HitVolume", Meta = (Keywords = "C++"))
		FVector GetParabolaTo(const FVector& Location, float DownAcceleration, float Duration);

	/////////////////////////////////////////////// INTERNAL ///////////////////////////////////////////


protected:

private:

};
