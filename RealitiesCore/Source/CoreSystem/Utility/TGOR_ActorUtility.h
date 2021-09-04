// The Gateway of Realities: Planes of Existence.

#pragma once

#include "RealitiesUtility/Utility/TGOR_Error.h"

#include "UObject/NoExportTypes.h"
#include "TGOR_ActorUtility.generated.h"

/**
 * 
 */
UCLASS()
class CORESYSTEM_API UTGOR_ActorUtility : public UObject
{
	GENERATED_BODY()

public:
		/** Calculates pitch and roll of a norm in mesh space*/
		UFUNCTION(BlueprintPure, Category = "!TGOR Utility", Meta = (Keywords = "C++"))
		static FVector2D CharacterIKFeetRotation(FVector Vector);
		
		/** Wrapper around FindTeleportSpot */
		UFUNCTION(BlueprintCallable, Category = "!TGOR Utility", Meta = (Keywords = "C++"))
		static bool FindTeleportSpot(AActor* Actor, FVector Location, FRotator Rotator, FVector& TargetLocation);
		
		/** Get water level if submerged, 0.0f if not */
		UFUNCTION(BlueprintPure, Category = "!TGOR Utility", Meta = (Keywords = "C++"))
		static float GetWaterLevel(ACharacter* Character);

		/** Get global vector in actor local space */
		UFUNCTION(BlueprintPure, Category = "!TGOR Utility", Meta = (Keywords = "C++"))
		static FVector GetLocalSpace(AActor* Actor, const FVector& Vector);

		/** Get actor local vector in global space */
		UFUNCTION(BlueprintPure, Category = "!TGOR Utility", Meta = (Keywords = "C++"))
		static FVector GetGlobalSpace(AActor* Actor, const FVector& Vector);

		/** Override mesh */
		UFUNCTION(BlueprintPure, Category = "!TGOR Utility", Meta = (Keywords = "C++"))
		static void OverrideMesh(UMeshComponent* Component, const TMap<uint8, UMaterialInstance*>& MaterialOverride);
};