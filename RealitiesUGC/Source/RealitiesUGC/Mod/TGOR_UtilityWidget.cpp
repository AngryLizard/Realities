// The Gateway of Realities: Planes of Existence.

#include "TGOR_UtilityWidget.h"

void UTGOR_UtilityWidget::AssignContentClass(TSubclassOf<UTGOR_Content> ContentClass)
{
	CurrentContent = ContentClass;
	if (*CurrentContent)
	{
		AssignContent(CurrentContent->GetDefaultObject<UTGOR_Content>());
	}
}

UTGOR_Content* UTGOR_UtilityWidget::ModifyCurrentContent()
{
	if (*CurrentContent)
	{
		UTGOR_Content* Content = CurrentContent->GetDefaultObject<UTGOR_Content>();
		if (IsValid(Content))
		{
			Content->MarkPackageDirty();
			return Content;
		}
	}
	return nullptr;
}
