// The Gateway of Realities: Planes of Existence.

#pragma once

#include "Realities/Base/Controller/TGOR_PlayerController.h"

#include "Realities/UI/TGOR_Widget.h"
#include "TGOR_Menu.generated.h"

////////////////////////////////////////////// DECL //////////////////////////////////////////////////////

class UTGOR_Query;

////////////////////////////////////////////// ENUM //////////////////////////////////////////////////////

/**
* Open/Close types for menus
*/
UENUM(BlueprintType)
enum class ETGOR_MenuTypeEnumeration : uint8
{
	/** Adds the menu to the viewport when opening and removes it when closing */
	Viewport,

	/** Makes the menu visible on open and hides it on closing */
	Visibility,

	/** Doesn't do anything with the widget when opening/closing */
	Permanent
	
};

///////////////////////////////////////////// STRUCTS ///////////////////////////////////////////////////

/**
 * TGOR_MenuAnimation holds two animation references
 */
USTRUCT(BlueprintType)
struct REALITIES_API FTGOR_MenuAnimation
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TGOR Interface")
		UWidgetAnimation* Expand;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TGOR Interface")
		UWidgetAnimation* Retract;
};

/**
* TGOR_Menu provides general menu functionality (layering, focus, open, close etc)
*/
UCLASS()
class REALITIES_API UTGOR_Menu : public UTGOR_Widget
{
	GENERATED_BODY()
	
public:

	UTGOR_Menu(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;
	virtual void OnWidgetRebuilt() override;
	
	//////////////////////////////////////////// IMPLEMENTABLES ////////////////////////////////////////

	/** Checks whether this menu is currently playing a trantision e.g. animation */
	UFUNCTION(BlueprintImplementableEvent, Category = "TGOR Interface", Meta = (Keywords = "C++"))
		void IsPlayingTransition(ETGOR_BoolEnumeration& Return);

	/** Optional pre-opening implementation (Opening animations etc) */
	UFUNCTION(BlueprintImplementableEvent, Category = "TGOR Interface", Meta = (Keywords = "C++"))
		void PrepareOpen(UTGOR_Query* Query);

	/** Optional opening implementation */
	UFUNCTION(BlueprintImplementableEvent, Category = "TGOR Interface", Meta = (Keywords = "C++"))
		void Activate();

	/** Optional pre-removal implementation (Closing animations etc) */
	UFUNCTION(BlueprintImplementableEvent, Category = "TGOR Interface", Meta = (Keywords = "C++"))
		void PrepareClose();

	/** Optional removal implementation (On actual removal) */
	UFUNCTION(BlueprintImplementableEvent, Category = "TGOR Interface", Meta = (Keywords = "C++"))
		void Cleanup();

	////////////////////////////////////////////////////////////////////////////////////////////////////
public:

	/** ZOrder and focus order */
	UPROPERTY(BlueprintReadOnly, Category = "TGOR Interface")
		int32 MenuLayer;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "TGOR Interface")
		ETGOR_MenuTypeEnumeration Type;

	/** If true, doesn't react to controller and escape calls */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TGOR Interface")
		bool IgnoreFocus;

	////////////////////////////////////////////////////////////////////////////////////////////////////
public:

	/** Return whether menu ignores focus */
	UFUNCTION(BlueprintPure, Category = "TGOR Interface", Meta = (Keywords = "C++"))
		bool IgnoresFocus() const;

	/** Return menu layer */
	UFUNCTION(BlueprintPure, Category = "TGOR Interface", Meta = (Keywords = "C++"))
		int32 GetMenuLayer() const;


	/** Return whether menu can be closed */
	UFUNCTION(BlueprintPure, Category = "TGOR Interface", Meta = (Keywords = "C++"))
		bool CanClose();

	/** Return if menu is open */
	UFUNCTION(BlueprintPure, Category = "TGOR Interface", Meta = (Keywords = "C++"))
		bool IsMenuOpen();

	/** Called after creation */
	UFUNCTION(BlueprintCallable, Category = "TGOR Interface", Meta = (Keywords = "C++"))
		void InitialiseMenu(int32 Layer);

	/** Opens menu */
	UFUNCTION(BlueprintCallable, Category = "TGOR Interface", Meta = (Keywords = "C++"))
		virtual void OpenMenu(UTGOR_Query* Query);
		void StartOpenMenu(UTGOR_Query* Query);
		void FinishOpenMenu();

	/** Closes menu */
	UFUNCTION(BlueprintCallable, Category = "TGOR Interface", Meta = (Keywords = "C++"))
		virtual void CloseMenu();
		void StartCloseMenu();
		void FinishCloseMenu();
	
	/** Plays closing/opening animation */
	UFUNCTION(BlueprintCallable, Category = "TGOR Interface", Meta = (Keywords = "C++"))
		void AnimateMenu(const FTGOR_MenuAnimation& Animation, UPARAM(ref) bool& State, bool Active);
	
	////////////////////////////////////////////////////////////////////////////////////////////////////

	/** Short key press */
	UFUNCTION(BlueprintImplementableEvent, Category = "TGOR Interface", Meta = (Keywords = "C++"))
		void ShortKey(FKey Key);

	/** key hold */
	UFUNCTION(BlueprintImplementableEvent, Category = "TGOR Interface", Meta = (Keywords = "C++"))
		void LongKey(FKey Key);

	/////////////////////////////////////////////// INTERNAL ///////////////////////////////////////////

private:

	bool ClosePending;
	bool OpenPending;
	bool IsOpen;

};
