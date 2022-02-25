// The Gateway of Realities: Planes of Existence.

#pragma once

#include "CoreMinimal.h"
#include "Realities/Components/Dimension/TGOR_RegionComponent.h"
#include "TGOR_InfluencableComponent.generated.h"

class UTGOR_CombatComponent;
class ATGOR_HitVolume;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FInfluenceVisualDelegate, const FVector&, Location);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FAmbientInfluenceChangeDelegate, FTGOR_ElementInstance, State);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FDirectInfluenceChangeDelegate, FTGOR_ElementInstance, State, AActor*, Instigator);

/**
 * 
 */
UCLASS()
class REALITIES_API UTGOR_InfluencableComponent : public UTGOR_RegionComponent
{
	GENERATED_BODY()

public:
	UTGOR_InfluencableComponent();

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	////////////////////////////////////////////////////////////////////////////////////////////////////
protected:

	UPROPERTY(BlueprintAssignable, Category = "TGOR Influence")
		FInfluenceVisualDelegate OnInfluenceVisual;

	UPROPERTY(BlueprintAssignable, Category = "TGOR Influence")
		FDirectInfluenceChangeDelegate OnDirectInfluence;

	UPROPERTY(BlueprintAssignable, Category = "TGOR Influence")
		FAmbientInfluenceChangeDelegate OnAmbientInfluence;

	////////////////////////////////////////////////////////////////////////////////////////////////////
public:

	/** Influence this component if not in Invincibility Frames (always applies with negative identifier) */
	UFUNCTION(BlueprintCallable, Category = "TGOR Influence", Meta = (Keywords = "C++"))
		void DirectInfluence(const FTGOR_ElementInstance& State, ATGOR_HitVolume* HitVolume, AActor* Instigator, int32 Identifier, int32 Segment, float Time, const FHitResult& Hit);

	/** Influence this component */
	UFUNCTION(BlueprintCallable, Category = "TGOR Influence", Meta = (Keywords = "C++"))
		void AmbientInfluence();

	/** Interface for InfluenceComponents */
	UFUNCTION(BlueprintCallable, Category = "TGOR Influence", Meta = (Keywords = "C++"))
		void AccumulateAmbient(const FTGOR_ElementInstance& State);

	////////////////////////////////////////////////////////////////////////////////////////////////////

	/** Component Influenced */
	UFUNCTION(BlueprintCallable, Category = "TGOR Influence", Meta = (Keywords = "C++"))
		virtual void DirectInfluenced(const FTGOR_ElementInstance& State, AActor* Instigator);

	/** Component Influenced */
	UFUNCTION(BlueprintCallable, Category = "TGOR Influence", Meta = (Keywords = "C++"))
		virtual void AmbientInfluenced(const FTGOR_ElementInstance& State);

	/** Callback from UE4 damage system when damage is dealt to the owning actor. */
	UFUNCTION()
		void OnDamageHealthEvent(AActor* DamagedActor, float Points, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser);

	////////////////////////////////////////////////////////////////////////////////////////////////////

	/** Compute region states */
	UFUNCTION(BlueprintCallable, Category = "TGOR Influence", Meta = (Keywords = "C++"))
		virtual void ComputeState(FTGOR_ElementInstance& State);
		void LerpField(FTGOR_ElementInstance& State, float FTGOR_ElementInstance::*Field, float CurrentLerp, float OppositeLerp, float PrevLerp, float NextLerp, float Lerp);

	////////////////////////////////////////////////////////////////////////////////////////////////////
protected:

	/** Stores damage taken with an index to prevent repeat damage */
	UPROPERTY(BlueprintReadOnly, Category = "TGOR Influence")
		TMap<int32, float> InvincibilityFrames;
	
private:
	FTGOR_ElementInstance _posMax;
	FTGOR_ElementInstance _negMin;
};
