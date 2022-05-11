// The Gateway of Realities: Planes of Existence.

#pragma once

#include "CoreMinimal.h"

#include "TargetSystem/TGOR_AimInstance.h"

#include "Components/SphereComponent.h"
#include "TGOR_DamageComponent.generated.h"

class USkeletalMeshComponent;

/**
 * 
 */
UCLASS(ClassGroup = (Custom), Blueprintable, meta = (BlueprintSpawnableComponent))
class ACTIONSYSTEM_API UTGOR_DamageComponent : public USphereComponent
{
	GENERATED_BODY()

public:
	UTGOR_DamageComponent();
	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	////////////////////////////////////////////////////////////////////////////////////////////////////
public:

	/** Initiate damage component, first time setup */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Combat|Internal", Meta = (Keywords = "C++"))
		void InitialiseDamage(USkeletalMeshComponent* MeshComp, float Radius, float TotalTime, float Strength);

	////////////////////////////////////////////////////////////////////////////////////////////////////
public:

	/** Apply forces on interactables within given params */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Combat|Internal", Meta = (Keywords = "C++"))
		void HitAround(const FVector& Origin, float Radius, const FTGOR_ForceInstance& Forces);

	/** Apply forces on interactables within this sphere component */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Combat|Internal", Meta = (Keywords = "C++"))
		void HitAroundSelf(const FTGOR_ForceInstance& Forces);

	////////////////////////////////////////////////////////////////////////////////////////////////////
public:

	/** Channel used for hits */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "!TGOR Combat")
		TEnumAsByte<ECollisionChannel> HitChannel;

	UFUNCTION()
		void OnDamageOverlap(UPrimitiveComponent* OverlappedComp, AActor* Other, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	virtual FPrimitiveSceneProxy* CreateSceneProxy() override;

	////////////////////////////////////////////////////////////////////////////////////////////////////
private:

	float TimelineTotalTime = 1.f;
	float TimelineCurrentTime = 0.f;
	float ImpactStrength = 1.f;

};
