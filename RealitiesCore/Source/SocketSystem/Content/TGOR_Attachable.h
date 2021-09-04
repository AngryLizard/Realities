// The Gateway of Realities: Planes of Existence.

#pragma once

#include "CoreMinimal.h"

#include "DimensionSystem/Content/TGOR_SpawnModule.h"
#include "TGOR_Attachable.generated.h"

class UTGOR_Socket;

/**
 * 
 */
UCLASS(Blueprintable)
class SOCKETSYSTEM_API UTGOR_Attachable : public UTGOR_SpawnModule
{
	GENERATED_BODY()

public:
	UTGOR_Attachable();

	////////////////////////////////////////////////////////////////////////////////////////////////////
public:

	/** Sockets in this spawner */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "!TGOR Insertion")
		TArray<TSubclassOf<UTGOR_Socket>> SocketInsertions;
	DECL_INSERTION(SocketInsertions);

	virtual void MoveInsertion(UTGOR_Content* Insertion, ETGOR_InsertionActionEnumeration Action, bool& Success) override;

};