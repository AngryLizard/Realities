// The Gateway of Realities: Planes of Existence.

#pragma once

#include "CoreMinimal.h"
#include "DimensionSystem/TGOR_DimensionInstance.h"

#include "CoreSystem/Interfaces/TGOR_SingletonInterface.h"
#include "GameFramework/PlayerController.h"
#include "TGOR_DimensionController.generated.h"

////////////////////////////////////////////// DECL //////////////////////////////////////////////////////

class UTGOR_WorldComponent;
class UTGOR_PilotMovement;

////////////////////////////////////////////// ENUM //////////////////////////////////////////////////////

UENUM(BlueprintType)
enum class ETGOR_PossessionResponse : uint8
{
	Success, // Succeeded
	Waiting, // Is waiting for dimension to load
	Collision, // Is already possessed
	NotFound, // Pawn doesn't exist
	Invalid // Dimension doesn't exist
};


/**
* TGOR_DimensionController handles loading and unloading dimensions for each player
*/
UCLASS()
class DIMENSIONSYSTEM_API ATGOR_DimensionController : public APlayerController, public ITGOR_SingletonInterface
{
	GENERATED_BODY()

	friend class UTGOR_WorldData;

public:

	ATGOR_DimensionController(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
	virtual void Tick(float DeltaTime) override;

	//////////////////////////////////////////// IMPLEMENTABLES ////////////////////////////////////////

	/** On posession failed response (Called on server only) */
	UFUNCTION(BlueprintImplementableEvent, Category = "!TGOR Online", Meta = (Keywords = "C++"))
		void OnPossessionResponse(ETGOR_PossessionResponse Response);

	////////////////////////////////////////////////// COMPONENTS //////////////////////////////////////
private:

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Character, meta = (AllowPrivateAccess = "true"))
		UTGOR_WorldComponent* WorldComponent;

public:
	FORCEINLINE UTGOR_WorldComponent* GetWorldManager() const { return WorldComponent; }

	////////////////////////////////////////////////////////////////////////////////////////////////////
public:

	/** Checks whether this user is registered to this world (server only). */
	UFUNCTION(BlueprintPure, BlueprintAuthorityOnly, Category = "!TGOR Online", Meta = (Keywords = "C++"))
		bool IsOnline() const;

	/** Called when any dimension was updated */
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "!TGOR Online", Meta = (Keywords = "C++"))
		virtual void OnWorldUpdate();

	////////////////////////////////////////////////////////////////////////////////////////////////////
protected:

	/** Body this controller is waiting on to possess */
	UPROPERTY(BlueprintReadOnly)
		FTGOR_DimensionIdentifier PossessionQueue;

	////////////////////////////////////////////////////////////////////////////////////////////////////
public:

	/** Possession queue */
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "!TGOR Dimension|Internal", Meta = (Keywords = "C++"))
		void SetPossessionQueue(const FTGOR_DimensionIdentifier& Identifier);

	/** Resets possession queue */
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "!TGOR Dimension|Internal", Meta = (Keywords = "C++"))
		void ClearPossessionQueue();

	/** Whether queue is currently filled */
	UFUNCTION(BlueprintPure, BlueprintAuthorityOnly, Category = "!TGOR Dimension|Internal", Meta = (Keywords = "C++"))
		bool HasPossessionQueue() const;

	/** Attempts possession, returns whether successful */
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "!TGOR Dimension|Internal", Meta = (Keywords = "C++"))
		bool ProgressPossessionQueue();

	////////////////////////////////////////////////////////////////////////////////////////////////////
protected:

	/** Called when we're posessing a given identity. Return true if successful */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Dimension", Meta = (Keywords = "C++"))
		virtual bool ApplyPossession(APawn* OtherPawn);
};
