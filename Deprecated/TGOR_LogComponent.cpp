// The Gateway of Realities: Planes of Existence.


#include "TGOR_LogComponent.h"
#include "GameFramework/Actor.h"


FTGOR_Log::FTGOR_Log()
{
	Type = "";
	Index = -1;
	Arg = -1;
}

UTGOR_LogComponent::UTGOR_LogComponent()
	: Super()
{
	_logPosition = 0;

	LogPersistence = 5;
}


void UTGOR_LogComponent::BeginPlay()
{
	Super::BeginPlay();
	
	Log.SetNum(LogPersistence);
	ClearLog();
}

void UTGOR_LogComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (Log[_logPosition].Log.Num() > 0)
	{
		Log[_logPosition].Log.Empty();
	}
	_logPosition = (_logPosition + 1) % Log.Num();
}

void UTGOR_LogComponent::ClearLog()
{
	for (int32 i = 0; i < Log.Num(); i++)
	{
		Log[i].Log.Empty();
	}
}


void UTGOR_LogComponent::AddLog(FString Type, int32 Index, int32 Arg)
{
	FTGOR_Log Entry;

	Entry.Type = Type;
	Entry.Index = Index;
	Entry.Arg = Arg;

	Log[_logPosition].Log.Add(Entry);
	OnLog.Broadcast(Entry);
}


bool UTGOR_LogComponent::HasLog(FString Type, int32 Index, int32 Arg)
{
	for (int32 i = 0; i < Log.Num(); i++)
	{
		FTGOR_LogRow& Row = Log[i];
		for (int32 j = 0; j < Row.Log.Num(); i++)
		{
			FTGOR_Log& Entry = Row.Log[j];
			if (Entry.Type == Type && Entry.Index == Index && Entry.Arg == Arg)
			{
				return(true);
			}
		}
	}
	return(false);
}


bool UTGOR_LogComponent::HasIndex(FString Type, int32 Index)
{
	for (int32 i = 0; i < Log.Num(); i++)
	{
		FTGOR_LogRow& Row = Log[i];
		for (int32 j = 0; j < Row.Log.Num(); i++)
		{
			FTGOR_Log& Entry = Row.Log[j];
			if (Entry.Type == Type && Entry.Index == Index)
			{
				return(true);
			}
		}
	}
	return(false);
}


bool UTGOR_LogComponent::HasArg(FString Type, int32 Arg)
{
	for (int32 i = 0; i < Log.Num(); i++)
	{
		FTGOR_LogRow& Row = Log[i];
		for (int32 j = 0; j < Row.Log.Num(); i++)
		{
			FTGOR_Log& Entry = Row.Log[j];
			if (Entry.Type == Type && Entry.Arg == Arg)
			{
				return(true);
			}
		}
	}
	return(false);
}


void UTGOR_LogComponent::SeekLog(AActor* Actor, FString Type, int32 Index, int32 Arg)
{
	UActorComponent* Component = Actor->GetComponentByClass(UTGOR_LogComponent::StaticClass());

	if (!IsValid(Component)) return;

	UTGOR_LogComponent* LogComponent = Cast<UTGOR_LogComponent>(Component);
	if (!IsValid(LogComponent)) return;

	LogComponent->AddLog(Type, Index, Arg);
}