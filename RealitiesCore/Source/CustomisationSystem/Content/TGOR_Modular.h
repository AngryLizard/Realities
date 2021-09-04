// The Gateway of Realities: Planes of Existence.

#pragma once

#include "DimensionSystem/Content/TGOR_SpawnModule.h"
#include "TGOR_Modular.generated.h"

class UTGOR_Bodypart;

/**
 * 
 */
UCLASS()
class CUSTOMISATIONSYSTEM_API UTGOR_Modular : public UTGOR_SpawnModule
{
	GENERATED_BODY()

public:
	UTGOR_Modular();

	////////////////////////////////////////////////////////////////////////////////////////////////////
public:

	/**
	* Skeleton asset
	*/
	UPROPERTY(EditAnywhere, Category = "!TGOR Animation")
		USkeleton* Skeleton;

	////////////////////////////////////////////////////////////////////////////////////////////////////
public:

	/** Supported bodypart insertions */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "!TGOR Insertion")
		TArray<TSubclassOf<UTGOR_Bodypart>> BodypartInsertions;
	DECL_INSERTION(BodypartInsertions);

	virtual void MoveInsertion(UTGOR_Content* Insertion, ETGOR_InsertionActionEnumeration Action, bool& Success) override;

private:
};
