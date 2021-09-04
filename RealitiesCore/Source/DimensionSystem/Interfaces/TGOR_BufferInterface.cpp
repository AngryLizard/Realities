// The Gateway of Realities: Planes of Existence.


#include "TGOR_BufferInterface.h"
#include "DimensionSystem/Content/TGOR_Spawner.h"


void UpdateContent_Implementation(UTGOR_Spawner* Spawner)
{
}

void ITGOR_BufferInterface::LoadFromBufferExternal(int32 Index)
{

}

void ITGOR_BufferInterface::LoadFromBufferInternal(int32 Index)
{

}

void ITGOR_BufferInterface::LerpToBuffer(int32 Index, int32 NextIndex, float Alpha)
{

}

void ITGOR_BufferInterface::StoreToBuffer(int32 Index)
{

}

void ITGOR_BufferInterface::SetBufferSize(int32 Size)
{

}


void ITGOR_StashInterface::Stash()
{
	StashState = true;
}

void ITGOR_StashInterface::Revert()
{
	StashState = false;
}

bool ITGOR_StashInterface::HasStash() const
{
	return StashState;
}
