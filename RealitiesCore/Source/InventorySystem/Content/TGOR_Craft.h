// The Gateway of Realities: Planes of Existence.

#pragma once

#include "TGOR_MatterProcess.h"
#include "TGOR_Craft.generated.h"

class UTGOR_StorageComponent;
class UTGOR_MatterComponent;

/**
 * 
 */
UCLASS()
class INVENTORYSYSTEM_API UTGOR_Craft : public UTGOR_MatterProcess
{
	GENERATED_BODY()
	
public:
	UTGOR_Craft();

	virtual float Process(UTGOR_ProcessComponent* Owner, UTGOR_ProcessStorage* Storage, float Antimatter) override;
	virtual bool CanProcess(UTGOR_ProcessComponent* Owner, UTGOR_ProcessStorage* Storage) const override;

	////////////////////////////////////////////////////////////////////////////////////////////////////
public:

	/** Tick this craft with given storage and owner, return actually used antimatter after process has concluded. */
	UFUNCTION(BlueprintImplementableEvent, Category = "!TGOR Inventory", Meta = (Keywords = "C++"))
		float OnProcess(UTGOR_StorageComponent* Sockets, UTGOR_MatterComponent* Matter, UTGOR_ProcessStorage* Storage, float Antimatter);

	/** Check whether this craft is still valid to be processed. */
	UFUNCTION(BlueprintImplementableEvent, Category = "!TGOR Inventory", Meta = (Keywords = "C++"))
		bool OnCanProcess(UTGOR_StorageComponent* Sockets, UTGOR_MatterComponent* Matter, UTGOR_ProcessStorage* Storage) const;


};
