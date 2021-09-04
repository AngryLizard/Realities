// The Gateway of Realities: Planes of Existence.

#pragma once

#include "CoreMinimal.h"
#include "CoreSystem/Utility/TGOR_CoreEnumerations.h"

#include "UObject/Interface.h"
#include "TGOR_DimensionInterface.generated.h"

class UTGOR_DimensionData;
class UTGOR_Dimension;

UINTERFACE(MinimalAPI, meta = (CannotImplementInterfaceInBlueprint))
class UTGOR_DimensionInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class DIMENSIONSYSTEM_API ITGOR_DimensionInterface
{
	GENERATED_BODY()

public:

	/** Called before or while executing loaders */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Game", Meta = (Keywords = "C++"))
		virtual bool PreAssemble(UTGOR_DimensionData* Dimension);

	/** Called after all loaders executed */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Game", Meta = (Keywords = "C++"))
		virtual bool Assemble(UTGOR_DimensionData* Dimension);

	/** Called after all DimensionActors and DimensionComponents have been assembled */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Game", Meta = (Keywords = "C++"))
		virtual bool PostAssemble(UTGOR_DimensionData* Dimension);

	/** Gets own dimension identifier */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Game", Meta = (ExpandEnumAsExecs = "Branches", Keywords = "C++"))
		virtual FName GetDimensionIdentifier(ETGOR_FetchEnumeration& Branches) const;

	/** Gets own dimension data */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Game", Meta = (ExpandEnumAsExecs = "Branches", Keywords = "C++"))
		virtual UTGOR_DimensionData* GetDimension(ETGOR_FetchEnumeration& Branches) const;

protected:
	/** Loaded DimensionData */
	TWeakObjectPtr<UTGOR_DimensionData> DimensionData;
};
