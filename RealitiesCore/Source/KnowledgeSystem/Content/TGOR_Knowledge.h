// The Gateway of Realities: Planes of Existence.

#pragma once

#include "CoreMinimal.h"
#include "CoreSystem/Content/TGOR_CoreContent.h"
#include "TGOR_Knowledge.generated.h"

class UTGOR_Tracker;
class UTGOR_Unlock;


/**
* Query used to check tracker status
*/
USTRUCT(BlueprintType)
struct FTGOR_TrackerQuery
{
	GENERATED_USTRUCT_BODY()

public:
	FTGOR_TrackerQuery();

	/** Minimum tracker value to trigger predicate */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "!TGOR Knowledge")
		float Threshold;

	/** Associated tracker class */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "!TGOR Knowledge")
		TSubclassOf<UTGOR_Tracker> Type;

	/** Content query association */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "!TGOR Knowledge")
		TArray<TSubclassOf<UTGOR_CoreContent>> Filter;

	/** Associated tracker content */
	UPROPERTY(BlueprintReadOnly, Category = "!TGOR Knowledge")
		UTGOR_Tracker* Tracker;
};

/**
 * 
 */
UCLASS(Blueprintable)
class KNOWLEDGESYSTEM_API UTGOR_Knowledge : public UTGOR_CoreContent
{
	GENERATED_BODY()

	friend class UTGOR_KnowledgeTree;

public:
	UTGOR_Knowledge();

	virtual void PostBuildResource() override;

	/** Trackers to query */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "!TGOR Knowledge")
		TArray<FTGOR_TrackerQuery> Trackers;

	/** Parent knowledges */
	UPROPERTY(BlueprintReadOnly, Category = "!TGOR Knowledge")
		TArray<UTGOR_Knowledge*> Parents;

	////////////////////////////////////////////////////////////////////////////////////////////////////
public:

	/** Child knowledges */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "!TGOR Insertion")
		TArray<TSubclassOf<UTGOR_Knowledge>> ChildInsertions;
	DECL_INSERTION(ChildInsertions);

	/** Attributes supported by this effect */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "!TGOR Insertion")
		TArray<TSubclassOf<UTGOR_Unlock>> UnlockInsertions;
	DECL_INSERTION(UnlockInsertions);


	virtual void MoveInsertion(UTGOR_Content* Insertion, ETGOR_InsertionActionEnumeration Action, bool& Success) override;
};
