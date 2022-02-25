#pragma once

#include "CoreMinimal.h"

#include "Realities/Base/Inventory/TGOR_Container.h"
#include "TGOR_Module.generated.h"

/**
 * The abstract base class for all item modules.
 */
UCLASS(Abstract, Blueprintable)
class REALITIES_API UTGOR_Module : public UTGOR_Container
{
	GENERATED_BODY()

public:

	UTGOR_Module();
	
	/** Compare this module with another module. */
	UFUNCTION(BlueprintCallable, Category = "TGOR Storage", Meta = (Keywords = "C++"))
		virtual bool Equals(const UTGOR_Module* Other) const;

	/** Merge this module with another module. */
	UFUNCTION(BlueprintCallable, Category = "TGOR Storage", Meta = (Keywords = "C++"))
		virtual bool Merge(const UTGOR_Module* Other, ETGOR_NetvarEnumeration Priority);

	////////////////////////////////////////////////////////////////////////////////////////////////////

	/** Build module according to parent. */
	UFUNCTION(BlueprintCallable, Category = "TGOR Storage", Meta = (Keywords = "C++"))
		virtual void BuildModule(const UTGOR_Storage* Parent);

	/** Set module priority. */
	UFUNCTION(BlueprintCallable, Category = "TGOR Storage", Meta = (Keywords = "C++"))
		void SetPriority(ETGOR_NetvarEnumeration Priority);

	/** Whether this module is relevant for given priority. */
	UFUNCTION(BlueprintPure, Category = "TGOR Storage", Meta = (Keywords = "C++"))
		bool IsRelevant(ETGOR_NetvarEnumeration Priority) const;

	////////////////////////////////////////////////////////////////////////////////////////////////////
protected:

	/** Priority used for networking. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "TGOR Storage")
		ETGOR_NetvarEnumeration ModulePriority;
};
