// The Gateway of Realities: Planes of Existence.

#pragma once

#include "CoreMinimal.h"

#include "../TGOR_BodypartInstance.h"

#include "UISystem/Queries/TGOR_ContentQuery.h"
#include "TGOR_ChildListQuery.generated.h"

class UTGOR_Bodypart;
class UTGOR_BodypartListWidget;


USTRUCT(BlueprintType)
struct FTGOR_ChildListEntry
{
	GENERATED_USTRUCT_BODY();

	/** Child from parent node */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "!TGOR Bodypart")
		FTGOR_BodypartChild Child;

	/** Bodypart index */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "!TGOR Bodypart")
		int32 Index = -1;

	/** Whether this entry is currently active */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "!TGOR Bodypart")
		bool IsActive = false;

	/** Whether this entry is currently required by parent */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "!TGOR Bodypart")
		bool IsRequired = false;
};


/**
 * 
 */
UCLASS()
class CUSTOMISATIONSYSTEM_API UTGOR_ChildListQuery : public UTGOR_ContentQuery
{
	GENERATED_BODY()
	
public:
	UTGOR_ChildListQuery();

	TArray<UTGOR_CoreContent*> QueryContent() const override;
	virtual void CommitContent(int32 Index) override;

	////////////////////////////////////////////////////////////////////////////////////////////////////
protected:

	UPROPERTY(BlueprintReadOnly, Category = "!TGOR Bodypart")
		UTGOR_BodypartListWidget* CallbackWidget = nullptr;

	UPROPERTY(BlueprintReadOnly, Category = "!TGOR Bodypart")
		TArray<FTGOR_ChildListEntry> ConnectedChildren;

	////////////////////////////////////////////////////////////////////////////////////////////////////
public:

	/** Create list from parent node */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Bodypart", Meta = (Keywords = "C++"))
		UTGOR_Bodypart* GetBodypartContent(const FTGOR_ChildListEntry& Entry) const;

	/** Create list from parent node */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Bodypart", Meta = (Keywords = "C++"))
		void SetupFromParentNode(UTGOR_BodypartListWidget* BodypartListWidget);
};
