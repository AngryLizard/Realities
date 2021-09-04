// The Gateway of Realities: Planes of Existence.

#pragma once

#include "CoreMinimal.h"
#include "CoreSystem/Content/TGOR_CoreContent.h"

#include "UISystem/Queries/TGOR_ContentQuery.h"
#include "TGOR_BodypartListQuery.generated.h"

class UTGOR_Bodypart;
class UTGOR_BodypartListWidget;

/**
 * 
 */
UCLASS()
class CUSTOMISATIONSYSTEM_API UTGOR_BodypartListQuery : public UTGOR_ContentQuery
{
	GENERATED_BODY()
	
public:
	UTGOR_BodypartListQuery();

	TArray<UTGOR_CoreContent*> QueryContent() const override;
	virtual void CommitNone() override;
	virtual void CommitContent(int32 Index) override;

	////////////////////////////////////////////////////////////////////////////////////////////////////
protected:

	UPROPERTY(BlueprintReadOnly, Category = "!TGOR Bodypart")
		int32 CustomisationNodeIndex;

	UPROPERTY(BlueprintReadOnly, Category = "!TGOR Bodypart")
		UTGOR_CustomisationComponent* OwnerComponent;

	UPROPERTY(BlueprintReadOnly, Category = "!TGOR Bodypart")
		TSubclassOf<UTGOR_Bodypart> BodypartType;

	UPROPERTY(BlueprintReadOnly, Category = "!TGOR Bodypart")
		TArray<UTGOR_Bodypart*> PossibleBodyparts;

	////////////////////////////////////////////////////////////////////////////////////////////////////
protected:

	/** Remove bodypart from node */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Bodypart", Meta = (Keywords = "C++"))
		void RemoveBodypart();

	/** Replace bodypart from node */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Bodypart", Meta = (Keywords = "C++"))
		void ReplaceBodypart(UTGOR_Bodypart* Content);

	////////////////////////////////////////////////////////////////////////////////////////////////////
public:

	/** Sets the initial selection to the given bodypart */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Bodypart", Meta = (Keywords = "C++"))
		void InitialiseBodypartSelection(UTGOR_Bodypart* Bodypart);

	/** Sets display to possible bodyparts */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Bodypart", Meta = (Keywords = "C++"))
		void SetPossibleBodyparts(APlayerController* OwningPlayer, UTGOR_CustomisationComponent* Component, int32 NodeIndex, TSubclassOf<UTGOR_Bodypart> Type, bool IsRequired);
};
