// The Gateway of Realities: Planes of Existence.

#pragma once

#include "CoreMinimal.h"

#include "../TGOR_ActionInstance.h"
#include "CoreSystem/TGOR_DisplayInstance.h"

#include "UISystem/Queries/TGOR_ListQuery.h"
#include "TGOR_InteractionQuery.generated.h"

class UTGOR_InteractableComponent;
class UTGOR_ActionComponent;
class UTGOR_Action;

/**
*
*/
USTRUCT(BlueprintType)
struct ACTIONSYSTEM_API FTGOR_InteractionEntry
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		UTGOR_ActionComponent* Caller = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		int32 Identifier = -1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FTGOR_Display Display;
};

/**
*
*/
USTRUCT(BlueprintType)
struct ACTIONSYSTEM_API FTGOR_InteractionRequest
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		UTGOR_ActionComponent* Caller = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		UTGOR_Action* Action = nullptr;
};

/**
 * 
 */
UCLASS()
class ACTIONSYSTEM_API UTGOR_InteractionQuery : public UTGOR_ListQuery
{
	GENERATED_BODY()
	
public:
	UTGOR_InteractionQuery();

	virtual TArray<FTGOR_Display> QueryDisplays() const override;
	virtual void CallIndex(int32 Index) override;

	////////////////////////////////////////////////////////////////////////////////////////////////////

	/** Fills this constructor with all available subactions of one component */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Interaction", Meta = (Keywords = "C++"))
		void Emplace(UTGOR_ActionComponent* Component, UTGOR_Action* Action);

	/** Rebuilds action query with callable actions */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Interaction", Meta = (Keywords = "C++"))
		void Update();

	////////////////////////////////////////////////////////////////////////////////////////////////////
protected:

	/** Action displays */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "!TGOR Interaction")
		TArray<FTGOR_InteractionEntry> Entries;

	/** Action requests */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "!TGOR Interaction")
		TArray<FTGOR_InteractionRequest> Requests;

};
