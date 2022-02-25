///////////////////////////////////////////////////
// The Gateway of Realities: Planes of Existence //
// Copyright 2016-2017                           //
///////////////////////////////////////////////////

#pragma once

#include "Realities/Components/Combat/Stat/TGOR_StatComponent.h"
#include "Realities/Utility/Datastructures/TGOR_TimestampQueue.h"

#include "TGOR_HealthComponent.generated.h"

UENUM(BlueprintType)
enum class ETGOR_HealthState : uint8
{
	Normal,
	Downed,
	Dead
};


DECLARE_DYNAMIC_MULTICAST_DELEGATE(FStateChangeDelegate);

/**
* UTGOR_HealthComponent adds health functionality to an actor.
*/
UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class REALITIES_API UTGOR_HealthComponent : public UTGOR_StatComponent
{
	GENERATED_BODY()

public:	
	UTGOR_HealthComponent();
	virtual void GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const override;
	virtual void ChangeStat(float Amount) override;
	virtual bool CanRegen() const override;

	////////////////////////////////////////////////////////////////////////////////////////////////////

	UPROPERTY(BlueprintAssignable, Category = "TGOR Vitals")
		FStateChangeDelegate OnDowned;

	UPROPERTY(BlueprintAssignable, Category = "TGOR Vitals")
		FStateChangeDelegate OnRecoverFromDowned;

	UPROPERTY(BlueprintAssignable, Category = "TGOR Vitals")
		FStateChangeDelegate OnDeath;

	////////////////////////////////////////////////////////////////////////////////////////////////////

	/** Called on the client when a new state has been replicated from the server. */
	UFUNCTION()
		void OnStateReplicated(ETGOR_HealthState PreviousState);


	/** Actor's health will be clamped at 1 health. Prevents downed or death state. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "TGOR Vitals")
		bool Immortal;
	
	/** Actor can enter downed state, if disabled 0 health will result in instant death state. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "TGOR Vitals")
		bool EnableDownedState;

	/** Actor's current health state(normal, downed, dead). */
	UPROPERTY(ReplicatedUsing = OnStateReplicated, BlueprintReadOnly, Category = "TGOR Vitals")
		ETGOR_HealthState HealthState;

	////////////////////////////////////////////////////////////////////////////////////////////////////
};
