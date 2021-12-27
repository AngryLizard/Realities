// The Gateway of Realities: Planes of Existence.

#pragma once

#include "AnimationSystem/TGOR_AnimationInstance.h"

#include "DimensionSystem/Content/TGOR_Spawner.h"
#include "TGOR_Archetype.generated.h"

class UTGOR_AnimationComponent;
class UTGOR_AnimInstance;
class UTGOR_Animation;
class UTGOR_Performance;
class ATGOR_PreviewActor;
class UTGOR_RigParam;

/**
 * 
 */
UCLASS(Blueprintable)
class ANIMATIONSYSTEM_API UTGOR_Archetype : public UTGOR_Spawner
{
	GENERATED_BODY()
	
public:
	UTGOR_Archetype();
	virtual TSubclassOf<AActor> GetSpawnClass_Implementation() const override;

	////////////////////////////////////////////////////////////////////////////////////////////////////
protected:

	/**
	* The default actor of this archetype to spawn it in the world.
	*/
	UPROPERTY(EditAnywhere, Category = "!TGOR Animation")
		TSubclassOf<ATGOR_PreviewActor> DefaultCustomisationActor;

	////////////////////////////////////////////////////////////////////////////////////////////////////
public:
	
	UFUNCTION(BlueprintImplementableEvent)
		void UpdateAnimInstance(const TMap<UTGOR_RigParam*, FTGOR_Normal>& Values);

	/** Gets rig parameters from given customisation */
	UFUNCTION(BlueprintPure, Category = "!TGOR Animation", Meta = (Keywords = "C++"))
		float GetRigParamValue(TSubclassOf<UTGOR_RigParam> RigParamType, const TMap<UTGOR_RigParam*, FTGOR_Normal>& Values, float Default = 0.5f) const;

	UFUNCTION(BlueprintPure, Category = "!TGOR Animation", Meta = (DeterminesOutputType = "Type", Keywords = "C++"))
		UTGOR_Performance* GetPerformance(TSubclassOf<UTGOR_Performance> Type) const;

	UFUNCTION(BlueprintPure, Category = "!TGOR Animation", Meta = (DeterminesOutputType = "Type", Keywords = "C++"))
		UTGOR_RigParam* GetRigParam(TSubclassOf<UTGOR_RigParam> Type) const;


	UFUNCTION(BlueprintCallable, Category = "!TGOR Animation", Meta = (Keywords = "C++"))
		virtual UTGOR_AnimInstance* GetAnimationInstance(UTGOR_AnimationComponent* Target);

	////////////////////////////////////////////////////////////////////////////////////////////////////
public:

	/** Supported animations */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "!TGOR Insertion")
		TArray<TSubclassOf<UTGOR_Animation>> AnimationInsertions;
	DECL_INSERTION(AnimationInsertions);

	/** Supported performances */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "!TGOR Insertion")
		TArray<TSubclassOf<UTGOR_Performance>> PerformanceInsertions;
	DECL_INSERTION(PerformanceInsertions);

	/** Supported rig params with default value */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "!TGOR Insertion")
		TArray<TSubclassOf<UTGOR_RigParam>> RigParamInsertions;
	DECL_INSERTION(RigParamInsertions);


	virtual void MoveInsertion(UTGOR_Content* Insertion, ETGOR_InsertionActionEnumeration Action, bool& Success) override;

};
