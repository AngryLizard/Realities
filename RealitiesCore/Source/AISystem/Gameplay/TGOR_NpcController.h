// The Gateway of Realities: Planes of Existence.

#pragma once

#include "RealitiesUtility/Structures/TGOR_Time.h"
#include "RealitiesUtility/Structures/TGOR_Samples.h"

#include "MovementSystem/TGOR_MovementInstance.h"

#include "CoreSystem/Interfaces/TGOR_SingletonInterface.h"
#include "GameFramework/Controller.h"
#include "TGOR_NpcController.generated.h"

////////////////////////////////////////////// DECL //////////////////////////////////////////////////////

class UTGOR_Behaviour;
class UTGOR_AimComponent;
class UTGOR_ColliderComponent;


////////////////////////////////////////////// STRUCT //////////////////////////////////////////////////////

USTRUCT(BlueprintType)
struct FTGOR_PerceptionPoint
{
	GENERATED_USTRUCT_BODY()

	/** Timestamp */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "!TGOR NPC")
		FTGOR_Time Timestamp;

	/** Perceived position */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "!TGOR NPC")
		FVector Point = FVector::ZeroVector;

	/** Perceived direction */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "!TGOR NPC")
		FVector Normal = FVector::ForwardVector;
};

USTRUCT(BlueprintType)
struct FTGOR_Perception
{
	GENERATED_USTRUCT_BODY()

	TArray<FTGOR_Samples> CreateSamples(const FVector& Project, float Threshold, float Distance) const;

	/** Perceived direction */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "!TGOR NPC")
		TArray<FTGOR_PerceptionPoint> Points;
};

USTRUCT(BlueprintType)
struct FTGOR_Memory
{
	GENERATED_USTRUCT_BODY()
		FTGOR_Memory();

	/** Perceived objects */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "!TGOR NPC")
		TMap<UTGOR_MobilityComponent*, FTGOR_Perception> Perceptions;
};

/**
* TGOR_NpcController controls npcs
*/
UCLASS(BlueprintType, Blueprintable)
class AISYSTEM_API ATGOR_NpcController : public AController, public ITGOR_SingletonInterface
{
	GENERATED_BODY()

public:

	////////////////////////////////////////////////////////////////////////////////////////////////////

	ATGOR_NpcController(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	virtual void Tick(float DeltaTime) override;
	virtual void GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const override;
	
	//////////////////////////////////////////// IMPLEMENTABLES ////////////////////////////////////////

	/** Update passive perception */
	UFUNCTION(BlueprintImplementableEvent, Category = "!TGOR NPC", Meta = (Keywords = "C++"))
		void PassivePerception();

	////////////////////////////////////////////////// COMPONENTS //////////////////////////////////////
private:

	////////////////////////////////////////////////////////////////////////////////////////////////////
public:

	/** See whether object is still visible. */
	UFUNCTION(BlueprintPure, Category = "!TGOR NPC", Meta = (Keywords = "C++"))
		bool CanSeeObject(UTGOR_MobilityComponent* Mobility) const;

	/** Take a glance at the world from current viewing direction */
	UFUNCTION(BlueprintCallable, Category = "!TGOR NPC", Meta = (Keywords = "C++"))
		bool SamplePerception(UTGOR_ColliderComponent* Collider, FVector2D SpreadX, FVector2D SpreadY, float Radius, float Distance);

	////////////////////////////////////////////////////////////////////////////////////////////////////
protected:

	/** Last perception update (Update at most once per second) */
	UPROPERTY(BlueprintReadOnly, Category = "!TGOR NPC", Meta = (Keywords = "C++"))
		FTGOR_Time LastPerceptionUpdate;

	/** Least time in between perception updates */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "!TGOR NPC", Meta = (Keywords = "C++"))
		float PerceptionUpdateTime;

	/** Time until perceptions disappear */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "!TGOR NPC", Meta = (Keywords = "C++"))
		float PerceptionTimeout;
	
	////////////////////////////////////////////////////////////////////////////////////////////////////
public:

	/** Track given object. */
	UFUNCTION(BlueprintCallable, Category = "!TGOR NPC", Meta = (Keywords = "C++"))
		void SetTrackingObject(UTGOR_MobilityComponent* Mobility);

	/** Get all dynamics we have perceived. */
	UFUNCTION(BlueprintCallable, Category = "!TGOR NPC", Meta = (Keywords = "C++"))
		void GetPerceivedObstacles(const FVector& Project, float Threshold, float Distance, TArray<FTGOR_MovementObstacle>& Obstacles) const;

	////////////////////////////////////////////////////////////////////////////////////////////////////
protected:

	/** Currently tracked object */
	UPROPERTY(BlueprintReadOnly, Category = "!TGOR NPC", Meta = (Keywords = "C++"))
		TWeakObjectPtr<UTGOR_MobilityComponent> Tracking;

	/** Last tracked locations of currently tracked object */
	UPROPERTY(BlueprintReadOnly, Category = "!TGOR NPC", Meta = (Keywords = "C++"))
		TArray<FTGOR_MovementSpace> TrackingTrail;

	/** How long the tracking queue gets */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "!TGOR NPC", Meta = (Keywords = "C++"))
		int32 TrackingTrailLength;

	////////////////////////////////////////////////////////////////////////////////////////////////////
protected:

	/** Currently running behaviour */
	UPROPERTY(BlueprintReadWrite, Category = "!TGOR NPC", Meta = (Keywords = "C++"))
		UTGOR_Behaviour* Behaviour;

	/** NPC memory and perception */
	UPROPERTY(BlueprintReadOnly, Category = "!TGOR NPC", Meta = (Keywords = "C++"))
		FTGOR_Memory Memory;

	/** Draws perception into world. */
	UFUNCTION(BlueprintCallable, Category = "!TGOR NPC", Meta = (Keywords = "C++"))
		void DebugDrawPerceptions(FVector Project, float Threshold, float Distance, float Duration, FLinearColor Color);

	////////////////////////////////////////////////////////////////////////////////////////////////////
protected:

	/** Previous movement input */
	UPROPERTY(BlueprintReadOnly, Category = "!TGOR NPC", Meta = (Keywords = "C++"))
		FVector PrevInput;

	/** Previous movement view */
	UPROPERTY(BlueprintReadOnly, Category = "!TGOR NPC", Meta = (Keywords = "C++"))
		FQuat PrevView;

	////////////////////////////////////////////////////////////////////////////////////////////////////
private:

	/** Next movement input */
	UPROPERTY()
		FVector NextInput;

	/** Next movement view */
	UPROPERTY()
		FQuat NextView;


	/** Random number generator for NPC operations */
	UPROPERTY()
		FRandomStream Random;

	/////////////////////////////////////////////// INTERNAL ///////////////////////////////////////////

protected:

private:
	
};
