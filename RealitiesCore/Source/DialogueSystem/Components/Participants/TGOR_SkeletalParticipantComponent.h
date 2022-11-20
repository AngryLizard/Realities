// The Gateway of Realities: Planes of Existence.

#pragma once

#include "CoreMinimal.h"
#include "DialogueSystem/TGOR_DialogueInstance.h"

#include "CoreSystem/Interfaces/TGOR_RegisterInterface.h"
#include "DialogueSystem/Interfaces/TGOR_ParticipantInterface.h"
#include "DimensionSystem/Interfaces/TGOR_SpawnerInterface.h"
#include "Components/SkeletalMeshComponent.h"
#include "TGOR_SkeletalParticipantComponent.generated.h"

class UTGOR_Participant;

/**
 * UTGOR_SkeletalParticipantComponent are participants that have a certain body/skeleton
 */
UCLASS(ClassGroup = (Custom), Blueprintable, meta = (BlueprintSpawnableComponent))
class DIALOGUESYSTEM_API UTGOR_SkeletalParticipantComponent : public USkeletalMeshComponent, public ITGOR_SpawnerInterface, public ITGOR_ParticipantInterface
{
	GENERATED_BODY()

public:
	UTGOR_SkeletalParticipantComponent();
	virtual void BeginPlay() override;

	virtual void UpdateContent_Implementation(FTGOR_SpawnerDependencies& Dependencies) override;
	virtual TMap<int32, UTGOR_SpawnModule*> GetModuleType_Implementation() const override;

	virtual bool IsCompatibleWith(AActor* Actor) const override;
	virtual UTGOR_Participant* GetParticipant() const override;

	//////////////////////////////////////////// IMPLEMENTABLES ////////////////////////////////////////

	////////////////////////////////////////////////////////////////////////////////////////////////////
public:

	/** Participant type this component spawns with. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "!TGOR Dialogue")
		TSubclassOf<UTGOR_Participant> SpawnParticipant;

	/** Participant represented by this component */
	UPROPERTY(BlueprintReadOnly, Category = "!TGOR Dialogue")
		UTGOR_Participant* Participant;
};

