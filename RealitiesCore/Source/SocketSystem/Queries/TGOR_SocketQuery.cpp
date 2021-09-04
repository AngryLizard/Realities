// The Gateway of Realities: Planes of Existence.

#include "TGOR_SocketQuery.h"

#include "SocketSystem/Components/TGOR_SocketComponent.h"
#include "SocketSystem/Content/TGOR_NamedSocket.h"

#include "CoreSystem/TGOR_Singleton.h"

UTGOR_SocketQuery::UTGOR_SocketQuery()
	: Super(),
	HostComponent(nullptr)
{
}

TArray<UTGOR_CoreContent*> UTGOR_SocketQuery::QueryContent() const
{
	return MigrateContentArray(Sockets);
}


void UTGOR_SocketQuery::AssignComponent(UTGOR_SocketComponent* SocketComponent)
{
	HostComponent = SocketComponent;

	Attachments.Reset();
	if (IsValid(HostComponent))
	{
		Sockets = HostComponent->GetSListOfType<UTGOR_NamedSocket>();
		for (UTGOR_NamedSocket* Socket : Sockets)
		{
			Attachments.Emplace(HostComponent->GetAttached(Socket));
		}
	}
}
