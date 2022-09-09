// The Gateway of Realities: Planes of Existence.

#include "TGOR_Dialogue.h"

#include "CoreSystem/TGOR_Singleton.h"
#include "DialogueSystem/Tasks/TGOR_DialogueTask.h"
#include "DialogueSystem/Components/TGOR_DialogueComponent.h"
#include "AnimationSystem/Content/TGOR_Animation.h"

UTGOR_Dialogue::UTGOR_Dialogue()
	: Super()
{
}

////////////////////////////////////////////////////////////////////////////////////////////////////

UTGOR_DialogueTask* UTGOR_Dialogue::CreateDialogueTask(UTGOR_DialogueComponent* Component, int32 SlotIdentifier)
{
	return UTGOR_Task::CreateTask(this, TaskType, Component, SlotIdentifier);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void UTGOR_Dialogue::TaskInitialise(UTGOR_DialogueTask* Task)
{
	OnTaskInitialise(Task);
}
