// The Gateway of Realities: Planes of Existence.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "TGOR_ParticipantInterface.generated.h"

class UTGOR_Participant;

UINTERFACE(MinimalAPI, meta = (CannotImplementInterfaceInBlueprint))
class UTGOR_ParticipantInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 *
 */
class DIALOGUESYSTEM_API ITGOR_ParticipantInterface
{
	GENERATED_BODY()

public:

	/** Whether an actor can be used fro this participant */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Dialogue", Meta = (Keywords = "C++"))
		virtual bool IsCompatibleWith(AActor* Actor) const;

	/** Get assigned participant */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Dialogue", Meta = (Keywords = "C++"))
		virtual UTGOR_Participant* GetParticipant() const;

	/** Participant that is linked by default */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Dialogue", Meta = (Keywords = "C++"))
		virtual AActor* GetLinkedActor() const;

	/** Whether participant is added automatically. */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Dialogue", Meta = (Keywords = "C++"))
		virtual bool HasLinkedActor() const;

	/** Whether participant is currently available. */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Dialogue", Meta = (Keywords = "C++"))
		virtual bool IsAvailable() const;
};
