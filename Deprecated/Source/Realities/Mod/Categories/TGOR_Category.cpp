// The Gateway of Realities: Planes of Existence. By Salireths.

#include "TGOR_Category.h"
#include "Realities/Base/TGOR_Singleton.h"
#include "Realities/Base/Content/TGOR_ContentManager.h"

UTGOR_Category::UTGOR_Category()
:	Super()
{
}

void UTGOR_Category::PostBuildResource()
{
	Super::PostBuildResource();
	BuildCategoryInsertions(this);

	if (InsertionClasses.Num() > 0)
	{
		ERROR("Categories do not support insertions on their own.", Error);
	}
}


void UTGOR_Category::BuildCategoryInsertions(UTGOR_Content* Category)
{
	// No need to rebuild if already constructed
	if (Category->Insertions.Num() > 0)
	{
		return;
	}

	// Look for all content classes that contain this category
	SINGLETON_CHK;
	UTGOR_ContentManager* ContentManager = Singleton->GetContentManager();
	for (UTGOR_Content* Content : ContentManager->ContentStorage)
	{
		if (Content->ContainsI(this))
		{
			Category->Insertions.Emplace(Content);
		}
	}

	// Get contents from all children
	for (UTGOR_Content* Sub : SubContent)
	{
		BuildCategoryInsertions(Sub);
		Category->Insertions.Append(Sub->Insertions);
	}
}