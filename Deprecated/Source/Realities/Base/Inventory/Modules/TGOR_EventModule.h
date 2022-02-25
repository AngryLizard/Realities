#pragma once

#include "CoreMinimal.h"
#include "Realities/Base/Instances/System/TGOR_ActionInstance.h"

#include "Realities/Base/Inventory/Modules/TGOR_Module.h"
#include "TGOR_EventModule.generated.h"

class UTGOR_Event;

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
	virtual void Tick(UTGOR_DimensionComponent* Owner, float DeltaSeconds) override;

	////////////////////////////////////////////////////////////////////////////////////////////////////
	
	/** Activate an event type */
	UFUNCTION(BlueprintCallable, Category = "TGOR Actions", Meta = (Keywords = "C++"))
		void ScheduleEvent(TSubclassOf<UTGOR_Event> Event);

	/** Checks an event type for whether it is currently active or not */
	UFUNCTION(BlueprintCallable, Category = "TGOR Actions", Meta = (ExpandEnumAsExecs = "Branches", Keywords = "C++"))
		void PeekEvent(TSubclassOf<UTGOR_Event> Event, ETGOR_EventEnumeration& Branches);

	/** Checks and reset an event type for whether it is currently active or not */
	UFUNCTION(BlueprintCallable, Category = "TGOR Actions", Meta = (ExpandEnumAsExecs = "Branches", Keywords = "C++"))
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