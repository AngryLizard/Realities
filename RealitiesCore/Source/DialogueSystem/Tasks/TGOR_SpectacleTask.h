// TGOR (C) // CHECKED //
#pragma once

#include "CoreMinimal.h"
#include "Net/UnrealNetwork.h"

#include "DialogueSystem/TGOR_DialogueInstance.h"
#include "DialogueSystem/Interfaces/TGOR_ParticipantInterface.h"

#include "AnimationSystem/Tasks/TGOR_AnimatedTask.h"
#include "TGOR_SpectacleTask.generated.h"

////////////////////////////////////////////// DECL //////////////////////////////////////////////////////

class UTGOR_SpectacleComponent;
class UTGOR_Spectacle;

UENUM(BlueprintType)
enum class ETGOR_SpectacleStateEnumeration : uint8
{
	Continue,
	Terminate
};

/**
* 
*/
UCLASS(Abstract, Blueprintable)
class DIALOGUESYSTEM_API UTGOR_SpectacleTask : public UTGOR_AnimatedTask
{
	GENERATED_BODY()

public:
	
	////////////////////////////////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////// ENGINE ////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////////////////////////////

	UTGOR_SpectacleTask();

	////////////////////////////////////////////////////////////////////////////////////////////////////
public:

	/** Returns whether Activator can be started in this context */
	UFUNCTION(BlueprintPure, Category = "!TGOR Dialogue", Meta = (Keywords = "C++"))
		virtual bool CanStart(const FTGOR_SpectacleConfiguration& Configuration) const;

	////////////////////////////////////////////////////////////////////////////////////////////////////
public:

	/** Called once when this task is created */
	virtual bool Initialise();

	/** Called once Activator is invoked */
	virtual void Prepare(FTGOR_SpectacleState& DialogueState);

	/** Called every tick while active */
	virtual bool Update(FTGOR_SpectacleState& DialogueState, float DeltaTime);

	/** Called once Activator is stopped */
	virtual void Interrupt(FTGOR_SpectacleState& DialogueState);

	////////////////////////////////////////////////////////////////////////////////////////////////////
protected:

	/** Called once when this task is assigned to a component. Should usually only happen once. */
	UFUNCTION(BlueprintImplementableEvent, Category = "!TGOR Dialogue", Meta = (Keywords = "C++"))
		void OnInitialise();

	/** Condition if dialogue can be started.
		If false this dialogue cannot be started, it can however be running. */
	UFUNCTION(BlueprintImplementableEvent, Category = "!TGOR Dialogue", Meta = (Keywords = "C++"))
		void OnCondition(ETGOR_ValidEnumeration& Branches) const;

	////////////////////////////////////////////////////////////////////////////////////////////////////
public:

	/** Returns a compatible configuration for a given set of actors */
	UFUNCTION(BlueprintPure, Category = "!TGOR Dialogue", Meta = (Keywords = "C++"))
		bool ComputeCompatibleConfiguration(const TArray<AActor*>& Actors, FTGOR_SpectacleConfiguration& Configuration) const;

	////////////////////////////////////////////////////////////////////////////////////////////////////
protected:

	/** Initialise participant component cache */
	UFUNCTION()
		bool InitialiseParticipants(UTGOR_SpectacleComponent* Owner, UTGOR_Spectacle* Spectacle);

	/** Participant component cache */
	UPROPERTY(Transient)
		TMap<UTGOR_Participant*, TScriptInterface<ITGOR_ParticipantInterface>> Participants;
};
