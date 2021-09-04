// The Gateway of Realities: Planes of Existence.

#pragma once

#include "RealitiesUGC/Mod/TGOR_ModInstance.h"

#include "Runtime/GameplayTags/Classes/GameplayTagContainer.h"
#include "RealitiesUtility/Utility/TGOR_WorldObject.h"
#include "TGOR_ModSetup.generated.h"


UENUM(BlueprintType)
enum class ETGOR_SetupOwnerEnumeration : uint8
{
	/** Everyone */
	All,
	/** Only clients */
	ClientOnly,
	/** Only server */
	ServerOnly
};


/**
* UTGOR_ModSetup is part of the loading process and gets called in order on game start
*/
UCLASS()
class REALITIESUGC_API UTGOR_ModSetup : public UTGOR_WorldObject
{
	GENERATED_BODY()

public:
	UTGOR_ModSetup();

	////////////////////////////////////////////////////////////////////////////////////////////////////

	/** Last state of this setup step */
	UPROPERTY(BlueprintReadOnly, Category = "!TGOR System")
		FTGOR_ModSetupState State;

	/** Name of this setup step */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "!TGOR System")
		FText SetupName;

	/** Whether this setup is only relevant for server */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "!TGOR System")
		ETGOR_SetupOwnerEnumeration SetupOwner;

	/** Optional parent setup (first match in loading order) this task is inserted after. If None the setup is inserted at the end of the loading chain.
		Tasks with the same parent task are loaded in reverse loading order after the parent task (i.e. newest first).*/
	UPROPERTY(BlueprintReadOnly, Category = "!TGOR System")
		TSubclassOf<UTGOR_ModSetup> ParentSetup;

	/** Set current status loading text */
	UFUNCTION(BlueprintCallable, Category = "!TGOR System", Meta = (Keywords = "C++"))
		void SetLoadingStatus(FText Text);

	/** Actually invoke setup, return if step is done */
	UFUNCTION(BlueprintNativeEvent)
		bool Attempt(bool IsServer);
	virtual bool Attempt_Implementation(bool IsServer);

	////////////////////////////////////////////////////////////////////////////////////////////////////
protected:

	/** Get local player controller (we assume there is only one)*/
	UFUNCTION(BlueprintPure, Category = "!TGOR System|Internal", Meta = (Keywords = "C++"))
		APlayerController* GetLocalPlayerController() const;
};
