// The Gateway of Realities: Planes of Existence.

#include "TGOR_DimensionGameInstance.h"

#include "Engine/ReplicationDriver.h"
#include "../TGOR_ReplicationGraph.h"


UTGOR_DimensionGameInstance::UTGOR_DimensionGameInstance(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer), UseCustomReplication(true)
{
}

void UTGOR_DimensionGameInstance::Init()
{
	Super::Init();

	if (UseCustomReplication)
	{
		UReplicationDriver::CreateReplicationDriverDelegate().BindLambda([](UNetDriver* ForNetDriver, const FURL& URL, UWorld* World) -> UReplicationDriver*
		{
			return NewObject<UTGOR_ReplicationGraph>(GetTransientPackage());
		});
	}
}
