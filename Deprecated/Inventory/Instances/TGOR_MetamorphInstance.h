// The Gateway of Realities: Planes of Existence.

#pragma once

#include "Base/Instances/Dimension/TGOR_RegionInstance.h"
#include "Base/Instances/Dimension/TGOR_ElementInstance.h"
#include "Utility/TGOR_CustomEnumerations.h"

#include "Engine/UserDefinedStruct.h"
#include "TGOR_MetamorphInstance.generated.h"

class UTGOR_ContainerComponent;
class UTGOR_Metamorph;
class UTGOR_ContentManager;


/**
* FTGOR_MetamorphRecipe stores metamorph index pairs
*/
USTRUCT(BlueprintType)
struct FTGOR_MetamorphRecipe
{
	GENERATED_USTRUCT_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TGOR|Attributes")
		UTGOR_Metamorph* Metamorph;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TGOR|Attributes")
		int32 Recipe;
};

/**
* TGOR_MetamorphEntryInstance stores information about one item
*/
USTRUCT(BlueprintType)
struct FTGOR_MetamorphEntryInstance
{
	GENERATED_USTRUCT_BODY()

public:
	FTGOR_MetamorphEntryInstance();

	////////////////////////////////////////////////////////////////////////////////////////////////////
	
	// Item class
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TGOR|Interaction")
		TSubclassOf<UTGOR_Item> Item;

	// Entry description
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TGOR|Interaction")
		FText Description;

	// Also accepts children if true
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TGOR|Interaction")
		bool AllowChildren;

	// Cached item instance
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "TGOR|Interaction")
		FTGOR_ItemInstance_OLD Instance;
};

/**
* TGOR_MetamorphInstance stores information about how an item should be metamorphed
*/
USTRUCT(BlueprintType)
struct FTGOR_MetamorphInstance
{
	GENERATED_USTRUCT_BODY()

public:
	FTGOR_MetamorphInstance();

	bool Satisfies(const FTGOR_ElementInstance& Instance) const;

	////////////////////////////////////////////////////////////////////////////////////////////////////

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TGOR|Interaction")
		FTGOR_MetamorphEntryInstance Input;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TGOR|Interaction")
		TArray<FTGOR_MetamorphEntryInstance> Output;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TGOR|Interaction")
		FTGOR_ElementInstance Min;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TGOR|Interaction")
		FTGOR_ElementInstance Max;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TGOR|Interaction")
		float Time;
};


/**
* TGOR_MetamorphInstance stores information about how an item should be metamorphed
*/
USTRUCT(BlueprintType)
struct FTGOR_CombinationInstance
{
	GENERATED_USTRUCT_BODY()

public:
	FTGOR_CombinationInstance();

	////////////////////////////////////////////////////////////////////////////////////////////////////

	// Required input items
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TGOR|Interaction")
		TArray<FTGOR_MetamorphEntryInstance> Input;

	// Craft description
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TGOR|Interaction")
		FText Description;

	// Output items
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TGOR|Interaction")
		TArray<FTGOR_MetamorphEntryInstance> Output;
};