// The Gateway of Realities: Planes of Existence.

#pragma once

#include "CoreMinimal.h"
#include "Editor/UnrealEd/Classes/Exporters/AnimSeqExportOption.h" 
#include "ISequencer.h"

#include "GameFramework/Actor.h"
#include "TGOR_SequenceTestingActor.generated.h"

//////////////////////////////////////////// DECL /////////////////////////////////////////////////

class ALevelSequenceActor;

/**
* TGOR_SequenceTestingActor is used to test sequence settings and operations from a safe distance
*/
UCLASS(ClassGroup = (Custom), Blueprintable, meta = (BlueprintSpawnableComponent))
class REALITIESANIMATION_API ATGOR_SequenceTestingActor : public AActor
{
	GENERATED_BODY()

public:

	////////////////////////////////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////// ENGINE ////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////////////////////////////

	ATGOR_SequenceTestingActor(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	////////////////////////////////////////////////////////////////////////////////////////////////////

	/** Actor that holds the skeletal mesh */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "!TGOR Bodypart")
		AActor* SourceActor;

	/** Skeleton to bake onto */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "!TGOR Bodypart")
		USkeleton* TargetSkeleton;

	/** Sequence to bake */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "!TGOR Bodypart")
		ALevelSequenceActor* SequenceActor;

	////////////////////////////////////////////////////////////////////////////////////////////////////

	/** Create the actual sequence */
	UFUNCTION(BlueprintCallable, Category = "Animation", Meta = (Keywords = "C++"))
		bool CreateAnimationSequence(const TArray<UObject*> NewAssets, USkeletalMeshComponent* SkelMeshComp, FGuid Binding, bool bCreateSoftLink);

	/** Test baking sequence to a different skeleton then in the source mesh */
	UFUNCTION(BlueprintCallable, CallInEditor, Category = "Animation", Meta = (Keywords = "C++"))
		void TestSequenceBakeToSkeleton();


	const TSharedPtr<ISequencer> GetSequencer() const;

private:

	UPROPERTY()
		UAnimSeqExportOption* AnimSeqExportOption;

private:
	TWeakPtr<ISequencer> Sequencer;
};
