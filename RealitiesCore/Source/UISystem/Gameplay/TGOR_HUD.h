// The Gateway of Realities: Planes of Existence.

#pragma once

#include "Blueprint/UserWidget.h"

#include "CoreSystem/Interfaces/TGOR_RegisterInterface.h"
#include "CoreSystem/Interfaces/TGOR_SingletonInterface.h"
#include "GameFramework/HUD.h"
#include "TGOR_HUD.generated.h"

////////////////////////////////////////////// DECL //////////////////////////////////////////////////////

class UTGOR_Menu;
class UTGOR_Overlay;
class ATGOR_GateActor;
class UTGOR_InteractionOverlay;
class UTGOR_Widget;

////////////////////////////////////////////// STRUCT //////////////////////////////////////////////////////

USTRUCT(BlueprintType)
struct FTGOR_MenuPair
{
	GENERATED_USTRUCT_BODY()
		FTGOR_MenuPair();
	FTGOR_MenuPair(UTGOR_Overlay* Overlay, UTGOR_Menu* Menu);

	/** Menu interface */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "!TGOR Camera")
		UTGOR_Menu* Menu;

	/** Overlay content */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "!TGOR Camera")
		UTGOR_Overlay* Overlay;

};

/**
* TGOR_HUD provides functionality to hold and maintain menus
*/
UCLASS()
class UISYSTEM_API ATGOR_HUD : public AHUD, public ITGOR_SingletonInterface, public ITGOR_RegisterInterface
{
	GENERATED_BODY()

	friend class SGATGORNode_MenuParamPin;

public:

	////////////////////////////////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////// ENGINE ////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////////////////////////////

	ATGOR_HUD();

	virtual void Tick(float DeltaTime) override;
	virtual void BeginPlay() override;

	virtual TSet<UTGOR_CoreContent*> GetActiveContent_Implementation() const override;

	//////////////////////////////////////////// IMPLEMENTABLES ////////////////////////////////////////

	/** */

	////////////////////////////////////////////////////////////////////////////////////////////////////
protected:

	/** Menus managed by this HUD */
	UPROPERTY(BlueprintReadOnly, Category = "!TGOR Interface")
		TArray<FTGOR_MenuPair> Menus;

	/** Escape menu name */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "!TGOR Interface")
		TSubclassOf<UTGOR_Overlay> EscapeMenu;

	/** Interaction overlay menu name */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "!TGOR Interface")
		TSubclassOf<UTGOR_Overlay> InteractionOverlayMenu;

	////////////////////////////////////////////////////////////////////////////////////////////////////
public:

	/** Opens a given menu */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Interface", Meta = (BlueprintProtected, Keywords = "C++"))
		void OpenMenu(TSubclassOf<UTGOR_Overlay> Type, UTGOR_Query* Query = nullptr);

	/** Opens a given menu from a given HUD class */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Interface", Meta = (WorldContext = "WorldContext", Keywords = "C++"))
		static void OpenWorldMenu(UObject* WorldContext, TSubclassOf<UTGOR_Overlay> Type, UTGOR_Query* Query);

	/** Closes a given menu */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Interface", Meta = (Keywords = "C++"))
		void CloseMenu(TSubclassOf<UTGOR_Overlay> Type);

	/** Closes a given menu from a given HUD class */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Interface", Meta = (WorldContext = "WorldContext", Keywords = "C++"))
		static void CloseWorldMenu(UObject* WorldContext, TSubclassOf<UTGOR_Overlay> Type);

	/** Gets menu if exists */
	UFUNCTION(BlueprintPure, Category = "!TGOR Interface", Meta = (Keywords = "C++"))
		FTGOR_MenuPair GetMenu(TSubclassOf<UTGOR_Overlay> Type);

	/** Gets menu if exists */
	UFUNCTION(BlueprintPure, Category = "!TGOR Interface", Meta = (WorldContext = "WorldContext", Keywords = "C++"))
		static FTGOR_MenuPair GetWorldMenu(UObject* WorldContext, TSubclassOf<UTGOR_Overlay> Type);

	/** Gets current local HUD */
	UFUNCTION(BlueprintPure, Category = "!TGOR Interface", Meta = (WorldContext = "WorldContext", Keywords = "C++"))
		static ATGOR_HUD* GetWorldHUD(UObject* WorldContext);

	////////////////////////////////////////////////////////////////////////////////////////////////////
	
	/** Return true if any menu is open */
	UFUNCTION(BlueprintPure, Category = "!TGOR Interface", Meta = (Keywords = "C++"))
		bool IsInMenu(bool CloseableOnly);

	/** Return top-most menu that is open */
	UFUNCTION(BlueprintPure, Category = "!TGOR Interface", Meta = (Keywords = "C++"))
		UTGOR_Menu* GetTop(bool CloseableOnly);
		
	/** Escapes all active menus until none is left, then calls OpenEscapeMenu */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Interface", Meta = (Keywords = "C++"))
		void Escape(bool EscapeAll = false);

	/** Escapes all active menus */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Interface", Meta = (WorldContext = "WorldContext", Keywords = "C++"))
		static void WorldEscape(UObject* WorldContext, bool EscapeAll = false);

	/** Focus changed */
	UFUNCTION(BlueprintImplementableEvent, Category = "!TGOR Interface", Meta = (Keywords = "C++"))
		void FocusChanged();

	////////////////////////////////////////////////////////////////////////////////////////////////////
	
private:

};
