// TGOR (C) // CHECKED //
#pragma once

#include "Realities/Actors/TGOR_Actor.h"
#include "TGOR_ProjectileActor.generated.h"

class UTGOR_ProjectileMovementComponent;

/**
* TGOR_ProjectileActor provides basic functionality for any kind of projectile.
*/
UCLASS()
class REALITIES_API ATGOR_ProjectileActor : public ATGOR_Actor
{
	GENERATED_BODY()
	
public:

	////////////////////////////////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////// ENGINE ////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////////////////////////////

	ATGOR_ProjectileActor(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	virtual void BeginPlay() override;
	virtual void Tick( float DeltaSeconds ) override;

	///////////////////////////////////////////// IMPLEMENTABLES /////////////////////////////////////////

	/** */

	////////////////////////////////////////////// COMPONENTS ////////////////////////////////////////////
private:

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "!TGOR Projectile", Meta = (AllowPrivateAccess = "true"))
		UTGOR_ProjectileMovementComponent* ProjectileMovement;

public:

	FORCEINLINE UTGOR_ProjectileMovementComponent* GetMovement() const { return ProjectileMovement; }

	/////////////////////////////////////////////// INTERNAL ///////////////////////////////////////////

protected:

private:

};
