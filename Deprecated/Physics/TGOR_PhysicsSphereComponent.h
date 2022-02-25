// The Gateway of Realities: Planes of Existence.

#pragma once

#include "CoreMinimal.h"
#include "Realities/Utility/Error/TGOR_Error.h"
#include "Realities/Components/Physics/TGOR_PhysicsObjectInterface.h"

#include "Kismet/KismetSystemLibrary.h"
#include "Components/SphereComponent.h"
#include "TGOR_PhysicsSphereComponent.generated.h"

/**
 * 
 */
UCLASS(ClassGroup = (Custom), Blueprintable, meta = (BlueprintSpawnableComponent))
class REALITIES_API UTGOR_PhysicsSphereComponent : public USphereComponent, public ITGOR_PhysicsObjectInterface
{
	GENERATED_BODY()
	
public:
	UTGOR_PhysicsSphereComponent();
	virtual void BeginPlay() override;

	virtual FVector ApplyMove(float DeltaTime, const FVector& External, const TArray<AActor*>& Ignore) override;

	virtual void SetParent(TScriptInterface<ITGOR_PhysicsObjectInterface> ParentInterface) override;
	virtual FTGOR_InertiaTensor ComputeInertiaTensor() override;
	virtual float ComputeTotalMass() override;

	virtual FVector GetOffset() const override;
	virtual void MoveOffset(const FVector& Delta) override;
	virtual FVector GetLocation() const override;
	virtual void Move(const FVector& Delta) override;
	virtual void ResolveSingle(FVector& PointVelocity, float Coeff, const FVector& HitNormal, const FVector& HitLocation, float Collective) override;
	virtual void ApplySingle(float Coeff, const FVector& HitNormal, const FVector& HitLocation, float Collective) override;


	/** Parent physics object */
	UPROPERTY(BlueprintReadOnly, Category = "TGOR Physics")
		TScriptInterface<ITGOR_PhysicsObjectInterface> Parent;

	/** Mass to compute inertia and friction */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TGOR Physics")
		float Mass;

	/** Bouncyness */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TGOR Physics")
		float Bouncyness;

	/** Friction coefficient of this body*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TGOR Physics")
		float Friction;

	/** Drag coefficient of this body*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TGOR Physics")
		float Drag;

	
	/** Currently registered position in world space */
	UPROPERTY(BlueprintReadOnly, Category = "TGOR Physics")
		FVector CurrentLocation;


	/** Debug trace type */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TGOR Physics", Meta = (Keywords = "C++"))
		TEnumAsByte<EDrawDebugTrace::Type> DebugType;
	
};
