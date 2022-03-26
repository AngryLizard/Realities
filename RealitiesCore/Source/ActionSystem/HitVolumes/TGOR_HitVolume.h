#pragma once

#include "CoreMinimal.h"

#include "TargetSystem/TGOR_AimInstance.h"

#include "CoreSystem/Actors/TGOR_Actor.h"
#include "TGOR_HitVolume.generated.h"

class UTGOR_DamageComponent;
class UTGOR_HitVolumeComponent;
class ATGOR_PhysicsVolume;
class ATGOR_LevelVolume;
class UTGOR_Force;

/**
 * 
 */
UCLASS(Abstract)
class ACTIONSYSTEM_API ATGOR_HitVolume : public ATGOR_Actor
{
	GENERATED_BODY()

public:
	ATGOR_HitVolume(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	virtual void Tick(float DeltaSeconds) override;

	////////////////////////////////////////////// COMPONENTS ////////////////////////////////////////////
private:

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "!TGOR Combat", Meta = (AllowPrivateAccess = "true"))
		UTGOR_DamageComponent* DamageComponent;

public:

	FORCEINLINE UTGOR_DamageComponent* GetDamage() const { return DamageComponent; }

	////////////////////////////////////////////////////////////////////////////////////////////////////
public:

	/** Assign LevelVolume */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Combat", Meta = (Keywords = "C++"))
		void AssignOwnerVolume(ATGOR_LevelVolume* Volume);

	/** Assign Instigator */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Combat", Meta = (Keywords = "C++"))
		void AssignOwnerInstigator(AActor* Actor);

	/** Get volume given location and previous volume */
	UFUNCTION(BlueprintPure, Category = "!TGOR Combat|Internal", Meta = (Keywords = "C++"))
		ATGOR_PhysicsVolume* QueryPhysicsVolume(ATGOR_PhysicsVolume* Volume, const FVector& Location) const;

	/** Update HitVolume, return false if all activity has concluded and this Volume is to be despawned */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Combat|Internal", Meta = (Keywords = "C++"))
		virtual bool Update(float DeltaTime);

	/** Kill HitVolume, gets called automatically if Update returns false. Just despawns this actor by default. */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Combat|Internal", Meta = (Keywords = "C++"))
		virtual void Kill();

	/** Shoot from current actor location towards a target */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Combat|Internal", Meta = (Keywords = "C++"))
		virtual void Shoot(const FTGOR_AimInstance& Target, const FTGOR_ForceInstance& Forces);

	////////////////////////////////////////////////////////////////////////////////////////////////////
protected:

	/** Max DeltaTime before striding happens (to limit approximation error) */
	UPROPERTY(BlueprintReadOnly, Category = "!TGOR Combat")
		float MaxDeltaTime;

	/** Actor that spawned this HitVolume */
	UPROPERTY(BlueprintReadOnly, Category = "!TGOR Combat")
		AActor* OwnerInstigator;

	/** Level Volume this HitVolume got spawned into */
	UPROPERTY(BlueprintReadOnly, Category = "!TGOR Combat")
		ATGOR_LevelVolume* OwnerVolume;

	/** Channel used for hits */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "!TGOR Combat")
		TEnumAsByte<ECollisionChannel> HitChannel;

};
