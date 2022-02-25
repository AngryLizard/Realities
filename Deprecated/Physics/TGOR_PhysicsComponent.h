// The Gateway of Realities: Planes of Existence.

#pragma once

#include "CoreMinimal.h"
#include "Realities/Utility/Error/TGOR_UnitTest.h"
#include "Realities/Components/Physics/TGOR_PhysicsObjectInterface.h"

#include "Components/SceneComponent.h"
#include "TGOR_PhysicsComponent.generated.h"

/**
 * 
 */
UCLASS(ClassGroup = (Custom), Blueprintable, meta = (BlueprintSpawnableComponent))
class REALITIES_API UTGOR_PhysicsComponent : public USceneComponent, public ITGOR_PhysicsObjectInterface
{
	GENERATED_BODY()
	TGOR_UNITTEST_BODY_BEGINPLAY

public:
	UTGOR_PhysicsComponent();

	virtual FVector ApplyMove(float DeltaTime, const FVector& External, const TArray<AActor*>& Ignore) override;

	virtual void SetParent(TScriptInterface<ITGOR_PhysicsObjectInterface> ParentInterface) override;
	virtual FTGOR_InertiaTensor ComputeInertiaTensor() override;
	virtual void InvalidateInertiaTensor() override;

	virtual float ComputeTotalMass() override;
	virtual void InvalidateTotalMass() override;

	virtual FVector GetOffset() const override;
	virtual void MoveOffset(const FVector& Delta) override;
	virtual FVector GetLocation() const override;
	virtual void Move(const FVector& Delta) override;
	virtual void ResolveSingle(FVector& PointVelocity, float Coeff, const FVector& HitNormal, const FVector& HitLocation, float Collective) override;
	virtual void ApplySingle(float Coeff, const FVector& HitNormal, const FVector& HitLocation, float Collective) override;

	/** Current location*/
	UPROPERTY(BlueprintReadOnly, Category = "TGOR Physics", Meta = (Keywords = "C++"))
		FVector CurrentLocation;

	/** Current local velocity*/
	UPROPERTY(BlueprintReadOnly, Category = "TGOR Physics", Meta = (Keywords = "C++"))
		FQuat CurrentRotation;


	/** Current velocity*/
	UPROPERTY(BlueprintReadOnly, Category = "TGOR Physics", Meta = (Keywords = "C++"))
		FVector CurrentVelocity;

	/** Current spin */
	UPROPERTY(BlueprintReadOnly, Category = "TGOR Physics", Meta = (Keywords = "C++"))
		FVector CurrentSpin;

	
	/** Pending impacts */
	UPROPERTY(BlueprintReadOnly, Category = "TGOR Physics", Meta = (Keywords = "C++"))
		float PendingCoeff;

	UPROPERTY(BlueprintReadOnly, Category = "TGOR Physics", Meta = (Keywords = "C++"))
		FVector PendingFreedom;

	UPROPERTY(BlueprintReadOnly, Category = "TGOR Physics", Meta = (Keywords = "C++"))
		FVector PendingHitOffset;

	/** Amount of pending forces */
	UPROPERTY(BlueprintReadOnly, Category = "TGOR Physics", Meta = (Keywords = "C++"))
		float PendingWeight;



	/** True if Inertia tensor needs to be recomputed */
	UPROPERTY(BlueprintReadOnly, Category = "TGOR Physics", Meta = (Keywords = "C++"))
		bool DirtyInertiaTensor;

	/** Current inertia tensor */
	UPROPERTY(BlueprintReadOnly, Category = "TGOR Physics", Meta = (Keywords = "C++"))
		FTGOR_InertiaTensor InertiaTensor;

	/** Current inversion inertia tensor */
	UPROPERTY(BlueprintReadOnly, Category = "TGOR Physics", Meta = (Keywords = "C++"))
		FTGOR_InertiaTensor InertiaInverse;

	/** True if total mass needs to be recomputed */
	UPROPERTY(BlueprintReadOnly, Category = "TGOR Physics", Meta = (Keywords = "C++"))
		bool DirtyTotalMass;

	/** Current total mass */
	UPROPERTY(BlueprintReadOnly, Category = "TGOR Physics")
		float TotalMass;


	/** Parent physics object */
	UPROPERTY(BlueprintReadOnly, Category = "TGOR Physics")
		TScriptInterface<ITGOR_PhysicsObjectInterface> Parent;

	/** Currently registered children */
	UPROPERTY(BlueprintReadOnly, Category = "TGOR Physics", Meta = (Keywords = "C++", ClampMin = "1"))
		TArray<TScriptInterface<ITGOR_PhysicsObjectInterface>> PhysicsComponents;
	

	/** Applies a force */
	UFUNCTION(BlueprintCallable, Category = "TGOR Physics", Meta = (Keywords = "C++"))
		void ApplyForce(float Coeff, const FVector& Freedom, const FVector& HitOffset, float Collective);


	/** Projects to degree of freedom */
	UFUNCTION(BlueprintPure, Category = "TGOR Physics", Meta = (Keywords = "C++"))
		virtual FVector FreedomProject(const FVector& Vector, const FVector& Offset) const;

	/** Computes internal forces (spring, piston...) */
	UFUNCTION(BlueprintPure, Category = "TGOR Physics", Meta = (Keywords = "C++"))
		virtual FVector ComputeInnerForce();
	
	/**  Computes internal spin (motor, spring...) */
	UFUNCTION(BlueprintPure, Category = "TGOR Physics", Meta = (Keywords = "C++"))
		virtual FVector ComputeInnerSpin();

	/** Updates structure in case of changes that effect the inertia tensor */
	UFUNCTION(BlueprintCallable, Category = "TGOR Physics", Meta = (Keywords = "C++"))
		virtual void UpdateStructure();


	/** Applies inner and outer force */
	UFUNCTION(BlueprintCallable, Category = "TGOR Physics", Meta = (Keywords = "C++"))
		void ApplyForces(const FVector& External, float DeltaTime);

	/** Computes location change due to restriction */
	UFUNCTION(BlueprintPure, Category = "TGOR Physics", Meta = (Keywords = "C++"))
		FVector ComputeRestrictionDelta();

	/** Computes velocity of this body */
	UFUNCTION(BlueprintPure, Category = "TGOR Physics", Meta = (Keywords = "C++"))
		FVector ComputeLocationDelta(float DeltaTime);

	/** Computes spin of this body */
	UFUNCTION(BlueprintPure, Category = "TGOR Physics", Meta = (Keywords = "C++"))
		FQuat ComputeRotationDelta(float DeltaTime);


	/** Computes local inertia tensors */
	UFUNCTION(BlueprintPure, Category = "TGOR Physics", Meta = (Keywords = "C++"))
		FTGOR_InertiaTensor ComputeLocalInertiaTensor();

	/** Computes parallel axis theorem matrix */
	UFUNCTION(BlueprintPure, Category = "TGOR Physics", Meta = (Keywords = "C++"))
		static FTGOR_InertiaTensor ComputeParallelAxisTheorem(const FVector& Offset, float Mass);

	/** Computes F*T/F to compute inertaitensor with correct scale and  */
	UFUNCTION(BlueprintPure, Category = "TGOR Physics", Meta = (Keywords = "C++"))
		static FTGOR_InertiaTensor ComputeTransform(const FTransform& Transform, const FTGOR_InertiaTensor& Tensor);

	TGOR_UNITTEST_BEGIN
	TGOR_ENSURE_STATIC( InertiaTensor,
		{
			FTGOR_InertiaTensor Tensor(
				FVector(5.0f, 3.0f, 2.0f),
				FVector(8.0f, 1.0f, 4.0f),
				FVector(7.0f, 5.0f, 5.0f));

			FTransform Transform(
				FVector(0.0f,  1.0f, 2.0f),
				FVector(2.0f, -2.0f, 1.0f),
				FVector(-5.0f, -4.0f, 2.0f),
				FVector::ZeroVector);

			FTGOR_InertiaTensor Expected(
				FVector(-3.2222f,  2.8222f, -10.9111f),
				FVector(-1.3333f, -1.2667f, -18.8667f),
				FVector( 1.7778f,  1.0222f,  15.4889f));

			FTGOR_InertiaTensor Result = UTGOR_PhysicsComponent::ComputeTransform(Transform, Tensor);
			return(Result == Expected);
		});
	TGOR_UNITTEST_END
};
