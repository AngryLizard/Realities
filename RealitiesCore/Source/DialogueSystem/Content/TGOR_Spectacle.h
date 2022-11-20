// TGOR (C) // CHECKED //
#pragma once

#include "DialogueSystem/TGOR_DialogueInstance.h"

#include "DimensionSystem/Content/TGOR_SpawnModule.h"
#include "TGOR_Spectacle.generated.h"

////////////////////////////////////////////// DECL //////////////////////////////////////////////////////

class UTGOR_SpectatorComponent;
class UTGOR_Spectator;

/**
* UTGOR_Spectacle can have spectators
*/
UCLASS()
class DIALOGUESYSTEM_API UTGOR_Spectacle : public UTGOR_SpawnModule
{
	GENERATED_BODY()

public:
	
	UTGOR_Spectacle();

	////////////////////////////////////////////////////////////////////////////////////////////////////
public:

	/** Participating parties */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "!TGOR Dialogue")
		TSet<TSubclassOf<UTGOR_Participant>> ParticipantInsertions;
	DECL_INSERTION(ParticipantInsertions);

	virtual void MoveInsertion(UTGOR_Content* Insertion, ETGOR_InsertionActionEnumeration Action, bool& Success) override;

};
