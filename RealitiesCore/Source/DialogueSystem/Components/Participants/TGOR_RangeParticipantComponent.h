// The Gateway of Realities: Planes of Existence.

#pragma once

#include "CoreMinimal.h"
#include "DialogueSystem/TGOR_DialogueInstance.h"

#include "CoreSystem/Interfaces/TGOR_RegisterInterface.h"
#include "DialogueSystem/Interfaces/TGOR_ParticipantInterface.h"
#include "DimensionSystem/Interfaces/TGOR_SpawnerInterface.h"
#include "CoreSystem/Components/TGOR_AreaComponent.h"
#include "TGOR_RangeParticipantComponent.generated.h"

class UTGOR_Participant;

/**
 * UTGOR_RangeParticipantComponent are participants that are present in a certain range
 */
UCLASS(ClassGroup = (Custom), Blueprintable, meta = (BlueprintSpawnableComponent))
class DIALOGUESYSTEM_API UTGOR_RangeParticipantComponent : public UTGOR_AreaComponent, public ITGOR_SpawnerInterface, public ITGOR_ParticipantInterface
{
	GENERATED_BODY()

public:
	UTGOR_RangeParticipantComponent(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

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

