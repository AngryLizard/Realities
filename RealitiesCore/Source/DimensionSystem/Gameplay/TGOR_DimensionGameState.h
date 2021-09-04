// The Gateway of Realities: Planes of Existence.

#pragma once

#include "CoreMinimal.h"

#include "DimensionSystem/TGOR_DimensionInstance.h"

#include "CoreSystem/Gameplay/TGOR_GameState.h"
#include "TGOR_DimensionGameState.generated.h"

class UTGOR_WorldComponent;

/**
 * 
 */
UCLASS()
class DIMENSIONSYSTEM_API ATGOR_DimensionGameState : public ATGOR_GameState
{
	GENERATED_BODY()

		friend class UTGOR_WorldData;
	friend class ATGOR_WorldSettings;

public:
	ATGOR_DimensionGameState(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());


	virtual void Tick(float DeltaSeconds) override;
	virtual void GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const override;

	//////////////////////////////////////////// IMPLEMENTABLES ////////////////////////////////////////

	////////////////////////////////////////////////// COMPONENTS //////////////////////////////////////
private:

	/** Loader used by the server. This is needed in addition to the hosting player controller so that dedicated servers load dimensions properly */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Character, meta = (AllowPrivateAccess = "true"))
		UTGOR_WorldComponent* WorldComponent;

public:

	FORCEINLINE UTGOR_WorldComponent* GetWorldManager() const { return WorldComponent; }
	
	////////////////////////////////////////////////////////////////////////////////////////////////////
public:

	/** Timeout in seconds for how long server waits for clients to unload */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "!TGOR System")
		float LoadTimeout;

	/** Server loaded dimensions */
	UPROPERTY(BlueprintReadOnly, Replicated, Category = "!TGOR Dimension", Meta = (Keywords = "C++"))
		FTGOR_DimensionRequestState DimensionRequestState;

	////////////////////////////////////////////////////////////////////////////////////////////////////
public:

	/** Called after the initial dimensions are spawned, for example at the start of the game when the hub level is being loaded.
	@param True when a initial dimension was loaded, such as when the persistent level is not an initial dimension, otherwise - false.
	*/
	UFUNCTION(BlueprintImplementableEvent, Category = "!TGOR Dimension", Meta = (Keywords = "C++"))
		void OnSpawnInitialDimensions(bool SpawnedInitial);

	/** Check whether anything loading related is going on for dimensions or content */
	UFUNCTION(BlueprintCallable, Category = "!TGOR System", Meta = (Keywords = "C++"))
		bool HasFinishedLoading() const;

	////////////////////////////////////////////////////////////////////////////////////////////////////
private:
};
