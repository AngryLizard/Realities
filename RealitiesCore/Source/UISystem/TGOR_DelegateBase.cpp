// The Gateway of Realities: Planes of Existence.


#include "TGOR_DelegateBase.h"


UTGOR_DelegateBase::UTGOR_DelegateBase()
	: Super()
{
	Target = nullptr;
	Singleton = nullptr;
}

void UTGOR_DelegateBase::AssignTarget(UObject* Object)
{
	Target = Object;
	AssignWorld(Object->GetWorld());
}

bool UTGOR_DelegateBase::hasTarget()
{
	return(Target != nullptr);
}

void UTGOR_DelegateBase::CallFloatEvent(float Number)
{
	EnsureDelegate(FloatDelegate, FloatEvent, Number);
}

void UTGOR_DelegateBase::CallIntegerEvent(int32 Index)
{
	EnsureDelegate(IntegerDelegate, IntegerEvent, Index);
}

void UTGOR_DelegateBase::CallObjectEvent(UObject* Object)
{
	EnsureDelegate(ObjectDelegate, ObjectEvent, Object);
}


void UTGOR_DelegateBase::AssignFloatEvent(FFloatDelegate Call)
{
	FloatDelegate = Call;
}

void UTGOR_DelegateBase::AssignIntegerEvent(FIntegerDelegate Call)
{
	IntegerDelegate = Call;
}

void UTGOR_DelegateBase::AssignObjectEvent(FObjectDelegate Call)
{
	ObjectDelegate = Call;
}
