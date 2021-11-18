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

float UTGOR_Attribute::ModifyValue(float Value, float Modifier) const
{
	switch (Accumulation)
	{
	case ETGOR_AttributeAccumulation::Add: Value += Modifier; break;
	case ETGOR_AttributeAccumulation::Mul: Value *= Modifier; break;
	case ETGOR_AttributeAccumulation::Max: Value = FMath::Max(Value, Modifier); break;
	}
	return Value;
}
