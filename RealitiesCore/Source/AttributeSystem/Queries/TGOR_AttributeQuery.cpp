// The Gateway of Realities: Planes of Existence.


#include "TGOR_AttributeQuery.h"

#include "AttributeSystem/Content/TGOR_Attribute.h"


UTGOR_AttributeQuery::UTGOR_AttributeQuery()
: Super()
{
	Required = true;
}

TArray<UTGOR_CoreContent*> UTGOR_AttributeQuery::QueryContent() const
{
	TArray<UTGOR_CoreContent*> Output;
	for (const FTGOR_AttributeOutput& Attribute : Attributes)
	{
		Output.Emplace(Attribute.Attribute);
	}
	return Output;
}

void UTGOR_AttributeQuery::AssignComponent(UTGOR_AttributeComponent* AttributeComponent)
{
	Attributes = AttributeComponent->ComputeActiveAttributes();
}

