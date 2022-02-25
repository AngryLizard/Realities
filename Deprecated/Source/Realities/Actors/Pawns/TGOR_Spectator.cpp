// The Gateway of Realities: Planes of Existence.


#include "TGOR_Spectator.h"

#include "Realities/Base/TGOR_Singleton.h"
#include "Realities/Base/System/Data/TGOR_UserData.h"
#include "Realities/Base/System/Data/TGOR_WorldData.h"
#include "Realities/Base/System/Data/TGOR_DimensionData.h"
#include "Realities/Mod/Spawner/Creatures/TGOR_Creature.h"
#include "Realities/Base/Controller/TGOR_PlayerController.h"
#include "Components/CapsuleComponent.h"

#include "Realities/Components/Combat/TGOR_AimComponent.h"
#include "Realities/Components/Movement/TGOR_PhysicsComponent.h"
#include "Realities/Components/System/Actions/TGOR_EquipmentComponent.h"

#include "Realities/Base/Content/TGOR_ContentManager.h"
#include "Realities/Base/Controller/TGOR_OnlineController.h"
#include "Realities/Actors/Dimension/TGOR_GateActor.h"
#include "Net/UnrealNetwork.h"

// Sets default values
ATGOR_Spectator::ATGOR_Spectator(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = true;

	UCapsuleComponent* Capsule = GetCapsule();

	EquipmentComponent = ObjectInitializer.CreateDefaultSubobject<UTGOR_EquipmentComponent>(this, FName(TEXT("EquipmentComponent")));
	EquipmentComponent->SetNetAddressable();
	EquipmentComponent->SetIsReplicated(true);
	
	MovementComponent = ObjectInitializer.CreateDefaultSubobject<UTGOR_PhysicsComponent>(this, FName(TEXT("MovementComponent")));
	MovementComponent->SetNetAddressable();
	MovementComponent->SetIsReplicated(true);

	AimComponent = ObjectInitializer.CreateDefaultSubobject<UTGOR_AimComponent>(this, FName(TEXT("AimComponent")));
	AimComponent->SetupAttachment(Capsule);
	AimComponent->SetNetAddressable();
	AimComponent->SetIsReplicated(true);

	UTGOR_IdentityComponent* Identity = GetIdentity();
	Identity->IgnoreStorage = true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////


void ATGOR_Spectator::BeginPlay()
{
	Super::BeginPlay();
}

void ATGOR_Spectator::GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

}

void ATGOR_Spectator::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!Controller) return;
	Controller->SetControlRotation(GetCameraRotation().Rotator());

	ATGOR_PlayerController* PlayerController = Cast<ATGOR_PlayerController>(GetController());
	if (IsValid(PlayerController) && PlayerController->IsLocalController())
	{
		// Rotate with movement base
		FTGOR_MovementSpace ParentSpace = MovementComponent->GetBase().ComputeParentSpace();
		const float Angle = ParentSpace.AngularVelocity.Size();
		if (Angle > SMALL_NUMBER)
		{
			GetCamera()->RotateCamera(ParentSpace.AngularVelocity / Angle, Angle * DeltaTime, false);
		}

		// Update aim
		const FVector Location = PlayerController->PlayerCameraManager->GetCameraLocation();
		const FRotator Rotation = PlayerController->PlayerCameraManager->GetCameraRotation();
		AimComponent->UpdateAimFromCameraNearby(Location, Rotation.Vector());
	}
}

void ATGOR_Spectator::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
}

void ATGOR_Spectator::UnPossessed()
{
	Super::UnPossessed();

	//UTGOR_GameInstance::DespawnActor(this);
}


bool ATGOR_Spectator::Assemble(UTGOR_DimensionData* Dimension)
{
	Super::Assemble(Dimension);
	return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void ATGOR_Spectator::RequestNewBody_Implementation(FTGOR_CreatureSetupInstance Setup)
{
	SINGLETON_CHK;
	UTGOR_UserData* UserData = Singleton->GetUserData();

	// Spawn in persistent dimension
	ETGOR_FetchEnumeration State;
	UTGOR_DimensionData* Dimension = GetDimension(State);
	if (State == ETGOR_FetchEnumeration::Found)
	{
		// Get spawn location as defined
		FTransform Transform = GetActorTransform();
		GetSpawnTransform(Transform);

		// Spawn new body
		const int32 ActorIdewntifier = Dimension->GetUniqueActorIdentifier();
		UTGOR_IdentityComponent* Identity = Dimension->AddDimensionObject(ActorIdewntifier, Setup.Creature, Transform.GetLocation(), Transform.GetRotation().Rotator(), State);
		if (State == ETGOR_FetchEnumeration::Found)
		{
			ATGOR_Avatar* Avatar = Cast<ATGOR_Avatar>(Identity->GetOwner());
			if (IsValid(Avatar))
			{
				Avatar->ApplySetup(Setup);

				// Assign body to user
				AController* OwnController = GetController();
				if (IsValid(OwnController))
				{
					ATGOR_OnlineController* OnlineController = Cast<ATGOR_OnlineController>(OwnController);
					if (IsValid(OnlineController))
					{
						const int32 UserKey = OnlineController->GetActiveUserKey();
						const int32 BodyIdentifier = UserData->AddUserBody(UserKey, Avatar);
						OnlineController->RequestBodySwitch(BodyIdentifier);
						OnBodyRequest(Avatar);
					}
					else
					{
						// Technically an error case since only NPCs have non-online controllers and those don't call the server
						OwnController->Possess(Avatar);
						OnBodyRequest(Avatar);
					}
				}
			}
		}
	}
}

bool ATGOR_Spectator::RequestNewBody_Validate(FTGOR_CreatureSetupInstance Setup)
{
	return true;
}