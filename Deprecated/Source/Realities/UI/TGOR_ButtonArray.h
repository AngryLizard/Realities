// The Gateway of Realities: Planes of Existence.

#pragma once

#include "TGOR_Menu.h"

#include "Realities/UI/TGOR_Widget.h"
#include "TGOR_ButtonArray.generated.h"

class UTGOR_SelectionQuery;
class UTGOR_ButtonEntry;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FButtonInitDelegate);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FButtonSelectDelegate, int32, Selection);

/**
 * TGOR_ButtonArray automatically generates a grid using a Menuconstructor
 */
UCLASS()
class REALITIES_API UTGOR_ButtonArray : public UTGOR_Menu
{
	GENERATED_BODY()

	////////////////////////////////////////////////////////////////////////////////////////////////////
public:

	////////////////////////////////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////// ENGINE ////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////////////////////////////

	virtual void OpenMenu(UTGOR_Query* Query) override;
	virtual void NativePreConstruct() override;

	//////////////////////////////////////////// IMPLEMENTABLES ////////////////////////////////////////

	UPROPERTY(BlueprintAssignable, Category = "TGOR Interface")
		FButtonInitDelegate OnArrayInit;

	UPROPERTY(BlueprintAssignable, Category = "TGOR Interface")
		FButtonSelectDelegate OnButtonSelect;

	UPROPERTY(BlueprintAssignable, Category = "TGOR Interface")
		FButtonSelectDelegate OnButtonCommit;

	/** Hover selection override */
	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, Category = "TGOR Interface", Meta = (Keywords = "C++"))
		void OnSelect(int32 Selection);

	/** Commit selection override */
	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, Category = "TGOR Interface", Meta = (Keywords = "C++"))
		void OnCommit(int32 Selection);

	/** Display button array, has to be implemented in UMG */
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "TGOR Interface", Meta = (Keywords = "C++"))
		void Display(const TArray<FTGOR_Display>& Icons);

	/** Select buttons in the button array */
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "TGOR Interface", Meta = (Keywords = "C++"))
		void SetSelection(int32 Select, float Slider);

	////////////////////////////////////////////////////////////////////////////////////////////////////
protected:

	/** Clear entries */
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "TGOR Interface", Meta = (Keywords = "C++"))
		void OnClearEntries();

	/** Attach entry to this array after creation, index in creation order (Called in design time as well) */
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "TGOR Interface", Meta = (Keywords = "C++"))
		void OnCreateEntry(UTGOR_ButtonEntry* Entry, int32 Index);

	/** Query used to display buttons */
	UPROPERTY(Transient, BlueprintReadOnly, Category = "TGOR Interface")
		UTGOR_SelectionQuery* OpenedQuery;

	////////////////////////////////////////////////////////////////////////////////////////////////////
public:

	/** Whether entries are listed in backwards order */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "TGOR Interface")
		bool BackwardsEntry;

	/** Number preview entries */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "TGOR Interface")
		int32 PreviewEntryCount;

	/** Entry type */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "TGOR Interface")
		TSubclassOf<UTGOR_ButtonEntry> ButtonEntryType;

	/** Button entry widgets */
	UPROPERTY(BlueprintReadOnly, Category = "TGOR Interface")
		TArray<UTGOR_ButtonEntry*> ButtonEntries;

	/** Commit selection */
	UFUNCTION(BlueprintCallable, Category = "TGOR Interface", Meta = (Keywords = "C++"))
		virtual void Commit(int32 Selection);

	/** Hover selection */
	UFUNCTION(BlueprintCallable, Category = "TGOR Interface", Meta = (Keywords = "C++"))
		virtual void Select(int32 Selection);

	/////////////////////////////////////////////// INTERNAL ///////////////////////////////////////////

protected:

private:

};
