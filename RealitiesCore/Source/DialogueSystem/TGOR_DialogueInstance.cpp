// The Gateway of Realities: Planes of Existence.


#include "TGOR_DialogueInstance.h"

#include "RealitiesUGC/Mod/TGOR_ContentManager.h"

#include "CoreSystem/Gameplay/TGOR_GameInstance.h"
#include "CoreSystem/TGOR_Singleton.h"
#include "CoreSystem/Storage/TGOR_Package.h"

#include "DialogueSystem/Content/TGOR_Dialogue.h"

DEFINE_LOG_CATEGORY(TGOR_DialogueLogs);

SERIALISE_INIT_SOURCE_IMPLEMENT(FTGOR_DialogueState);

FTGOR_DialogueState::FTGOR_DialogueState()
:	ActiveSlot(INDEX_NONE)
{
}

void FTGOR_DialogueState::Send(FTGOR_NetworkWritePackage& Package, UTGOR_Singleton* Context) const
{
	Package.WriteEntry(ActiveSlot);
}

void FTGOR_DialogueState::Recv(FTGOR_NetworkReadPackage& Package, UTGOR_Singleton* Context)
{
	Package.ReadEntry(ActiveSlot);
}
