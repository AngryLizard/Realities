// The Gateway of Realities: Planes of Existence.

#pragma once

#include "CoreMinimal.h"

#include "UObject/Interface.h"
#include "TGOR_MatterInterface.generated.h"

////////////////////////////////////////////// DECL //////////////////////////////////////////////////////

class UTGOR_Matter;
class UTGOR_Section;

UINTERFACE(MinimalAPI, meta = (CannotImplementInterfaceInBlueprint))
class UTGOR_MatterInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class REALITIES_API ITGOR_MatterInterface
{
	GENERATED_BODY()

public:

	/** Get currently available matter. */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Inventory", Meta = (Keywords = "C++"))
		virtual TMap<UTGOR_Matter*, int32> GetComposition(TSubclassOf<UTGOR_Section> Filter);

	/** Get currently missing matter from full capacity */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Inventory", Meta = (Keywords = "C++"))
		virtual TMap<UTGOR_Matter*, int32> GetMissing(TSubclassOf<UTGOR_Section> Filter);

	/** Get matter storage capacity of this container */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Inventory", Meta = (Keywords = "C++"))
		virtual TMap<UTGOR_Matter*, int32> GetCapacity(TSubclassOf<UTGOR_Section> Filter);

	/** Gets by this container supported matter types */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Inventory", Meta = (Keywords = "C++"))
		virtual TSet<UTGOR_Matter*> GetSupported(TSubclassOf<UTGOR_Section> Filter);

	////////////////////////////////////////////////////////////////////////////////////////////////////
public:

	/** Remove matter quantities by given composition, returns what actuall got removed */
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "!TGOR Inventory", Meta = (Keywords = "C++"))
		virtual TMap<UTGOR_Matter*, int32> PayStorageMatter(TSubclassOf<UTGOR_Section> Filter, const TMap<UTGOR_Matter*, int32>& Composition);

	/** Add matter quantities by given composition, returns leftovers */
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "!TGOR Inventory", Meta = (Keywords = "C++"))
		virtual TMap<UTGOR_Matter*, int32> InvestStorageMatter(TSubclassOf<UTGOR_Section> Filter, const TMap<UTGOR_Matter*, int32>& Composition);

};
