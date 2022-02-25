// The Gateway of Realities: Planes of Existence.

#pragma once

#include "CoreMinimal.h"
#include "Realities/UI/Queries/TGOR_ListQuery.h"
#include "TGOR_SelectionQuery.generated.h"

class UTGOR_ButtonArray;

/**
 * 
 */
UCLASS()
class REALITIES_API UTGOR_SelectionQuery : public UTGOR_ListQuery
{
	GENERATED_BODY()
	
public:
	UTGOR_SelectionQuery();
	virtual void CallIndex(int32 Index) override;

	////////////////////////////////////////////////////////////////////////////////////////////////////
protected:

	/** Initial selection that query reverts to on Cancel */
	UPROPERTY(BlueprintReadWrite, Category = "TGOR Interface")
		int32 InitialSelection;

	/** Current selection */
	UPROPERTY(BlueprintReadWrite, Category = "TGOR Interface")
		int32 Selection;

	/** Parent to be notified if set */
	UPROPERTY(BlueprintReadWrite, Category = "TGOR Interface")
		UTGOR_ButtonArray* Parent;

	////////////////////////////////////////////////////////////////////////////////////////////////////
public:

	/** Assign parent menu to be notified on selection */
	UFUNCTION(BlueprintCallable, Category = "TGOR Interface", Meta = (Keywords = "C++"))
		void AssignParent(UTGOR_ButtonArray* Menu);

	/** Silently set selection without calling the querys CallIndex */
	UFUNCTION(BlueprintCallable, Category = "TGOR Interface", Meta = (Keywords = "C++"))
		void InitialiseSelection(int32 Index);

	/** Cancel selection and revert to the initial selection */
	UFUNCTION(BlueprintCallable, Category = "TGOR Interface", Meta = (Keywords = "C++"))
		virtual void Revert();

	/** Cancel selection override */
	UFUNCTION(BlueprintImplementableEvent, Category = "TGOR Interface", Meta = (Keywords = "C++"))
		void OnRevert();

	/** Commit selection */
	UFUNCTION(BlueprintCallable, Category = "TGOR Interface", Meta = (Keywords = "C++"))
		virtual void Commit();

	/** Commit selection override */
	UFUNCTION(BlueprintImplementableEvent, Category = "TGOR Interface", Meta = (Keywords = "C++"))
		void OnCommit();
};
