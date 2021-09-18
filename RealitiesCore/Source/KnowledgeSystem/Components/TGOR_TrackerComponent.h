// The Gateway of Realities: Planes of Existence.

#pragma once

#include "KnowledgeSystem/TGOR_TrackerInstance.h"

#include "CoreSystem/Interfaces/TGOR_RegisterInterface.h"
#include "DimensionSystem/Components/TGOR_DimensionComponent.h"
#include "TGOR_TrackerComponent.generated.h"

class UTGOR_Tracker;

/**
* TGOR_TrackerComponent gives an actor the ability to apply trackers
*/
UCLASS(ClassGroup = (Custom), Blueprintable, meta = (BlueprintSpawnableComponent))
class KNOWLEDGESYSTEM_API UTGOR_TrackerComponent : public UTGOR_DimensionComponent
{
	GENERATED_BODY()

public:	
	UTGOR_TrackerComponent();
	virtual bool PreAssemble(UTGOR_DimensionData* Dimension) override;

	////////////////////////////////////////////////////////////////////////////////////////////////////
public:

	/** Clears and adds content classes to context. */
	UFUNCTION(BlueprintPure, Category = "!TGOR Knowledge", Meta = (Keywords = "C++"))
		TSet<UTGOR_CoreContent*> GetContext() const;

	/** Registers tracker information to this pawn with given context appendum. */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Knowledge", Meta = (Keywords = "C++"))
		void PutTracker(TSubclassOf<UTGOR_Tracker> Type, const TSet<UTGOR_CoreContent*>& Appendum, float Delta = 1.0f);

	/** Flushes tracker cache into another tracker database */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Knowledge|Internal", Meta = (Keywords = "C++"))
		void FlushTracker(UPARAM(ref) FTGOR_TrackerData& Target);

	////////////////////////////////////////////////////////////////////////////////////////////////////
protected:

	/** Registered tracker values */
	UPROPERTY(BlueprintReadOnly, Category = "!TGOR Knowledge", meta = (Keywords = "C++"))
		FTGOR_TrackerData Trackers;

	/** Associated Register objects */
	UPROPERTY(BlueprintReadOnly, Category = "!TGOR Knowledge")
		TArray<TScriptInterface<ITGOR_RegisterInterface>> RegisterObjects;
};
