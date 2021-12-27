// The Gateway of Realities: Planes of Existence.

#pragma once

#include "CoreMinimal.h"

#include "../TGOR_AnimationInstance.h"

#include "Components/SkeletalMeshComponent.h"
#include "DimensionSystem/Interfaces/TGOR_SpawnerInterface.h"
#include "CoreSystem/Interfaces/TGOR_SingletonInterface.h"
#include "TGOR_AnimationComponent.generated.h"

class UTGOR_AnimatedTask;
class UTGOR_AnimInstance;
class UTGOR_Performance;

/**
 * 
 */
UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class ANIMATIONSYSTEM_API UTGOR_AnimationComponent : public USkeletalMeshComponent, public ITGOR_SpawnerInterface, public ITGOR_SingletonInterface
{
	GENERATED_BODY()

	friend class UTGOR_Animation;
	friend class UTGOR_Archetype;
	friend class UTGOR_AnimatedTask;

public:
	UTGOR_AnimationComponent();
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	virtual void GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const override;
	virtual void FinalizeBoneTransform() override;

	virtual void UpdateContent_Implementation(FTGOR_SpawnerDependencies& Dependencies) override;
	virtual TMap<int32, UTGOR_SpawnModule*> GetModuleType_Implementation() const override;

	//////////////////////////////////////////// IMPLEMENTABLES ////////////////////////////////////////

	////////////////////////////////////////////////////////////////////////////////////////////////////
public:

	////////////////////////////////////////////////////////////////////////////////////////////////////
public:

	/** Applies a loadout to this component */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Animation", Meta = (Keywords = "C++"))
		bool ApplyAnimationSetup(FTGOR_AnimationInstance Setup);

	/** Sets a given performance slot to another animation, returns true if the slot changed */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Animation", Meta = (Keywords = "C++"))
		bool SwitchAnimation(TSubclassOf<UTGOR_Performance> PerformanceType, UTGOR_AnimatedTask* AnimatedTask);

	/** Get currently running animation */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Animation", Meta = (Keywords = "C++"))
		UTGOR_AnimatedTask* GetAnimation(TSubclassOf<UTGOR_Performance> PerformanceType) const;

	/** Archetype this component spawns with. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "!TGOR Animation")
		TSubclassOf<UTGOR_Archetype> SpawnArchetype;

	////////////////////////////////////////////////////////////////////////////////////////////////////
private:

	/** Currently running animations */
	UPROPERTY()
		TSet<UTGOR_Performance*> PerformanceSlots;

	/** Currently running anim instance */
	UPROPERTY(Transient)
		TWeakObjectPtr<UTGOR_AnimInstance> AnimInstance;

	////////////////////////////////////////////////////////////////////////////////////////////////////
public:

	/** Serverside setup information */
	UPROPERTY(ReplicatedUsing = OnReplicateAnimationSetup, BlueprintReadOnly, Category = "!TGOR Animation")
		FTGOR_AnimationInstance AnimationSetup;

	/** Called on each client on replication of current action setup structure. */
	UFUNCTION()
		void OnReplicateAnimationSetup();
};
