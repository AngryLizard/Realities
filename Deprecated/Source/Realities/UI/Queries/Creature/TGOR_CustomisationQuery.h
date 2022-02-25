// The Gateway of Realities: Planes of Existence.

#pragma once

#include "Realities/Base/Instances/Creature/TGOR_CreatureInstance.h"
#include "Realities/Base/Content/TGOR_Content.h"
#include "Templates/SubclassOf.h"

#include "Realities/UI/Queries/TGOR_Query.h"
#include "TGOR_CustomisationQuery.generated.h"

class UTGOR_ModularSkeletalMeshComponent;
class UTGOR_Creature;

/**
* TGOR_CustomisationQuery provides an interface for the customisation menu to communicate with
*/
UCLASS(Blueprintable, DefaultToInstanced)
class REALITIES_API UTGOR_CustomisationQuery : public UTGOR_Query
{
	GENERATED_BODY()
	
public:
	UTGOR_CustomisationQuery();

	////////////////////////////////////////////////////////////////////////////////////////////////////
public:

	/** Provides skeletal Mesh, tells whether appearance was reverted from cache. */
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "TGOR Creature", Meta = (Keywords = "C++"))
		void ApplyAppearance(const FTGOR_CreatureAppearanceInstance& Appearance, UTGOR_Creature* Creature, bool Revert);

	/** Provides skeletal Mesh. */
	UFUNCTION(BlueprintImplementableEvent, BlueprintPure, Category = "TGOR Creature", Meta = (Keywords = "C++"))
		UTGOR_ModularSkeletalMeshComponent* GetSkeletalMesh() const;

	/** Provides creature content. */
	UFUNCTION(BlueprintImplementableEvent, BlueprintPure, Category = "TGOR Creature", Meta = (Keywords = "C++"))
		UTGOR_Creature* GetCreature() const;


	/** Initialise appearance and cache */
	UFUNCTION(BlueprintCallable, Category = "TGOR Creature", Meta = (Keywords = "C++"))
		UTGOR_BodypartNode* CreatureSetup();

	/** Apply cached appearance */
	UFUNCTION(BlueprintCallable, Category = "TGOR Creature", Meta = (Keywords = "C++"))
		void ApplyCached();

	/** Sets cache to current appearance */
	UFUNCTION(BlueprintCallable, Category = "TGOR Creature", Meta = (Keywords = "C++"))
		void CacheCurrent();

	////////////////////////////////////////////////////////////////////////////////////////////////////
protected:

	UPROPERTY(BlueprintReadOnly, Category = "TGOR Creature")
		FTGOR_CreatureAppearanceInstance AppearanceCache;

};
