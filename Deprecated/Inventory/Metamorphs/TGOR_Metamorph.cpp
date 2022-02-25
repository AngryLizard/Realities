// The Gateway of Realities: Planes of Existence.

#include "TGOR_Metamorph.h"
#include "Realities.h"

#include "Base/Content/TGOR_ContentManager.h"
#include "Base/TGOR_Singleton.h"
#include "Mod/Items/TGOR_Item.h"


UTGOR_Metamorph::UTGOR_Metamorph()
	: Super()
{
}

void UTGOR_Metamorph::PostBuildResource()
{
	// TAKEN OUT BY LHYKOS FOR COMPATIBILITY
	/*int32 MetamorphNum = MetamorphRecipes.Num();
	for (int i = 0; i < MetamorphNum; i++)
	{
		FTGOR_MetamorphInstance& Instance = MetamorphRecipes[i];

		// Initialise input
		BuildInstance(Instance.Input, i, &UTGOR_Metamorph::BuildInput, &UTGOR_Item::MetamorphRecipeInputList);

		// Initialise output
		for (FTGOR_MetamorphEntryInstance& Entry : Instance.Output)
		{
			BuildInstance(Entry, i, &UTGOR_Metamorph::BuildInput, &UTGOR_Item::MetamorphRecipeOutputList);
		}
	}

	int32 CombinationhNum = CombinationRecipes.Num();
	for (int i = 0; i < CombinationhNum; i++)
	{
		FTGOR_CombinationInstance& Instance = CombinationRecipes[i];

		// Initialise input
		for (FTGOR_MetamorphEntryInstance& Entry : Instance.Input)
		{
			BuildInstance(Entry, i, &UTGOR_Metamorph::BuildInput, &UTGOR_Item::CombinationRecipeInputList);
		}

		// Initialise output
		for (FTGOR_MetamorphEntryInstance& Entry : Instance.Output)
		{
			BuildInstance(Entry, i, &UTGOR_Metamorph::BuildInput, &UTGOR_Item::CombinationRecipeOutputList);
		}
	}*/
}

void UTGOR_Metamorph::BuildInstance(FTGOR_MetamorphEntryInstance& Entry, int32 Index, void (UTGOR_Metamorph::*Build)(int32, FTGOR_ItemInstance_OLD&), TArray<FTGOR_MetamorphRecipe> UTGOR_Item::*Array)
{
	// TAKEN OUT BY LHYKOS
	/*Entry.Instance.Content = Singleton->ContentManager->GetByKey(Entry.Item);
	if (IsValid(Entry.Instance.Content))
	{
		// Set common recipe fields
		FTGOR_MetamorphRecipe Recipe;
		Recipe.Metamorph = this;
		Recipe.Recipe = Index;

		// Build instance
		(this->*Build)(Index, Entry.Instance);

		// Register inside items used
		if (Entry.AllowChildren)
		{
			TArray<UTGOR_Item*> List = Singleton->ContentManager->GetList(Entry.Item);
			for (UTGOR_Item* Sub : List)
			{
				(Sub->*Array).Add(Recipe);
			}
		}
		else
		{
			(Entry.Instance.Content->*Array).Add(Recipe);
		}
	}*/
}

bool UTGOR_Metamorph::InputFilter_Implementation(int32 Index, UTGOR_MetamorphComponent* Component, const FTGOR_ItemInstance_OLD& Instance) const
{
	return(true);
}

void UTGOR_Metamorph::OutputPayload_Implementation(int32 Index, UTGOR_MetamorphComponent* Component, FTGOR_ItemInstance_OLD& Instance)
{

}


void UTGOR_Metamorph::Output_Implementation(int32 Index, UTGOR_MetamorphComponent* Component, AActor* Instigator)
{

}


void UTGOR_Metamorph::BuildInput_Implementation(int32 Index, FTGOR_ItemInstance_OLD& Instance)
{

}

void UTGOR_Metamorph::BuildOutput_Implementation(int32 Index, FTGOR_ItemInstance_OLD& Instance)
{

}