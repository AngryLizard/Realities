// The Gateway of Realities: Planes of Existence.

#pragma once

#include "CoreMinimal.h"

#include "Realities/UI/Queries/TGOR_ContentQuery.h"
#include "TGOR_EquipmentQuery.generated.h"

class UTGOR_ActionComponent;
class UTGOR_Action;

/**
* TGOR_EquipmentQuery scans an action component for active slots of a given type
*/
UCLASS(Blueprintable, DefaultToInstanced)
class REALITIES_API UTGOR_EquipmentQuery : public UTGOR_ContentQuery
{
	GENERATED_BODY()
	
public:
	UTGOR_EquipmentQuery();
	TArray<UTGOR_Content*> QueryContent() const override;

	////////////////////////////////////////////////////////////////////////////////////////////////////
public:
	
	/** Grab equipped actions of given type */
	UFUNCTION(BlueprintCallable, Category = "TGOR Combat", Meta = (Keywords = "C++"))
		void Assign(UTGOR_ActionComponent* ActionComponent, TSubclassOf<UTGOR_Action> Type);

	/** Get selected action */
	UFUNCTION(BlueprintCallable, Category = "TGOR Combat", Meta = (Keywords = "C++"))
		UTGOR_Action* GetSelectedAction() const;
	
	UPROPERTY(BlueprintReadOnly, Category = "TGOR Combat")
		TArray<UTGOR_Action*> Actions;

};
