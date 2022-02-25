// The Gateway of Realities: Planes of Existence.

#pragma once

#include "CoreMinimal.h"
#include "Realities/Utility/Error/TGOR_Error.h"
#include "Realities/Utility/TGOR_CustomEnumerations.h"

#include "Realities/UI/TGOR_ButtonEntry.h"
#include "TGOR_BodypartWidget.generated.h"

class UTGOR_Bodypart;
class UTGOR_BodypartNode;
class UTGOR_BodypartListWidget;

class UTGOR_ContentQuery;

/**
 * 
 */
UCLASS()
class REALITIES_API UTGOR_BodypartWidget : public UTGOR_ButtonEntry
{
	GENERATED_BODY()
	
public:
	UTGOR_BodypartWidget(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());


	////////////////////////////////////////////////////////////////////////////////////////////////////
protected:

	UPROPERTY(BlueprintReadOnly, Category = "TGOR Bodypart")
		UTGOR_BodypartListWidget* BodypartListWidget;

	UPROPERTY(BlueprintReadOnly, Category = "TGOR Bodypart")
		UTGOR_BodypartNode* BodypartNode;

	UPROPERTY(BlueprintReadOnly, Category = "TGOR Bodypart")
		uint8 EntryIndex;

	////////////////////////////////////////////////////////////////////////////////////////////////////
public:

	/** Get bodypart list widget */
	UFUNCTION(BlueprintPure, Category = "TGOR Bodypart", Meta = (Keywords = "C++"))
		UTGOR_BodypartListWidget* GetBodypartList() const;

	/** Get bodypart node */
	UFUNCTION(BlueprintPure, Category = "TGOR Bodypart", Meta = (Keywords = "C++"))
		UTGOR_BodypartNode* GetBodypartNode() const;


	/** Opens a content query on the bodypartlist widget */
	UFUNCTION(BlueprintCallable, Category = "TGOR Bodypart", Meta = (Keywords = "C++"))
		void OpenContentQuery(UTGOR_ContentQuery* Query);

	/** Connects this widget to a bodypart for callbacks */
	UFUNCTION(BlueprintCallable, Category = "TGOR Bodypart", Meta = (Keywords = "C++"))
		void Connect(UTGOR_BodypartListWidget* ListWidget, UTGOR_BodypartNode* Node, uint8 Index);

	/** Initialises after connection */
	UFUNCTION(BlueprintImplementableEvent, Category = "TGOR Bodypart", Meta = (Keywords = "C++"))
		void ConnectInit();
};
