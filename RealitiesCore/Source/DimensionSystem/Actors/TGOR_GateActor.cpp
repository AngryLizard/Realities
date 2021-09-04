// The Gateway of Realities: Planes of Existence.


#include "TGOR_GateActor.h"

#include "DimensionSystem/Data/TGOR_DimensionData.h"
#include "DimensionSystem/Components/TGOR_DimensionGateComponent.h"
#include "DimensionSystem/Components/TGOR_PilotComponent.h"
#include "CoreSystem/Components/TGOR_SceneComponent.h"

#include "GameFramework/GameMode.h"
#include "CoreSystem/TGOR_Singleton.h"
#include "Net/UnrealNetwork.h"

// Sets default values
ATGOR_GateActor::ATGOR_GateActor(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	SetActorEnableCollision(true);

	UTGOR_SceneComponent* Root = ObjectInitializer.CreateDefaultSubobject<UTGOR_SceneComponent>(this, FName(TEXT("Root")));
	SetRootComponent(Root);
	Root->SetMobility(EComponentMobility::Static);

	GateComponent = ObjectInitializer.CreateDefaultSubobject<UTGOR_DimensionGateComponent>(this, FName(TEXT("PreviewMesh")));
	GateComponent->SetIsReplicated(true);
	GateComponent->SetupAttachment(Root);

	bReplicates = true;
}

// Called when the game starts or when spawned
void ATGOR_GateActor::BeginPlay()
{
	Super::BeginPlay();
	
	GateComponent->ChangeState(ETGOR_GatewayState::Idle);
}

void ATGOR_GateActor::GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

}

// Called every frame
void ATGOR_GateActor::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );
}

bool ATGOR_GateActor::Assemble(UTGOR_DimensionData* Dimension)
{
	Super::Assemble(Dimension);

	Dimension->MainGate = this;
	return true;
}


void ATGOR_GateActor::RepNotifyLifeline()
{

}

void ATGOR_GateActor::CheckContentTeleport()
{
	if (GateComponent->IsReady())
	{
		if (!GateComponent->IsTeleporting() && Content.Num() > 0)
		{
			GateComponent->StartTeleport();
		}
		else if (!LinksHaveContent())
		{
			GateComponent->ChangeState(ETGOR_GatewayState::Active);
		}
	}
}

void ATGOR_GateActor::AddContent(UTGOR_PilotComponent* Component)
{
	Content.AddUnique(Component);

	/*
	APawn* Pawn = Cast<APawn>(Component->GetOwner());
	if(IsValid(Pawn))
	{
		ATGOR_OnlineController* OnlineController = Cast<ATGOR_OnlineController>(Pawn->GetController());
		if (IsValid(OnlineController))
		{
			GateComponent->LoadForPlayer(OnlineController);
		}
	}
	*/
	CheckContentTeleport();
}

void ATGOR_GateActor::RemoveContent(UTGOR_PilotComponent* Component)
{
	Content.Remove(Component);
	CheckContentTeleport();
}


void ATGOR_GateActor::ExchangeContent()
{
	if (!GateComponent->IsMaster) return;
	
	// Since after teleport the teleported actor overlap as well, we need to establish both lists first
	if (IsValid(GateComponent->GateLink))
	{
		ATGOR_GateActor* Gate = Cast<ATGOR_GateActor>(GateComponent->GateLink->GetOwner());
		if (IsValid(Gate))
		{
			for (int32 Index = Gate->Content.Num() - 1; Index >= 0; Index--)
			{
				ETGOR_ComputeEnumeration State;
				Gate->GateComponent->EnterOther(Gate->Content[Index], true, FTransform::Identity, State);
				if (State == ETGOR_ComputeEnumeration::Success &&
					Gate->Content.IsValidIndex(Index)) // Actor possibly was removed by teleport event
				{
					Gate->Content.RemoveAt(Index);
				}
			}

			Gate->OnGateExchange();
		}
	}

	// Teleport own content
	for (int32 Index = Content.Num()-1; Index >= 0; Index--)
	{
		ETGOR_ComputeEnumeration State = ETGOR_ComputeEnumeration::Success;
		GateComponent->EnterOther(Content[Index], true, FTransform::Identity, State);
		if (State == ETGOR_ComputeEnumeration::Success &&
			Content.IsValidIndex(Index)) // Actor possibly was removed by teleport event
		{
			Content.RemoveAt(Index);
		}
	}

	// Custom exchange
	OnGateExchange();

	// Has to be set *after* teleport, since Teleportation is disabled while transceiving
	GateComponent->ChangeState(ETGOR_GatewayState::Transceiving);
}

bool ATGOR_GateActor::HasContent() const
{
	return Content.Num() > 0;
}

bool ATGOR_GateActor::LinksHaveContent() const
{
	if (HasContent())
	{
		return true;
	}

	if (IsValid(GateComponent->GateLink))
	{
		ATGOR_GateActor* Gate = Cast<ATGOR_GateActor>(GateComponent->GateLink);
		if (IsValid(Gate))
		{
			return Gate->HasContent();
		}
	}
	return false;
}
