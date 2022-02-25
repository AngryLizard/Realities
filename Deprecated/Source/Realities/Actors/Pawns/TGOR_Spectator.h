// TGOR (C) // CHECKED //
#pragma once

#include "CoreMinimal.h"
#include "Realities/Actors/Pawns/TGOR_Pawn.h"
#include "Realities/Base/Instances/Creature/TGOR_CreatureInstance.h"
#include "Realities/Utility/Datastructures/TGOR_Time.h"
#include "TGOR_Spectator.generated.h"

class UTGOR_PhysicsComponent;
class UTGOR_EquipmentComponent;

UCLASS()
class REALITIES_API ATGOR_Spectator : public ATGOR_Pawn
{
	GENERATED_BODY()

public:

	////////////////////////////////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////// ENGINE ////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////////////////////////////

	ATGOR_Spectator(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	virtual void GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const override;
	virtual bool Assemble(UTGOR_DimensionData* Dimension) override;
	virtual void UnPossessed() override;
	
	////////////////////////////////////////////////// COMPONENTS //////////////////////////////////////
private:
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Character, Meta = (AllowPrivateAccess = "true"))
		UTGOR_PhysicsComponent* MovementComponent;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Character, Meta = (AllowPrivateAccess = "true"))
		UTGOR_EquipmentComponent* EquipmentComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Character, meta = (AllowPrivateAccess = "true"))
		UTGOR_AimComponent* AimComponent;

public:

	FORCEINLINE UTGOR_EquipmentComponent* GetEquipment() const { return EquipmentComponent; }
	FORCEINLINE UTGOR_PhysicsComponent* GetMovement() const { return MovementComponent; }
	FORCEINLINE UTGOR_AimComponent* GetAim() const { return AimComponent; }

public:

	/** Request creation of a new body */
	UFUNCTION(BlueprintCallable, Reliable, Server, WithValidation, Category = "TGOR Dimension", Meta = (Keywords = "C++"))
		void RequestNewBody(FTGOR_CreatureSetupInstance Setup);
	virtual void RequestNewBody_Implementation(FTGOR_CreatureSetupInstance Setup);
	virtual bool RequestNewBody_Validate(FTGOR_CreatureSetupInstance Setup);

	/////////////////////////////////////////////// INTERNAL ///////////////////////////////////////////

	/** Returns Transform of where requested character ought to spawn. */
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, BlueprintAuthorityOnly, Category = "TGOR Pawn", Meta = (Keywords = "C++"))
		void GetSpawnTransform(FTransform& Transform) const;

	/** Body request callback. */
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, BlueprintAuthorityOnly, Category = "TGOR Pawn", Meta = (Keywords = "C++"))
		void OnBodyRequest(ATGOR_Avatar* Avatar);

protected:

private:

};
