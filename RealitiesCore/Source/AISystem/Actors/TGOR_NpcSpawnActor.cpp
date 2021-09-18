// The Gateway of Realities: Planes of Existence.


#include "TGOR_NpcSpawnActor.h"

#include "CoreSystem/Components/TGOR_SceneComponent.h"
#include "CoreSystem/Gameplay/TGOR_GameInstance.h"
#include "CoreSystem/TGOR_Singleton.h"

#include "DimensionSystem/Data/TGOR_DimensionData.h"
#include "DimensionSystem/Components/TGOR_IdentityComponent.h"

#include "CustomisationSystem/Components/TGOR_ModularSkeletalMeshComponent.h"
#include "CreatureSystem/Content/TGOR_Creature.h"
#include "CreatureSystem/Actors/TGOR_Pawn.h"

#include "AISystem/Gameplay/TGOR_NpcController.h"
#include "AISystem/Content/TGOR_Npc.h"

#include "Net/UnrealNetwork.h"

// Sets default values
ATGOR_NpcSpawnActor::ATGOR_NpcSpawnActor(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	SetActorEnableCollision(true);

	UTGOR_SceneComponent* Root = ObjectInitializer.CreateDefaultSubobject<UTGOR_SceneComponent>(this, FName(TEXT("Root")));
	SetRootComponent(Root);
	Root->SetMobility(EComponentMobility::Static);

	PreviewComponent = ObjectInitializer.CreateDefaultSubobject<UTGOR_ModularSkeletalMeshComponent>(this, FName(TEXT("PreviewComponent")));
	PreviewComponent->SetIsReplicated(false);
	PreviewComponent->SetupAttachment(Root);

	bReplicates = true;
}

// Called when the game starts or when spawned
void ATGOR_NpcSpawnActor::BeginPlay()
{
	Super::BeginPlay();

	// TODO: Do this at a better, more controlled time (via some general spawner interface)
	Spawn();
}

void ATGOR_NpcSpawnActor::GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

}

// Called every frame
void ATGOR_NpcSpawnActor::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );
}

void ATGOR_NpcSpawnActor::Spawn()
{
	UTGOR_IdentityComponent* Identity = GetIdentity();
	if (IsValid(Identity))
	{
		ETGOR_FetchEnumeration State;
		UTGOR_DimensionData* Dimension = GetDimension(State);
		if (IsValid(Dimension))
		{
			UTGOR_Npc* Npc = Cast<UTGOR_Npc>(Identity->GetActorSpawner());
			if (IsValid(Npc))
			{
				UTGOR_Creature* Spawner = Npc->Instanced_CreatureInsertion.Collection;//GetIFromType<UTGOR_Creature>();
				if (IsValid(Spawner))
				{
					const int32 Identifier = Dimension->GetUniqueActorIdentifier();
					const FVector Location = GetActorLocation();
					const FRotator Rotation = GetActorRotation();

					UTGOR_IdentityComponent* Component = Dimension->AddDimensionObject(Identifier, Spawner, Location, Rotation, State);
					if (IsValid(Component))
					{
						APawn* Pawn = Cast<APawn>(Component->GetOwner());
						if (IsValid(Component))
						{
							Pawn->AIControllerClass = Npc->Controller;
							Pawn->SpawnDefaultController();

							UTGOR_GameInstance::DespawnActor(this);
						}
					}
				}
			}
		}
	}

}