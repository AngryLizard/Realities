// TGOR (C) // CHECKED //
#pragma once

#include "CoreMinimal.h"
#include "Utility/Error/TGOR_Error.h"

#include "Actors/Combat/HitVolumes/TGOR_HitVolume.h"
#include "TGOR_FunctionHitVolume.generated.h"


///////////////////////////////////////////////// STRUCTS //////////////////////////////////////////////

USTRUCT(BlueprintType)
struct REALITIES_API FTGOR_FunctionSegment
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TGOR HitVolume")
		FTGOR_Time Time;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TGOR HitVolume")
		FTGOR_Time LastDisplay;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TGOR HitVolume")
		bool Active;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TGOR HitVolume")
		int32 Index;


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TGOR HitVolume")
		float Distance;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TGOR HitVolume")
		FTransform Transform;


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TGOR HitVolume")
		FVector Primary;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TGOR HitVolume")
		FVector Secondary;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TGOR HitVolume")
		FVector Tertiary;
};

/**
 * ATGOR_FunctionHitVolume defines hitvolumens that follow a function
 */
UCLASS(Blueprintable)
class REALITIES_API ATGOR_FunctionHitVolume : public ATGOR_HitVolume
{
	GENERATED_BODY()

public:

	////////////////////////////////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////// ENGINE ////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////////////////////////////

	ATGOR_FunctionHitVolume(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	virtual void Build(const FTGOR_ElementInstance& State, const FVector& Focus) override;
	virtual void Update(const FTGOR_ElementInstance& State, const FVector& Focus) override;
	virtual bool VolumeTick(float DeltaSeconds) override;

	//////////////////////////////////////////////// IMPLEMENTABLES ////////////////////////////////////

	/** Moves a volume */
	UFUNCTION(BlueprintImplementableEvent, Category = "TGOR HitVolume", Meta = (Keywords = "C++"))
		bool MoveVolume(const FTGOR_FunctionSegment& Segment, const FVector& From, const FVector& To, float Time);

	/** Called once a segment got spawned */
	UFUNCTION(BlueprintImplementableEvent, Category = "TGOR HitVolume", Meta = (Keywords = "C++"))
		void SpawnVolume(const FTGOR_FunctionSegment& Segment);

	/** Timeout volume */
	UFUNCTION(BlueprintImplementableEvent, Category = "TGOR HitVolume", Meta = (Keywords = "C++"))
		void TimeoutVolume(const FTGOR_FunctionSegment& Segment, const FVector& Location, float Time);

	/** Called once a segment got spawned */
	UFUNCTION(BlueprintImplementableEvent, Category = "TGOR HitVolume", Meta = (Keywords = "C++"))
		void ValidateVolume(const FTGOR_FunctionSegment& Segment);

	/** Called once a segment got removed */
	UFUNCTION(BlueprintImplementableEvent, Category = "TGOR HitVolume", Meta = (Keywords = "C++"))
		void InvalidateVolume(const FTGOR_FunctionSegment& Segment);

	////////////////////////////////////////////// COMPONENTS //////////////////////////////////////////
private:

	/**  */

	////////////////////////////////////////////////////////////////////////////////////////////////////
protected:

	/** Length of one segment in seconds */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "TGOR HitVolume")
		float SegmentTime;

	/** Rate at which new segments spawn (0.0 for single segment) */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "TGOR HitVolume")
		float SpawnTime;

	/** Lifetime of a projectile in seconds */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "TGOR HitVolume")
		float Lifetime;

	/** Volume head index */
	UPROPERTY(BlueprintReadOnly, Category = "TGOR HitVolume")
		int32 Head;

	/** Set of volume structs */
	UPROPERTY(BlueprintReadOnly, Category = "TGOR HitVolume")
		TArray<FTGOR_FunctionSegment> Segments;

	/////////////////////////////////////////////// INTERNAL ///////////////////////////////////////////

protected:

	// Setup params
	virtual void Setup(FTGOR_FunctionSegment& Segment);

	// Computes function offset depending on x (time offset) and total time (in case curve changes over time)
	virtual FVector Function(float X, const FTGOR_FunctionSegment& Segment, float Time);

private:

	/** Add empty segment to list */
	void AddEmptySegment();

	/** Display segment */
	bool DisplaySegment(FTGOR_FunctionSegment& Segment, FTGOR_Time Timestamp);

	/** Times a segment out */
	void TimeoutSegment(const FTGOR_FunctionSegment& Segment, FTGOR_Time Timestamp);

	/** Moves head forward */
	void MoveHead(const FVector& Focus, FTGOR_Time Timestamp);
	
	/** Creates head volume */
	void CreateHead(const FVector& Focus, FTGOR_Time Timestamp);

};
