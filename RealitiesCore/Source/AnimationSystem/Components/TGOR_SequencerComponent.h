// The Gateway of Realities: Planes of Existence.

#pragma once

#include "CoreMinimal.h"
#include "Net/UnrealNetwork.h"
#include "LevelSequenceActor.h"
#include "MovieSceneObjectBindingID.h"

#include "DimensionSystem/TGOR_MobilityInstance.h"

#include "DimensionSystem/Components/TGOR_MobilityComponent.h"
#include "TGOR_SequencerComponent.generated.h"

class UTGOR_SequencerPilotTask;

/**
 * TGOR_SequencerComponent allows components to attach linearly
 */
UCLASS(ClassGroup = (Custom), Blueprintable, meta = (BlueprintSpawnableComponent))
class ANIMATIONSYSTEM_API UTGOR_SequencerComponent : public UTGOR_MobilityComponent
{
	GENERATED_BODY()

		friend class UTGOR_SequencerPilotTask;
	
public:
	UTGOR_SequencerComponent();
	virtual void BeginPlay() override;

	//////////////////////////////////////////// IMPLEMENTABLES ////////////////////////////////////////

	////////////////////////////////////////////////////////////////////////////////////////////////////
public:

	/** Plays sequencer and properly disconnects all attached pilots on finish. */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Movement", Meta = (Keywords = "C++"))
		void PlaySequence();

	/** Disengages all connected pilots in case it needs to be done early to give time for blending */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Movement", Meta = (Keywords = "C++"))
		void DisengageSequencer();

	/** Parents a component to this mobility using the Parent socket at a given world position. */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Movement", Meta = (Keywords = "C++"))
		bool ParentSequencerPose(UTGOR_PilotComponent* Attachee, FMovieSceneObjectBindingID Binding);

	/** Parents a component to this mobility using the Parent socket. */
	UFUNCTION(BlueprintPure, Category = "!TGOR Movement", Meta = (Keywords = "C++"))
		bool CanParentSequencerPose(UTGOR_PilotComponent* Attachee, FMovieSceneObjectBindingID Binding) const;

	////////////////////////////////////////////////////////////////////////////////////////////////////
protected:

	/** Attached level sequence */
	UPROPERTY(EditAnywhere, Category = "!TGOR Movement")
		ALevelSequenceActor* SequenceActor = nullptr;

	/** Attachments to this component */
	UPROPERTY()
		TSet<TWeakObjectPtr<UTGOR_SequencerPilotTask>> SequencerAttachments;

	/** Add a child to attachment list */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Movement", Meta = (Keywords = "C++"))
		void RegisterAttachToSequencer(UTGOR_SequencerPilotTask* Attachee);

	/** Remove a child from attachment list */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Movement", Meta = (Keywords = "C++"))
		void RegisterDetachFromSequencer(UTGOR_SequencerPilotTask* Attachee);

	////////////////////////////////////////////////////////////////////////////////////////////////////
private:

	UFUNCTION()
		void OnSequenceFinished();

	USkeletalMeshComponent* GetMeshFromBinding(FMovieSceneObjectBindingID Binding) const;

};
