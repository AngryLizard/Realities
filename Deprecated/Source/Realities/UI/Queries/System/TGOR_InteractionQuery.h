// The Gateway of Realities: Planes of Existence.

#pragma once

#include "CoreMinimal.h"
#include "Realities/Base/Instances/System/TGOR_ActionInstance.h"
#include "Realities/Base/Instances/Combat/TGOR_AimInstance.h"

#include "UObject/TextProperty.h"

#include "Realities/UI/Queries/TGOR_ListQuery.h"
#include "TGOR_InteractionQuery.generated.h"

class UTGOR_InteractableComponent;
class UTGOR_ActionComponent;
class UTGOR_Action;


/**
*
*/
USTRUCT(BlueprintType)
struct REALITIES_API FTGOR_InteractionEntry
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		UTGOR_ActionComponent* Caller;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FTGOR_SlotIndex SlotIndex;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FTGOR_Display Display;
};

/**
 * 
 */
UCLASS()
class REALITIES_API UTGOR_InteractionQuery : public UTGOR_ListQuery
{
	GENERATED_BODY()
	
public:
	UTGOR_InteractionQuery();

	virtual TArray<FTGOR_Display> QueryDisplays() const override;
	virtual void CallIndex(int32 Index) override;

	////////////////////////////////////////////////////////////////////////////////////////////////////

	/** Fills this constructor with all available actions of one component */
	UFUNCTION(BlueprintCallable, Category = "TGOR Interaction", Meta = (Keywords = "C++"))
		void Emplace(UTGOR_ActionComponent* Component, TSubclassOf<UTGOR_Action> ActionClass);

	/** Empties all data */
	UFUNCTION(BlueprintCallable, Category = "TGOR Interaction", Meta = (Keywords = "C++"))
		void Reset();

	////////////////////////////////////////////////////////////////////////////////////////////////////
protected:

	/** Action displays */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TGOR Interaction")
		TArray<FTGOR_InteractionEntry> Entries;

};
