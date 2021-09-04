// The Gateway of Realities: Planes of Existence.

#pragma once

#include "CoreMinimal.h"
#include "UISystem/Queries/TGOR_Query.h"
#include "TGOR_TabQuery.generated.h"

class UTGOR_ItemQuery;
class UTGOR_ActionTask;
class UTGOR_TabMenu;

/**
 * 
 */
UCLASS()
class ACTIONSYSTEM_API UTGOR_TabQuery : public UTGOR_Query
{
	GENERATED_BODY()
	
public:
	UTGOR_TabQuery();

	/** Owning task */
	UPROPERTY(BlueprintReadWrite, Category = "!TGOR Action")
		TWeakObjectPtr<UTGOR_ActionTask> OwnerTask;

	/** Tab menu opened through this query */
	UPROPERTY(BlueprintReadWrite, Category = "!TGOR Action")
		TSubclassOf<UTGOR_TabMenu> Tab;
};
