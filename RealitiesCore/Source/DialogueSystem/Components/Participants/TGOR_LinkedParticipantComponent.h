// The Gateway of Realities: Planes of Existence.

#pragma once

#include "CoreMinimal.h"
#include "DialogueSystem/TGOR_DialogueInstance.h"

#include "CoreSystem/Interfaces/TGOR_RegisterInterface.h"
#include "DialogueSystem/Interfaces/TGOR_ParticipantInterface.h"
#include "DimensionSystem/Interfaces/TGOR_SpawnerInterface.h"
#include "CoreSystem/Interfaces/TGOR_SingletonInterface.h"
#include "CoreSystem/Components/TGOR_Component.h"
#include "TGOR_LinkedParticipantComponent.generated.h"

class UTGOR_Participant;

/**
 * UTGOR_LinkedParticipantComponent are participants that are already linked to existing actors
 */
UCLASS(ClassGroup = (Custom), Blueprintable, meta = (BlueprintSpawnableComponent))
class DIALOGUESYSTEM_API UTGOR_LinkedParticipantComponent : public UTGOR_Component, public ITGOR_SpawnerInterface, public ITGOR_ParticipantInterface
{
	GENERATED_BODY()

public:
	UTGOR_LinkedParticipantComponent();

	virtual void UpdateContent_Implementation(FTGOR_SpawnerDependencies& Dependencies) override;
	virtual TMap<int32, UTGOR_SpawnModule*> GetModuleType_Implementation() const override;

	virtual bool IsCompatibleWith(AActor* Actor) const override;
	virtual UTGOR_Participant* GetParticipant() const override;
	virtual AActor* GetLinkedActor() const override;
	virtual bool HasLinkedActor() const override;

	//////////////////////////////////////////// IMPLEMENTABLES ////////////////////////////////////////

	////////////////////////////////////////////////////////////////////////////////////////////////////
public:

	/** Participant type this component spawns with. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "!TGOR Dialogue")
		TSubclassOf<UTGOR_Participant> SpawnParticipant;

	/** Participant represented by this component */
	UPROPERTY(BlueprintReadOnly, Category = "!TGOR Dialogue")
		UTGOR_Participant* Participant;

	////////////////////////////////////////////////////////////////////////////////////////////////////
public:

	/** Assigned actor */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "!TGOR Dialogue", meta = (EditCondition = "bAssignSelf"))
		AActor* AssignLink = nullptr;
};

