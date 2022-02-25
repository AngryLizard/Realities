#pragma once

#include "CoreMinimal.h"
#include "Realities/Base/Instances/System/TGOR_ActionInstance.h"

#include "Realities/Base/Inventory/Modules/TGOR_Module.h"
#include "TGOR_EventModule.generated.h"

class UTGOR_Event;

UENUM(BlueprintType)
enum class ETGOR_EventEnumeration : uint8
{
	Running,
	Dead
};

/**
 * 
 */
UCLASS(BlueprintType, Blueprintable)
class REALITIES_API UTGOR_EventModule : public UTGOR_Module
{
	GENERATED_BODY()

public:

	UTGOR_EventModule();
	virtual bool Equals(const UTGOR_Module* Other) const override;
	virtual bool Merge(const UTGOR_Module* Other, ETGOR_NetvarEnumeration Priority) override;
	virtual void BuildModule(const UTGOR_Storage* Parent) override;

	virtual void Write_Implementation(FTGOR_GroupWritePackage& Package, UTGOR_Singleton* Context) const override;
	virtual bool Read_Implementation(FTGOR_GroupReadPackage& Package, UTGOR_Singleton* Context) override;
	virtual void Send_Implementation(FTGOR_NetworkWritePackage& Package, UTGOR_Singleton* Context) const override;
	virtual bool Recv_Implementation(FTGOR_NetworkReadPackage& Package, UTGOR_Singleton* Context) override;

	virtual void Clear() override;
	virtual FString Print() const override;
	virtual void Post() override;


	////////////////////////////////////////////////////////////////////////////////////////////////////

	/** Set event values. */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Storage", Meta = (Keywords = "C++"))
		void PopulateEvents(const TArray<UTGOR_Event*>& Register);

	////////////////////////////////////////////////////////////////////////////////////////////////////
	
	/** Add an event type to the queue. Only one event of the same type can exist in the queue. */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Storage", Meta = (Keywords = "C++"))
		void ScheduleEvent(TSubclassOf<UTGOR_Event> Event);

	/** Checks an event type for whether it is currently active or not */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Storage", Meta = (ExpandEnumAsExecs = "Branches", Keywords = "C++"))
		void PeekEvent(TSubclassOf<UTGOR_Event> Event, ETGOR_EventEnumeration& Branches);

	/** Checks and remove event type from the queue for whether it is currently active or not */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Storage", Meta = (ExpandEnumAsExecs = "Branches", Keywords = "C++"))
		void ConsumeEvent(TSubclassOf<UTGOR_Event> Event, ETGOR_EventEnumeration& Branches);

	////////////////////////////////////////////////////////////////////////////////////////////////////
private:

	/** Mask of activated events */
	UPROPERTY()
		TArray<bool> Activated;

	/** Keeps track of events after consumption for replication till next tick */
	UPROPERTY()
		TArray<bool> Prompted;

	/** List of possible events */
	UPROPERTY()
		TArray<UTGOR_Event*> Events;

};