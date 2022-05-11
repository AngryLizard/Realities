// The Gateway of Realities: Planes of Existence.

#pragma once

#include "CoreMinimal.h"
#include "RealitiesUtility/Structures/TGOR_Time.h"

#include "Components/SphereComponent.h"
#include "CoreSystem/Interfaces/TGOR_SingletonInterface.h"
#include "TGOR_ImpactComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FKnockbackDelegate, const FVector&, Direction, float, Strength);

/**
 * 
 */
UCLASS(ClassGroup = (Custom), Blueprintable, meta = (BlueprintSpawnableComponent))
class ACTIONSYSTEM_API UTGOR_ImpactComponent : public USphereComponent, public ITGOR_SingletonInterface
{
	GENERATED_BODY()

public:
	UTGOR_ImpactComponent();
	virtual void BeginPlay() override;

	////////////////////////////////////////////////////////////////////////////////////////////////////
public:

	UPROPERTY(BlueprintAssignable, Category = "!TGOR Combat")
		FKnockbackDelegate OnKnockbackDelegate;

	/** Apply impact to the owner of this component, applies knockback and damage if relevant. Returns whether we actually did damage. */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Combat", Meta = (Keywords = "C++"))
		bool ApplyImpact(const FVector& Direction, float Strength);

	/** Whether we're currently in invincibility frames */
	UFUNCTION(BlueprintPure, Category = "!TGOR Combat", Meta = (Keywords = "C++"))
		bool HasInvincibility() const;

	/** Up to a certain strength we ignore impacts */
	UPROPERTY(Transient, BlueprintReadOnly, Category = "!TGOR Combat")
		float KnockbackThreshold = 0.1f;

	/** Time in seconds invincibility frame lasts */
	UPROPERTY(Transient, BlueprintReadOnly, Category = "!TGOR Combat")
		float InvincibilityTotalTime = 1.0f;

	////////////////////////////////////////////////////////////////////////////////////////////////////
private:

	FTGOR_Time LastImpact;


};
