// The Gateway of Realities: Planes of Existence.


#include "TGOR_Spectator.h"

#include "Components/CapsuleComponent.h"
#include "RealitiesUGC/Mod/TGOR_ContentManager.h"

#include "CoreSystem/TGOR_Singleton.h"
#include "PlayerSystem/Data/TGOR_UserData.h"
#include "DimensionSystem/Data/TGOR_WorldData.h"
#include "DimensionSystem/Data/TGOR_DimensionData.h"
#include "DimensionSystem/Components/TGOR_TrackedComponent.h"
#include "MovementSystem/Components/TGOR_MovementComponent.h"
#include "PhysicsSystem/Components/TGOR_ArmatureComponent.h"
#include "CreatureSystem/Content/TGOR_Creature.h"
#include "PlayerSystem/Gameplay/TGOR_PlayerController.h"

#include "CoreSystem/Gameplay/TGOR_GameInstance.h"
#include "TargetSystem/Components/TGOR_AimComponent.h"
#include "PhysicsSystem/Components/TGOR_PhysicsComponent.h"
#include "ActionSystem/Components/TGOR_ActionComponent.h"
#include "CoreSystem/Storage/TGOR_Package.h"

#include "PlayerSystem/Gameplay/TGOR_OnlineController.h"
#include "DimensionSystem/Actors/TGOR_GateActor.h"
#include "Net/UnrealNetwork.h"

SERIALISE_INIT_SOURCE_IMPLEMENT(FTGOR_CreateBodySetup);


FTGOR_CreateBodySetup::FTGOR_CreateBodySetup()
	: Creature(nullptr)
{
}

void FTGOR_CreateBodySetup::Send(FTGOR_NetworkWritePackage& Package, UTGOR_Singleton* Context) const
{
	Package.WriteEntry(Creature);
	Package.WriteEntry(Setup);
}

void FTGOR_CreateBodySetup::Recv(FTGOR_NetworkReadPackage& Package, UTGOR_Singleton* Context)
{
	Package.ReadEntry(Creature);
	Package.ReadEntry(Setup);
}


// Sets default values
ATGOR_Spectator::ATGOR_Spectator(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = true;

	ActionComponent = ObjectInitializer.CreateDefaultSubobject<UTGOR_ActionComponent>(this, FName(TEXT("ActionComponent")));
	ActionComponent->SetNetAddressable();
	ActionComponent->SetIsReplicated(true);
	
	AimComponent = ObjectInitializer.CreateDefaultSubobject<UTGOR_AimComponent>(this, FName(TEXT("AimComponent")));
	AimComponent->SetupAttachment(GetRootComponent());
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

	UTGOR_ArmatureComponent* Armature = GetArmature();
	ATGOR_PlayerController* PlayerController = Cast<ATGOR_PlayerController>(GetController());
	if (IsValid(PlayerController) && PlayerController->IsLocalController() && IsValid(Armature))
	{
		// Rotate with movement base
		FTGOR_MovementDynamic ParentSpace = Armature->ComputeBase();
		const float Angle = ParentSpace.AngularVelocity.Size();
		if (Angle > SMALL_NUMBER)
		{
			UTGOR_CameraComponent* Camera = GetCamera();
			Camera->RotateCamera(ParentSpace.AngularVelocity / Angle, Angle * DeltaTime, false);
		}

		// Update aim
		AimComponent->UpdateCandidatesNearby();
		const FVector Location = PlayerController->PlayerCameraManager->GetCameraLocation();
		const FRotator Rotation = PlayerController->PlayerCameraManager->GetCameraRotation();
		AimComponent->UpdateAimFromCamera(Location, Rotation.Vector());
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

void ATGOR_Spectator::RequestNewBody_Implementation(FTGOR_CreateBodySetup Setup)
{
	SINGLETON_CHK;
	UTGOR_UserData* UserData = Singleton->GetData<UTGOR_UserData>();
	if (IsValid(UserData))
	{
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
					// Assign body to user
					AController* OwnController = GetController();
					if (IsValid(OwnController))
					{
						ATGOR_OnlineController* OnlineController = Cast<ATGOR_OnlineController>(OwnController);
						if (IsValid(OnlineController))
						{
							const int32 UserKey = OnlineController->GetActiveUserKey();
							const int32 BodyIdentifier = UserData->AddUserBody(UserKey, Avatar->GetIdentity());

							OnlineController->RequestAppearance_Implementation(BodyIdentifier, Setup.Setup.Appearance);
							OnlineController->RequestActions_Implementation(BodyIdentifier, Setup.Setup.Actions);
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
}

bool ATGOR_Spectator::RequestNewBody_Validate(FTGOR_CreateBodySetup Setup)
{
	return true;
}