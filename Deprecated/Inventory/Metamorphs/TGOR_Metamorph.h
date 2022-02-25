// The Gateway of Realities: Planes of Existence.

#pragma once

#include "Base/Instances/Inventory/TGOR_ItemInstance_OLD.h"
#include "Base/Instances/Inventory/TGOR_MetamorphInstance.h"
#include "Utility/TGOR_Math.h"

#include "Base/Content/TGOR_Content.h"
#include "TGOR_Metamorph.generated.h"


class UTGOR_Item;
class UTGOR_MetamorphComponent;

/**
 * 
 */
UCLASS()
class REALITIES_API UTGOR_Metamorph : public UTGOR_Content
{
	GENERATED_BODY()
	
public:
	UTGOR_Metamorph();

	void PostBuildResource() override;
	void BuildInstance(	FTGOR_MetamorphEntryInstance& Entry, int32 Index, void (UTGOR_Metamorph::*Build)(int32, FTGOR_ItemInstance_OLD&), TArray<FTGOR_MetamorphRecipe> UTGOR_Item::*Array);

	/** Metamorph recipes of this metamorph */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "TGOR|Attributes")
		TArray<FTGOR_MetamorphInstance> MetamorphRecipes;

	/** Combination recipes of this metamorph */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "TGOR|Attributes")
		TArray<FTGOR_CombinationInstance> CombinationRecipes;

	/** Returns if metamorph can be called in this context */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "TGOR|Attributes", Meta = (Keywords = "C++"))
		bool InputFilter(int32 Index, UTGOR_MetamorphComponent* Component, const FTGOR_ItemInstance_OLD& Instance) const;
		virtual bool InputFilter_Implementation(int32 Index, UTGOR_MetamorphComponent* Component, const FTGOR_ItemInstance_OLD& Instance) const;

	/** Ouput payload */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "TGOR|Attributes", Meta = (Keywords = "C++"))
		void OutputPayload(int32 Index, UTGOR_MetamorphComponent* Component, UPARAM(ref) FTGOR_ItemInstance_OLD& Instance);
		virtual void OutputPayload_Implementation(int32 Index, UTGOR_MetamorphComponent* Component, UPARAM(ref) FTGOR_ItemInstance_OLD& Instance);

	/** Called on output of a whole recipe */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "TGOR|Attributes", Meta = (Keywords = "C++"))
		void Output(int32 Index, UTGOR_MetamorphComponent* Component, AActor* Instigator);
		virtual void Output_Implementation(int32 Index, UTGOR_MetamorphComponent* Component, AActor* Instigator);


	/** Builds the input instance (index already set) */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "TGOR|Attributes", Meta = (Keywords = "C++"))
		void BuildInput(int32 Index, FTGOR_ItemInstance_OLD& Instance);
		virtual void BuildInput_Implementation(int32 Index, FTGOR_ItemInstance_OLD& Instance);

	/** Builds the output instance (index already set) */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "TGOR|Attributes", Meta = (Keywords = "C++"))
		void BuildOutput(int32 Index, FTGOR_ItemInstance_OLD& Instance);
		virtual void BuildOutput_Implementation(int32 Index, FTGOR_ItemInstance_OLD& Instance);
};