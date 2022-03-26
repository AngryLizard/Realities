// TGOR (C) // CHECKED //
#pragma once

#include "CoreMinimal.h"

#include "MovementSystem/Tasks/TGOR_EuclideanMovementTask.h"
#include "TGOR_OrientableTask.generated.h"

////////////////////////////////////////////// DECL //////////////////////////////////////////////////////

class UTGOR_HandleComponent;
class UTGOR_ArmatureComponent;
class UTGOR_Primitive;

/**
*
*/
UCLASS(Blueprintable)
class MOVEMENTSYSTEM_API UTGOR_OrientableTask : public UTGOR_EuclideanMovementTask
{
	GENERATED_BODY()

public:

	////////////////////////////////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////// ENGINE ////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////////////////////////////

	UTGOR_OrientableTask();

	virtual void Initialise() override;
	virtual bool Invariant(const FTGOR_MovementSpace& Space, const FTGOR_MovementExternal& External) const override;

	////////////////////////////////////////////////////////////////////////////////////////////////////
public:

	/** Arm primitive types */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "!TGOR Movement|Anatomy")
		TSet<TSubclassOf<UTGOR_Primitive>> PrimitiveTypes;

	////////////////////////////////////////////////////////////////////////////////////////////////////
protected:

	/** Upvector according to legs */
	UPROPERTY(Transient, BlueprintReadOnly, Category = "!TGOR Movement")
		FVector LocalUpVector;

	/** Movement handle of feet */
	UPROPERTY(Transient, BlueprintReadOnly, Category = "!TGOR Movement")
		TArray<UTGOR_HandleComponent*> Primitives;

};
