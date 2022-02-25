// The Gateway of Realities: Planes of Existence.

#pragma once

#include "CoreMinimal.h"
#include "Realities/Base/Instances/Combat/TGOR_AimInstance.h"
#include "Realities/Components/Dimension/TGOR_DimensionComponent.h"
#include "Realities/Components/Combat/TGOR_AimComponent.h"
#include "TGOR_HitVolumeComponent.generated.h"

class ATGOR_PhysicsVolume;

USTRUCT(BlueprintType)
struct FTGOR_HitVolumeHandle
{
	GENERATED_USTRUCT_BODY()
		FTGOR_HitVolumeHandle();

	/** Register to poll for activity */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "TGOR Aim")
		TScriptInterface<ITGOR_RegisterInterface> Register;

	/** Registered HitVolume actor */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "TGOR Aim")
		ATGOR_HitVolume* HitVolume;

	/** Whether this HitVolume stays after the content handle ceases */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "TGOR Aim")
		bool Autonomous;
};

/**
 * 
 */
UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class REALITIES_API UTGOR_HitVolumeComponent : public UTGOR_AimComponent
{
	GENERATED_BODY()

public:
	UTGOR_HitVolumeComponent();
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	/** Spawn or (if already exist) just shoot HitVolume of given type towards current aim */
	UFUNCTION(BlueprintCallable, Category = "TGOR Combat", Meta = (Keywords = "C++"))
		void ShootHitVolume(TScriptInterface<ITGOR_RegisterInterface> Register, UTGOR_Content* Content, TSubclassOf<ATGOR_HitVolume> Type, const FTransform& Transform, const FTGOR_ForceInstance& Forces, bool Autonomous = true);

	/** Remove manually, only remove non-autonomous if false */
	UFUNCTION(BlueprintCallable, Category = "TGOR Aim", Meta = (Keywords = "C++"))
		void KillHitVolume(UTGOR_Content* Content, bool Autonomous = true);


	////////////////////////////////////////////////////////////////////////////////////////////////////
protected:

	/** Handles for aim suspension */
	UPROPERTY(BlueprintReadOnly, Category = "TGOR Bodypart")
		TMap<UTGOR_Content*, FTGOR_HitVolumeHandle> HitVolumeHandles;
	
};
