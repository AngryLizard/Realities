// The Gateway of Realities: Planes of Existence.

#pragma once

#include "CoreMinimal.h"

#include "DimensionSystem/Content/TGOR_SpawnModule.h"
#include "TGOR_Stage.generated.h"

class UTGOR_Camera;

////////////////////////////////////////////////////////////////////////////////////////////////////

UCLASS(Blueprintable)
class CAMERASYSTEM_API UTGOR_Stage : public UTGOR_SpawnModule
{
	GENERATED_BODY()

public:
	UTGOR_Stage();

	////////////////////////////////////////////////////////////////////////////////////////////////////
public:

	/** Cameras in this spawner */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "!TGOR Insertion")
		TArray<TSubclassOf<UTGOR_Camera>> CameraInsertions;
	DECL_INSERTION(CameraInsertions);

	virtual void MoveInsertion(UTGOR_Content* Insertion, ETGOR_InsertionActionEnumeration Action, bool& Success) override;

private:
};

