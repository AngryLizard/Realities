// The Gateway of Realities: Planes of Existence.

#pragma once

#include "CoreMinimal.h"
#include "Realities/Base/Content/TGOR_Content.h"
#include "TGOR_Knowledge.generated.h"

class UTGOR_Tracker;


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
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TGOR Knowledge")
		float Threshold;

	/** Associated tracker class */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TGOR Knowledge")
		TSubclassOf<UTGOR_Tracker> Type;

	/** Content query association */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TGOR Knowledge")
		TArray<TSubclassOf<UTGOR_Content>> Filter;

	/** Associated tracker content */
	UPROPERTY(BlueprintReadOnly, Category = "TGOR Knowledge")
		UTGOR_Tracker* Tracker;
};

/**
 * 
 */
UCLASS(Blueprintable)
class REALITIES_API UTGOR_Knowledge : public UTGOR_Content
{
	GENERATED_BODY()

	friend class UTGOR_KnowledgeTree;

public:
	UTGOR_Knowledge();

	virtual void PostBuildResource() override;

	/** Trackers to query */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TGOR Knowledge")
		TArray<FTGOR_TrackerQuery> Trackers;

	/** Parent knowledge */
	UPROPERTY(BlueprintReadOnly, Category = "TGOR Knowledge")
		TArray<UTGOR_Knowledge*> Parents;

	/** Parent knowledges */
	UPROPERTY(BlueprintReadOnly, Category = "TGOR Knowledge")
		TArray<UTGOR_Knowledge*> Children;
};
