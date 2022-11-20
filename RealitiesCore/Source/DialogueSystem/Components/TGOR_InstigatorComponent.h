// The Gateway of Realities: Planes of Existence.

#pragma once

#include "CoreMinimal.h"
#include "DialogueSystem/TGOR_DialogueInstance.h"
#include "CoreSystem/Utility/TGOR_CoreEnumerations.h"

#include "CoreSystem/Components/TGOR_Component.h"
#include "TGOR_InstigatorComponent.generated.h"

class UTGOR_SpectacleTask;
class UTGOR_Participant;

/**
 * UTGOR_InstigatorComponent represents the audience participating in a dialogue, usually attached to controllers
 */
UCLASS(ClassGroup = (Custom), Blueprintable, meta = (BlueprintSpawnableComponent))
class DIALOGUESYSTEM_API UTGOR_InstigatorComponent : public UTGOR_Component
{
	GENERATED_BODY()

public:
	UTGOR_InstigatorComponent();

	//////////////////////////////////////////// IMPLEMENTABLES ////////////////////////////////////////

	////////////////////////////////////////////////////////////////////////////////////////////////////
public:

	/** Added an actor to current command */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Dialogue", Meta = (Keywords = "C++"))
		void AddToCommand(AActor* Actor);

	/** Clear current command */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Dialogue", Meta = (Keywords = "C++"))
		void ClearCommand();

	/** Creates a company from current command */
	UFUNCTION(BlueprintPure, Category = "!TGOR Dialogue", Meta = (Keywords = "C++"))
		TArray<AActor*> GetCommand() const;

	/** Check whether a given task can be run for this instigator, returns used configuration */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Dialogue", Meta = (Keywords = "C++"))
		bool ValidateTask(UTGOR_SpectacleTask* Task, FTGOR_SpectacleConfiguration& Configuration);

	/** Get participant type for an actor type and configuration */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Dialogue", Meta = (ExpandEnumAsExecs = "Branches", Keywords = "C++"))
		UTGOR_Participant* GetParticipantByType(const FTGOR_SpectacleConfiguration& Configuration, TSubclassOf<AActor> ActorType, ETGOR_FetchEnumeration& Branches);

	/** Get participant type for a command and configuration */
	UFUNCTION(BlueprintPure, Category = "!TGOR Dialogue", Meta = (DeterminesOutputType = "ActorType", Keywords = "C++"))
		AActor* GetCommandByType(TSubclassOf<AActor> ActorType) const;

	////////////////////////////////////////////////////////////////////////////////////////////////////
protected:

	/** Current list of actors under the command of this instigator */
	UPROPERTY(Transient)
		TArray<TWeakObjectPtr<AActor>> Commands;

	/** Cached configurations */ 
	UPROPERTY(Transient)
		TMap<UTGOR_SpectacleTask*, FTGOR_SpectacleConfiguration> ConfigurationCache;
};

