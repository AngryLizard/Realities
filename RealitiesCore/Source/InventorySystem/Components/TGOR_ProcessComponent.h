// TGOR (C) // CHECKED //
#pragma once

#include "CoreMinimal.h"
#include "RealitiesUtility/Structures/TGOR_Time.h"

#include "../TGOR_ProcessInstance.h"

#include "TGOR_ContainerComponent.h"
#include "TGOR_ProcessComponent.generated.h"


////////////////////////////////////////////// DECL //////////////////////////////////////////////////////

class UTGOR_ItemStorage;
class UTGOR_Matter;
class UTGOR_Item;


///////////////////////////////////////////// DELEGATES ///////////////////////////////////////////////////

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FProcessTerminalDelegate);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FProcessMassDelegate, float, Mass);


/**
 * TGOR_ProcessComponent manages processing
 */
UCLASS(ClassGroup = (Custom), Blueprintable, meta = (BlueprintSpawnableComponent))
class INVENTORYSYSTEM_API UTGOR_ProcessComponent : public UTGOR_ContainerComponent
{
	GENERATED_BODY()

public:

	////////////////////////////////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////// ENGINE ////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////////////////////////////

	UTGOR_ProcessComponent();

	void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	void GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const override;

	//////////////////////////////////////////// IMPLEMENTABLES ////////////////////////////////////////

	/** Called when a batch of antimatter was generated  */
	UPROPERTY(BlueprintAssignable, Category = "!TGOR Inventory")
		FProcessMassDelegate OnAntimatterGenerate;

	/** Called when antimatted is drained due to invalid process configuration  */
	UPROPERTY(BlueprintAssignable, Category = "!TGOR Inventory")
		FProcessMassDelegate OnAntimatterOutlet;

	////////////////////////////////////////////////////////////////////////////////////////////////////
protected:

	/** Mass of antimater that is produced per second  */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "!TGOR Inventory")
		float ProcessingRate;

	////////////////////////////////////////////////////////////////////////////////////////////////////
protected:

	/** Amount of antimatter required for one processing tick  */
	UPROPERTY(EditAnywhere, Replicated, BlueprintReadOnly, Category = "!TGOR Inventory")
		float ProcessAntimatter;

	/** Last time processing has been triggered */
	UPROPERTY(EditAnywhere, Replicated, BlueprintReadOnly, Category = "!TGOR Inventory")
		FTGOR_Time LastProcessing;

	////////////////////////////////////////////////////////////////////////////////////////////////////
public:

	/** Schedules a process in this component */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Inventory", Meta = (Keywords = "C++"))
		void ScheduleProcess(UTGOR_Process* Process);

	/** Cancels current running process */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Inventory", Meta = (Keywords = "C++"))
		void CancelProcess();

	////////////////////////////////////////////////////////////////////////////////////////////////////
protected:

	/** Currently active process */
	UPROPERTY(EditAnywhere, Replicated, BlueprintReadOnly, Category = "!TGOR Inventory")
		FTGOR_ProcessInstance ProcessState;

	/** Whether the current process is valid */
	UFUNCTION(BlueprintPure, Category = "!TGOR Inventory", Meta = (Keywords = "C++"))
		bool HasRunningProcess() const;

	/** Whether the current process is valid and of a given type */
	UFUNCTION(BlueprintPure, Category = "!TGOR Inventory", Meta = (Keywords = "C++"))
		bool IsRunningProcess(TSubclassOf<UTGOR_Process> Type) const;


	/////////////////////////////////////////////// INTERNAL ///////////////////////////////////////////

protected:

private:

};
