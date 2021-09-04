// The Gateway of Realities: Planes of Existence.
#include "TGOR_Attribute.h"
#include "AttributeSystem/Components/TGOR_AttributeComponent.h"

UTGOR_Attribute::UTGOR_Attribute()
	: Super(),
	Accumulation(ETGOR_AttributeAccumulation::Add),
	DefaultValue(0.0f)
{
}

float UTGOR_Attribute::GetInstanceAttribute(const FTGOR_AttributeInstance& Instance, UTGOR_Attribute* Attribute, float Default)
{
	return Instance.GetAttribute(Attribute, Default);
}

FTGOR_AttributeInstance UTGOR_Attribute::UpdateInstanceAttribute(const UTGOR_AttributeComponent* Component, const TArray<UTGOR_Attribute*>& Attributes)
{
	FTGOR_AttributeInstance Instance;
	for (UTGOR_Attribute* Attribute : Attributes)
	{
		Instance.Values.Emplace(Attribute, Attribute->DefaultValue);
	}
	Component->UpdateAttributes(Instance.Values);
	return Instance;
}