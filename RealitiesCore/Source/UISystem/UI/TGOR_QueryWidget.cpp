// The Gateway of Realities: Planes of Existence.


#include "TGOR_QueryWidget.h"


UTGOR_QueryWidget::UTGOR_QueryWidget(const FObjectInitializer& ObjectInitializer)
: Super(ObjectInitializer)
{
}

void UTGOR_QueryWidget::ApplyQuery(UTGOR_Query* Query)
{
	OnApplyQuery(Query);
}

