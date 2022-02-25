// TGOR (C) // CHECKED //
#pragma once

#include "CoreMinimal.h"
#include "Utility/Error/TGOR_Error.h"

#include "Actors/Combat/HitVolumes/TGOR_HitVolume.h"
#include "TGOR_PatternHitVolume.generated.h"

class UTGOR_HitVolumeComponent;

/**
 * ATGOR_PatternHitVolume defines hitvolumes that get built out of multiple volumes
 */
UCLASS(Blueprintable)
class REALITIES_API ATGOR_PatternHitVolume : public ATGOR_HitVolume
{
	GENERATED_BODY()
	
public:

	////////////////////////////////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////// ENGINE ////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////////////////////////////

	ATGOR_PatternHitVolume(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	virtual void Build(const FTGOR_ElementInstance& State, const FVector& Focus) override;
	virtual void Update(const FTGOR_ElementInstance& State, const FVector& Focus) override;
	virtual bool VolumeTick(float DeltaSeconds) override;

	//////////////////////////////////////////////// IMPLEMENTABLES /////////////////////////////////////////

	/** Updates volumes with focus */
	UFUNCTION(BlueprintCallable, Category = "TGOR HitVolume", Meta = (Keywords = "C++"))
		virtual void UpdateFocus(const FVector& Focus, FTGOR_Time Time);

	/** Updates single volume */
	UFUNCTION(BlueprintCallable, Category = "TGOR HitVolume", Meta = (Keywords = "C++"))
		virtual void UpdateVolume(int32 Index, float Time);

	/** Whether hitvolume is still running (deleted on false) */
	UFUNCTION(BlueprintCallable, Category = "TGOR HitVolume", Meta = (Keywords = "C++"))
		virtual bool IsRunning(float Time);

	/** Builds a pattern in this pattern and returns the amount of volumes */
	UFUNCTION(BlueprintCallable, Category = "TGOR HitVolume", Meta = (Keywords = "C++"))
		virtual int32 BuildVolumes();


	////////////////////////////////////////////// COMPONENTS //////////////////////////////////////////////////////
private:

	/**  */
	
	////////////////////////////////////////////////////////////////////////////////////////////////////
protected:

	/** Number of volumes */
	UPROPERTY(BlueprintReadOnly, Category = "TGOR HitVolume")
		int32 NumberOfVolumes;

	/////////////////////////////////////////////// INTERNAL ///////////////////////////////////////////

protected:

private:

};
