// The Gateway of Realities: Planes of Existence.


#include "TGOR_PhysicsInterface.h"


// Add default functionality here for any ITGOR_PhysicsObjectInterface functions that are not pure virtual.


float ITGOR_PhysicsInterface::ComputeTotalMass()
{
	return(0.0f);
}


void ITGOR_PhysicsInterface::InvalidateTotalMass()
{

}

void ITGOR_PhysicsInterface::ResolveSingle(FVector& PointVelocity, float Coeff, const FVector& HitNormal, const FVector& HitLocation, float Collective)
{

}

void ITGOR_PhysicsInterface::ApplySingle(float Coeff, const FVector& HitNormal, const FVector& HitLocation, float Collective)
{

}

void ITGOR_PhysicsInterface::ResolveMulti(FVector& PointVelocity, float Coeff, TScriptInterface<ITGOR_PhysicsInterface> Responder, const FVector& HitNormal, const FVector& HitLocation, float Collective)
{

}