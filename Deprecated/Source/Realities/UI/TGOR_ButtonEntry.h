// The Gateway of Realities: Planes of Existence.

#pragma once

#include "TGOR_Menu.h"

#include "Realities/UI/TGOR_Widget.h"
#include "TGOR_ButtonEntry.generated.h"

class UTGOR_SelectionQuery;

/**
 * TGOR_ButtonEntry is a button that can be plugged into a ButtonArray
 */
UCLASS()
class REALITIES_API UTGOR_ButtonEntry : public UTGOR_Widget
{
	GENERATED_BODY()

	////////////////////////////////////////////////////////////////////////////////////////////////////
public:

	virtual void NativeOnMouseLeave(const FPointerEvent& InMouseEvent) override;
	virtual void NativeOnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual FReply NativeOnMouseButtonUp(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;

	////////////////////////////////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////// ENGINE ////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////////////////////////////

	/** Assign query */
	UFUNCTION(BlueprintCallable, Category = "TGOR Interface", Meta = (Keywords = "C++"))
		void Assign(const FTGOR_Display& Display, UTGOR_SelectionQuery* Query, int32 Index);

	//////////////////////////////////////////// IMPLEMENTABLES ////////////////////////////////////////

	/** Assign a query entry for custom non-display struct data, index in query order */
	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, Category = "TGOR Interface", Meta = (Keywords = "C++"))
		void OnAssign(UTGOR_SelectionQuery* Query, int32 Index);

	/** Display a display struct */
	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, Category = "TGOR Interface", Meta = (Keywords = "C++"))
		void OnDisplay(const FTGOR_Display& Display);

	////////////////////////////////////////////////////////////////////////////////////////////////////
public:

	/** Set pressed */
	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, Category = "TGOR Interface", Meta = (Keywords = "C++"))
		void VisualisePressed();

	/** Set leave */
	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, Category = "TGOR Interface", Meta = (Keywords = "C++"))
		void VisualiseLeave();

	/** Set hovering */
	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, Category = "TGOR Interface", Meta = (Keywords = "C++"))
		void VisualiseHover();

	/** Set selected state */
	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, Category = "TGOR Interface", Meta = (Keywords = "C++"))
		void VisualiseSelected(bool Selected);

	////////////////////////////////////////////////////////////////////////////////////////////////////
protected:

	/** Query used to display buttons */
	UPROPERTY(Transient, BlueprintReadOnly, Category = "TGOR Interface")
		UTGOR_SelectionQuery* OpenedQuery;

	/** Index used to display buttons */
	UPROPERTY( BlueprintReadOnly, Category = "TGOR Interface")
		int32 OpenedIndex;

	/////////////////////////////////////////////// INTERNAL ///////////////////////////////////////////

private:

};
