// The Gateway of Realities: Planes of Existence.

#include "TGOR_InstigatorComponent.h"
#include "RealitiesUtility/Utility/TGOR_Error.h"
#include "RealitiesUtility/Utility/TGOR_Math.h"

#include "DialogueSystem/Components/TGOR_SpectacleComponent.h"
#include "DialogueSystem/Tasks/TGOR_SpectacleTask.h"

#include "DrawDebugHelpers.h"
#include "Net/UnrealNetwork.h"

UTGOR_InstigatorComponent::UTGOR_InstigatorComponent()
	: Super()
{
}

void UTGOR_InstigatorComponent::AddToCommand(AActor* Actor)
{
	Commands.Emplace(Actor);
	
	// Reset cache
	ConfigurationCache.Empty();
}

void UTGOR_InstigatorComponent::ClearCommand()
{
	Commands.Empty();
	
	// Reset cache
	ConfigurationCache.Empty();
}

TArray<AActor*> UTGOR_InstigatorComponent::GetCommand() const
{
	TArray<AActor*> Actors;
	Actors.Reset(Commands.Num());
	for (const TWeakObjectPtr<AActor>& Command : Commands)
	{
		if (Command.IsValid())
		{
			Actors.Emplace(Command.Get());
		}
	}
	return Actors;
}

bool UTGOR_InstigatorComponent::ValidateTask(UTGOR_SpectacleTask* Task, FTGOR_SpectacleConfiguration& Configuration)
{
	if (const FTGOR_SpectacleConfiguration* Ptr = ConfigurationCache.Find(Task))
	{
		Configuration = *Ptr;
		return Task->CanStart(Configuration);
	}

	if (Task->ComputeCompatibleConfiguration(GetCommand(), Configuration))
	{
		ConfigurationCache.Emplace(Task, Configuration);
		return Task->CanStart(Configuration);
	}
	return false;
}

UTGOR_Participant* UTGOR_InstigatorComponent::GetParticipantByType(const FTGOR_SpectacleConfiguration& Configuration, TSubclassOf<AActor> ActorType, ETGOR_FetchEnumeration& Branches)
{
	// This is more general than GetCommandByType, it could be that there are multiple commands of the given type
	for (const auto& Pair : Configuration.Participants)
	{
		if (Pair.Value.IsValid() && Pair.Value->IsA(ActorType))
		{
			for (const TWeakObjectPtr<AActor>& Command : Commands)
			{
				if (Pair.Value.Get() == Command)
				{
					Branches = ETGOR_FetchEnumeration::Found;
					return Pair.Key;
				}
			}
		}
	}
	Branches = ETGOR_FetchEnumeration::Empty;
	return nullptr;
}

AActor* UTGOR_InstigatorComponent::GetCommandByType(TSubclassOf<AActor> ActorType) const
{
	for (const TWeakObjectPtr<AActor>& Command : Commands)
	{
		if (Command.IsValid() &&
			Command->IsA(ActorType))
		{
			return Command.Get();
		}
	}
	return nullptr;
}