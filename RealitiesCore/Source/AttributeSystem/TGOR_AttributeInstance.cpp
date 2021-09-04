// The Gateway of Realities: Planes of Existence.

#include "TGOR_AttributeInstance.h"
#include "CoreSystem/TGOR_Singleton.h"

#include "Content/TGOR_Attribute.h"


FTGOR_AttributeInstance::FTGOR_AttributeInstance()
{
}

float FTGOR_AttributeInstance::GetAttribute(UTGOR_Attribute* Attribute, float Default) const
{
	if (!IsValid(Attribute))
	{
		return Default;
	}

	const float* Ptr = Values.Find(Attribute);
	if (Ptr)
	{
		return *Ptr;
	}
	return Attribute->DefaultValue;
}
