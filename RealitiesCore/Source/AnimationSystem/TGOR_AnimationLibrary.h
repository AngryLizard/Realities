// The Gateway of Realities: Planes of Existence.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "TGOR_AnimationLibrary.generated.h"

class UTGOR_AttachCone;
class UTGOR_AttachComponent;

class UTGOR_Ellipsoid;
class UTGOR_EllipsoidComponent;

class UTGOR_Primitive;
class UTGOR_HandleComponent;

UCLASS()
class ANIMATIONSYSTEM_API UTGOR_AnimationLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_UCLASS_BODY()

	/** Find attach cone for given type */
	UFUNCTION(BlueprintPure, Category = "!TGOR Movement", Meta = (Keywords = "C++"))
		static UTGOR_AttachComponent* GetAttachConeComponent(AActor* Owner, TSubclassOf<UTGOR_AttachCone> Type);

	/** Find attach cones for given types */
	UFUNCTION(BlueprintPure, Category = "!TGOR Movement", Meta = (Keywords = "C++"))
		static void GetAttachConeComponents(TArray<UTGOR_AttachComponent*>& Cones, AActor* Owner, const TSet<TSubclassOf<UTGOR_AttachCone>>& Types);

	////////////////////////////////////////////////////////////////////////////////////////////////////
	
	/** Find ellipsoid for given type */
	UFUNCTION(BlueprintPure, Category = "!TGOR Animation", Meta = (Keywords = "C++"))
		static UTGOR_EllipsoidComponent* GetEllipsoidComponent(AActor* Owner, TSubclassOf<UTGOR_Ellipsoid> Type);

	/** Find ellipsoids for given types */
	UFUNCTION(BlueprintPure, Category = "!TGOR Animation", Meta = (Keywords = "C++"))
		static void GetEllipsoidComponents(TArray<UTGOR_EllipsoidComponent*>& Ellipsoids, AActor* Owner, const TSet<TSubclassOf<UTGOR_Ellipsoid>>& Types);

	////////////////////////////////////////////////////////////////////////////////////////////////////

	/** Find handle for given type */
	UFUNCTION(BlueprintPure, Category = "!TGOR Animation", Meta = (Keywords = "C++"))
		static UTGOR_HandleComponent* GetHandleComponent(AActor* Owner, TSubclassOf<UTGOR_Primitive> Type, bool EnforceCone);

	/** Find handles for given types */
	UFUNCTION(BlueprintPure, Category = "!TGOR Animation", Meta = (Keywords = "C++"))
		static void GetHandleComponents(TArray<UTGOR_HandleComponent*>& Handles, AActor* Owner, const TSet<TSubclassOf<UTGOR_Primitive>>& Types, bool EnforceCone);
};
