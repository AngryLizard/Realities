// The Gateway of Realities: Planes of Existence.

#pragma once
#include "CoreMinimal.h"

#include "CreatureSystem/Content/TGOR_Creature.h"
#include "TGOR_DefaultCreature.generated.h"

////////////////////////////////////////////////////////////////////////////////////////////////////

UCLASS()
class CREATURESYSTEM_API UTGOR_DefaultCreature : public UTGOR_Creature
{
	GENERATED_BODY()

public:
	UTGOR_DefaultCreature();
	virtual void BuildResource() override;

private:
};

