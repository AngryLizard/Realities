// TGOR (C) // CHECKED //
#pragma once

#include "CoreMinimal.h"

#include "RealitiesUGC/Mod/TGOR_ModRegistry.h"
#include "RealitiesUGC/Mod/TGOR_ModInstance.h"

#include "CoreSystem/Interfaces/TGOR_SingletonInterface.h"
#include "CoreSystem/Components/TGOR_Component.h"
#include "TGOR_ContentVirtualisationComponent.generated.h"

class UTGOR_ContentVirtualisationWidget;
class UTGOR_ContentVirtualisationSingleton;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FContentVirtualisationDelegate);

/**
 * Provides sandboxing a TGOR_Singleton e.g. for ContentManagement within editor
 */
UCLASS(ClassGroup = (Custom), Blueprintable, meta = (BlueprintSpawnableComponent))
class CORECONTENT_API UTGOR_ContentVirtualisationComponent : public UTGOR_Component
{
	GENERATED_BODY()


public:

	////////////////////////////////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////// ENGINE ////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////////////////////////////

	UTGOR_ContentVirtualisationComponent();

	//////////////////////////////////////////// IMPLEMENTABLES ////////////////////////////////////////


	UPROPERTY(BlueprintAssignable, Category = "!TGOR System")
		FContentVirtualisationDelegate OnVirtualisation;

	////////////////////////////////////////////////////////////////////////////////////////////////////
public:

	/** Mod setup to be loaded */
	UPROPERTY(BlueprintReadOnly, Category = "!TGOR System")
		FTGOR_ModInstance ModSetup;

	/** Widget type to be used for this component */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "!TGOR System")
		TSubclassOf<UTGOR_ContentVirtualisationWidget> WidgetClass;

	/** CoreMod to be virtualised */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "!TGOR System")
		TSubclassOf<UTGOR_Mod> VirtualisedMod;

	/** Component class to spawn children of */
	UPROPERTY(Transient, BlueprintReadOnly, Category = "!TGOR System")
		UTGOR_ContentVirtualisationSingleton* VirtualSingleton;

	/** The Registry that holds information about UGC and assigned class overrides */
	UPROPERTY(Transient, BlueprintReadOnly, Category = "!TGOR System")
		UTGOR_ModRegistry* VirtualRegistry;

	/** Create and initialise widget */
	UFUNCTION(BlueprintCallable, Category = "!TGOR System", Meta = (Keywords = "C++"))
		UTGOR_ContentVirtualisationWidget* CreateVirtualisationWidget();

	/** Virtualise singleton for owning actor and all components */
	UFUNCTION(BlueprintCallable, Category = "!TGOR System", Meta = (Keywords = "C++"))
		void Virtualise(bool Refresh = false);

	/////////////////////////////////////////////// INTERNAL ///////////////////////////////////////////
	   	
private:

};
