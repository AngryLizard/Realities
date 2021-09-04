// The Gateway of Realities: Planes of Existence.

#pragma once

#include "../TGOR_ProcessInstance.h"
#include "../Storage/TGOR_StorageInstance.h"

#include "CoreSystem/Content/TGOR_CoreContent.h"
#include "TGOR_Process.generated.h"

class UTGOR_Matter;
class UTGOR_ProcessStorage;
class UTGOR_ProcessComponent;
class UTGOR_ContainerComponent;

/**
 * 
 */
UCLASS()
class INVENTORYSYSTEM_API UTGOR_Process : public UTGOR_CoreContent
{
	GENERATED_BODY()
	
public:
	UTGOR_Process();

	////////////////////////////////////////////////////////////////////////////////////////////////////
public:

	/**
	* Create an empty storage instance for this item.
	* @see TGOR_Storage
	*/
	UFUNCTION(BlueprintCallable, Category = "!TGOR Inventory", Meta = (Keywords = "C++"))
		virtual UTGOR_ProcessStorage* CreateStorage();

	/** Build Storage modules and set their default values */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Inventory", Meta = (Keywords = "C++"))
		virtual void BuildStorage(UTGOR_ProcessStorage* Storage);

	/** Change values and module values of a created storage object */
	UFUNCTION(BlueprintImplementableEvent, Category = "!TGOR Inventory", Meta = (Keywords = "C++"))
		void InitialiseStorage(UTGOR_ProcessStorage* Storage);

	////////////////////////////////////////////////////////////////////////////////////////////////////
public:

	/** Tick this science with given storage and owner, return action to be performed by container after tick. */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Inventory", Meta = (Keywords = "C++"))
		virtual void Tick(UTGOR_DimensionComponent* Owner, UTGOR_ProcessStorage* Storage, float DeltaSeconds);

	/** Process a given amount of antimatter, return what was actually used. */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Inventory", Meta = (Keywords = "C++"))
		virtual float Process(UTGOR_ProcessComponent* Owner, UTGOR_ProcessStorage* Storage, float Antimatter);

	/** Check whether a process is valid for a given owner. */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Inventory", Meta = (Keywords = "C++"))
		virtual bool CanProcess(UTGOR_ProcessComponent* Owner, UTGOR_ProcessStorage* Storage) const;

	////////////////////////////////////////////////////////////////////////////////////////////////////
public:

	/** Modules that are part of this science storage */
	UPROPERTY(EditAnywhere, Category = "!TGOR Inventory")
		FTGOR_StorageDeclaration Modules;

protected:

};
