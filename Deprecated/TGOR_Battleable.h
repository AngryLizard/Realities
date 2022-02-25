// TGOR (C) // CHECKED //
#pragma once

#include "Actors/Pawns/TGOR_Character.h"
#include "TGOR_Battleable.generated.h"

///////////////////////////////////////////////// DECL ///////////////////////////////////////////////////

class UTGOR_HealthComponent;
class UTGOR_StaminaComponent;
class UTGOR_EnergyComponent;
class UTGOR_AimComponent;
class UTGOR_UpperComponent;
class UTGOR_LowerComponent;
class ATGOR_Consciousness;

///////////////////////////////////////////////// ENUM ///////////////////////////////////////////////////

UENUM(BlueprintType)
enum class ETGOR_SocketTypes : uint8
{
	WeaponTip,
	WeaponBase,
	Eyes,
	Mouth,
	Butt,
	Genitalia,
	Chest,
	Legs,
	Feet
};

/**
* TGOR_Battleable provides actor functionality for combat. It handles locating targets
* as well as mode changes indicating whether the actor is in combat or not.
*/

UCLASS()
class REALITIES_API ATGOR_Battleable : public ATGOR_Character
{
	GENERATED_BODY()
	
public:

	////////////////////////////////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////// ENGINE ////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////////////////////////////

	ATGOR_Battleable(const FObjectInitializer& ObjectInitializer);

	virtual void BeginPlay() override;
	virtual void GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const override;
	virtual void Tick(float DeltaTime) override;
	virtual bool HasCursor_Implementation() override;
	
	//////////////////////////////////////////////// IMPLEMENTABLES /////////////////////////////////////////

	/** Aggressive mode on. */
	UFUNCTION(BlueprintImplementableEvent, Category = "TGOR|Combat", Meta = (Keywords = "C++"))
		void CombatEntered();

	/** Aggressive mode off. */
	UFUNCTION(BlueprintImplementableEvent, Category = "TGOR|Combat", Meta = (Keywords = "C++"))
		void CombatLeft();

	////////////////////////////////////////////// COMPONENTS //////////////////////////////////////////////////////

	/** Component that handles upper body. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "TGOR|Combat", meta = (AllowPrivateAccess = "true"))
		UTGOR_UpperComponent* UpperComponent;

	/** Component that handles the whole/lower body. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "TGOR|Combat", meta = (AllowPrivateAccess = "true"))
		UTGOR_LowerComponent* LowerComponent;

	/** Component that allows handling aiming. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "TGOR|System", meta = (AllowPrivateAccess = "true"))
		UTGOR_AimComponent* AimComponent;

	/** Health component */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "TGOR|System", meta = (AllowPrivateAccess = "true"))
		UTGOR_HealthComponent* HealthComponent;

	/** Stamina component */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "TGOR|System", meta = (AllowPrivateAccess = "true"))
		UTGOR_StaminaComponent* StaminaComponent;

	/** Energy component */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "TGOR|System", meta = (AllowPrivateAccess = "true"))
		UTGOR_EnergyComponent* EnergyComponent;

	////////////////////////////////////////////////////////////////////////////////////////////////////

	/** Spawns a consciousness and possesses it */
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "TGOR|Combat", Meta = (Keywords = "C++"))
		void SpawnAsConsciousness(TSubclassOf<ATGOR_Consciousness> Class, bool Despawn);

	/** Sets aggressive mode */
	UFUNCTION(BlueprintCallable, Category = "TGOR|Combat", Meta = (Keywords = "C++"))
		void SetIsAggressive(bool Aggressive);

	/** Sets aggressive mode */
	UFUNCTION(Reliable, Server, WithValidation, Category = "TGOR|Combat", Meta = (Keywords = "C++"))
		void ReplicateIsAggressive(bool Aggressive);
		void ReplicateIsAggressive_Implementation(bool Aggressive);
		bool ReplicateIsAggressive_Validate(bool Aggressive);

	/** Aggression mode */
	UPROPERTY(Replicated, BlueprintReadOnly, Category = "TGOR|Combat", Meta = (Keywords = "C++"))
		bool IsAggressive;

	/////////////////////////////////////////////// INTERNAL ///////////////////////////////////////////

protected:

	/** Return true if Aggressive mode can be entered (Does not leave aggressive when false) */
	virtual bool CanEnterAggressive();

	/** Aggressive mode changed */
	virtual void ChangedAggressive(bool Aggressive);

private:

};
