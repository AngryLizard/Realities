// The Gateway of Realities: Planes of Existence.

#pragma once

#include "CoreMinimal.h"

#include "CustomisationSystem/TGOR_BodypartInstance.h"
#include "UISystem/Queries/TGOR_Query.h"
#include "TGOR_AppearanceQuery.generated.h"

class UTGOR_ModularSkeletalMeshComponent;
class UTGOR_Modular;

/**
* TGOR_CustomisationQuery provides an interface for the customisation menu to communicate with
*/
UCLASS(Blueprintable, DefaultToInstanced)
class CUSTOMISATIONSYSTEM_API UTGOR_AppearanceQuery : public UTGOR_Query
{
	GENERATED_BODY()
	
public:
	UTGOR_AppearanceQuery();

	////////////////////////////////////////////////////////////////////////////////////////////////////
public:

	/** Provides skeletal Mesh, tells whether appearance was reverted from cache. */
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "!TGOR Creature", Meta = (Keywords = "C++"))
		void ApplyAppearance(const FTGOR_AppearanceInstance& Appearance, UTGOR_Modular* Modular, bool Revert);

	/** Provides skeletal Mesh. */
	UFUNCTION(BlueprintImplementableEvent, BlueprintPure, Category = "!TGOR Creature", Meta = (Keywords = "C++"))
		UTGOR_ModularSkeletalMeshComponent* GetSkeletalMesh() const;

	/** Provides creature content. */
	UFUNCTION(BlueprintImplementableEvent, BlueprintPure, Category = "!TGOR Creature", Meta = (Keywords = "C++"))
		UTGOR_Modular* GetModular() const;


	/** Initialise appearance and cache */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Creature", Meta = (Keywords = "C++"))
		void CreatureSetup();

	/** Apply cached appearance */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Creature", Meta = (Keywords = "C++"))
		void ApplyCached();

	/** Sets cache to current appearance */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Creature", Meta = (Keywords = "C++"))
		void CacheCurrent();

	////////////////////////////////////////////////////////////////////////////////////////////////////
protected:

	UPROPERTY(BlueprintReadOnly, Category = "!TGOR Creature")
		FTGOR_AppearanceInstance AppearanceCache;

};
