// The Gateway of Realities: Planes of Existence.

#pragma once

#include "Realities/CoreSystem/Mods/Content/TGOR_CoreContent.h"
#include "TGOR_Animator.generated.h"

class ITGOR_AnimationInterface;
class UTGOR_SubAnimInstance;
class UTGOR_AnimInstance;
class UTGOR_Performance;
class ATGOR_Pawn;

/**
 * 
 */

UCLASS(Blueprintable)
class REALITIES_API UTGOR_Animator : public UTGOR_CoreContent
{
	GENERATED_BODY()
	
public:
	UTGOR_Animator();
	virtual bool Validate() override;


	////////////////////////////////////////////////////////////////////////////////////////////////////

	UFUNCTION(BlueprintCallable, Category = "!TGOR Animation", Meta = (Keywords = "C++"))
		virtual TSubclassOf<UTGOR_AnimInstance> GetInstanceClass() const;


	UFUNCTION(BlueprintCallable, Category = "!TGOR Animation", Meta = (Keywords = "C++"))
		virtual UTGOR_AnimInstance* GetAnimationInstance(TScriptInterface<ITGOR_AnimationInterface> Target);

};
