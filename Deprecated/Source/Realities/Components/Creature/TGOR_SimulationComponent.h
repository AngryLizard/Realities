// The Gateway of Realities: Planes of Existence.

#pragma once

#include "CoreMinimal.h"
#include "Realities/Base/Instances/Creature/TGOR_CreatureInstance.h"

#include "Realities/Components/Dimension/TGOR_InfluencableComponent.h"
#include "TGOR_SimulationComponent.generated.h"

class UTGOR_CombatComponent;
class ATGOR_Pawn;

/**
 * 
 */
UCLASS()
class REALITIES_API UTGOR_SimulationComponent : public UTGOR_InfluencableComponent
{
	GENERATED_BODY()
	
public:
	UTGOR_SimulationComponent();

	virtual void BeginPlay() override;
	virtual void RegionTick(float DeltaTime) override;

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	void GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const override;

	////////////////////////////////////////////////////////////////////////////////////////////////////
	
	/** Tick called only on the server */
	UFUNCTION(BlueprintCallable, Category = "TGOR Simulation", Meta = (Keywords = "C++"))
		void ServerTick(float DeltaTime);

	/** Tick called only on autonomous client */
	UFUNCTION(BlueprintCallable, Category = "TGOR Simulation", Meta = (Keywords = "C++"))
		void ClientTick(float DeltaTime);

	/** Tick called only on non owner server */
	UFUNCTION(BlueprintCallable, Category = "TGOR Simulation", Meta = (Keywords = "C++"))
		void AuthorityTick(float DeltaTime);

	/** Tick called only on locally controlled */
	UFUNCTION(BlueprintCallable, Category = "TGOR Simulation", Meta = (Keywords = "C++"))
		void OwnerTick(float DeltaTime);

	/** Tick only called on simulated clients */
	UFUNCTION(BlueprintCallable, Category = "TGOR Simulation", Meta = (Keywords = "C++"))
		void SimulatedTick(float DeltaTime);

	////////////////////////////////////////////////////////////////////////////////////////////////////

	virtual void DirectInfluenced(const FTGOR_ElementInstance& State, AActor* Instigator) override;
	virtual void AmbientInfluenced(const FTGOR_ElementInstance& State) override;
	
	////////////////////////////////////////////////////////////////////////////////////////////////////
	
	/** Exhaust stamina */
	UFUNCTION(BlueprintCallable, Category = "TGOR Simulation", Meta = (Keywords = "C++"))
		void Exhaust(float Amount, bool Sync);

	/** Exhaust energy */
	UFUNCTION(BlueprintCallable, Category = "TGOR Simulation", Meta = (Keywords = "C++"))
		void Deplete(float Amount, bool Sync);

	/** Exhaust health */
	UFUNCTION(BlueprintCallable, Category = "TGOR Simulation", Meta = (Keywords = "C++"))
		void Damage(float Amount, bool Sync);

	////////////////////////////////////////////////////////////////////////////////////////////////////

	/** Applies an organ setup to this character(nervous, digestive, etc.) */
	UFUNCTION(BlueprintCallable, Category = "TGOR Creature", Meta = (Keywords = "C++"))
		bool ApplySetup(const FTGOR_CreatureSetupInstance& Setup);

	/** Called on each client on replication of current setup structure. */
	UFUNCTION()
		void OnReplicateSetup();

	////////////////////////////////////////////////////////////////////////////////////////////////////
protected:

	/** Serverside character setup information */
	UPROPERTY(ReplicatedUsing = OnReplicateSetup, BlueprintReadWrite, Category = "TGOR Creature")
		FTGOR_CreatureSetupInstance CurrentSetup;

	////////////////////////////////////////////////////////////////////////////////////////////////////
	
	/** Owner of the simulation */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "TGOR Pawn")
		ATGOR_Pawn* Pawn;

	/** */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "TGOR Simulation")
		float HealthRegenPerSecond;

	/** */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "TGOR Simulation")
		float DownedHealthRegenPerSecond;

	/** */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "TGOR Simulation")
		float EnergyRegenPerSecond;

	/** */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "TGOR Simulation")
		float StaminaRegenPerSecond;

private:

	/** Calls regeneration functions */
	void Regenerate();
};
