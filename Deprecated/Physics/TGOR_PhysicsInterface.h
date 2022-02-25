// The Gateway of Realities: Planes of Existence.

#pragma once

#include "CoreMinimal.h"

#include "UObject/Interface.h"
#include "TGOR_PhysicsInterface.generated.h"


// This class does not need to be modified.
UINTERFACE(MinimalAPI, meta = (CannotImplementInterfaceInBlueprint))
class UTGOR_PhysicsInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class REALITIES_API ITGOR_PhysicsInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:

	/** Get mass of this body */
	UFUNCTION(BlueprintCallable, Category = "TGOR Physics", Meta = (Keywords = "C++"))
		virtual float ComputeTotalMass();

	/** Forces recomputation of mass */
	UFUNCTION(BlueprintCallable, Category = "TGOR Physics", Meta = (Keywords = "C++"))
		virtual void InvalidateTotalMass();


	/** Resolve an impulse */
	UFUNCTION(BlueprintCallable, Category = "TGOR Physics", Meta = (Keywords = "C++"))
		virtual void ResolveSingle(FVector& PointVelocity, float Coeff, const FVector& HitNormal, const FVector& HitLocation, float Collective);

	/** Resolve an impulse */
	UFUNCTION(BlueprintCallable, Category = "TGOR Physics", Meta = (Keywords = "C++"))
		virtual void ResolveMulti(FVector& PointVelocity, float Coeff, TScriptInterface<ITGOR_PhysicsInterface> Responder, const FVector& HitNormal, const FVector& HitLocation, float Collective);

	/** Applies an impulse */
	UFUNCTION(BlueprintCallable, Category = "TGOR Physics", Meta = (Keywords = "C++"))
		virtual void ApplySingle(float Coeff, const FVector& HitNormal, const FVector& HitLocation, float Collective);
};
