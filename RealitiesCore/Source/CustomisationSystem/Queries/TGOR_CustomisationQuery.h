// The Gateway of Realities: Planes of Existence.

#pragma once

#include "CoreMinimal.h"

#include "../TGOR_BodypartInstance.h"
#include "../Components/TGOR_CustomisationComponent.h"

#include "UISystem/Queries/TGOR_ContentQuery.h"
#include "TGOR_CustomisationQuery.generated.h"

class UTGOR_Customisation;


/**
 *
 */
UCLASS()
class CUSTOMISATIONSYSTEM_API UTGOR_CustomisationEntryQuery : public UTGOR_ContentQuery
{
	GENERATED_BODY()

public:
	UTGOR_CustomisationEntryQuery();

	TArray<UTGOR_CoreContent*> QueryContent() const override;

	////////////////////////////////////////////////////////////////////////////////////////////////////
protected:

	UPROPERTY(BlueprintReadOnly, Category = "!TGOR Bodypart")
		UTGOR_BodypartListWidget* CallbackWidget;

	UPROPERTY(BlueprintReadOnly, Category = "!TGOR Bodypart")
		UTGOR_Customisation* Customisation;

	UPROPERTY(BlueprintReadOnly, Category = "!TGOR Bodypart")
		TArray<FTGOR_CustomisationPayload> ConnectedCustomisations;

	////////////////////////////////////////////////////////////////////////////////////////////////////
public:

	/** Create list from parent node */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Bodypart", Meta = (Keywords = "C++"))
		void SetupFromBodypartNode(UTGOR_BodypartListWidget* BodypartListWidget, UTGOR_Customisation* Type, ETGOR_CustomisationQueryEnumeration Query);
};


/**
 *
 */
UCLASS()
class CUSTOMISATIONSYSTEM_API UTGOR_CustomisationListQuery : public UTGOR_ContentQuery
{
	GENERATED_BODY()

public:
	UTGOR_CustomisationListQuery();

	TArray<UTGOR_CoreContent*> QueryContent() const override;

	////////////////////////////////////////////////////////////////////////////////////////////////////
protected:

	UPROPERTY(BlueprintReadOnly, Category = "!TGOR Bodypart")
		UTGOR_BodypartListWidget* CallbackWidget;

	UPROPERTY(BlueprintReadOnly, Category = "!TGOR Bodypart")
		TSubclassOf<UTGOR_Customisation> CustomisationType;

	UPROPERTY(BlueprintReadOnly, Category = "!TGOR Bodypart")
		TArray<UTGOR_Customisation*> ConnectedCustomisations;

	////////////////////////////////////////////////////////////////////////////////////////////////////
public:

	/** Create list from parent node */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Bodypart", Meta = (Keywords = "C++"))
		void SetupFromBodypartNode(UTGOR_BodypartListWidget* BodypartListWidget, TSubclassOf<UTGOR_Customisation> Type);
};