// The Gateway of Realities: Planes of Existence.

#pragma once

#include "CoreSystem/Content/TGOR_CoreContent.h"
#include "TGOR_Animation.generated.h"

class ITGOR_AnimationInterface;
class UTGOR_SubAnimInstance;
class UTGOR_Performance;

/**
 * 
 */

UCLASS(Blueprintable)
class ANIMATIONSYSTEM_API UTGOR_Animation : public UTGOR_CoreContent
{
	GENERATED_BODY()
	
public:
	UTGOR_Animation();
	virtual bool Validate_Implementation() override;

	////////////////////////////////////////////////////////////////////////////////////////////////////
public:

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "!TGOR Animation")
		TSubclassOf<UTGOR_SubAnimInstance> InstanceClass;

	////////////////////////////////////////////////////////////////////////////////////////////////////

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "!TGOR Animation")
		float BlendTime;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "!TGOR Animation")
		float PhysicsStrength;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "!TGOR Animation")
		TMap<FName, FName> PhysicsProfiles;

	////////////////////////////////////////////////////////////////////////////////////////////////////

	UFUNCTION(BlueprintCallable, Category = "!TGOR Animation", Meta = (Keywords = "C++"))
		UTGOR_AnimInstance* GetAnimationInstance(TScriptInterface<ITGOR_AnimationInterface> Target);

#if WITH_EDITOR
	virtual void PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent) override;
#endif // WITH_EDITOR
};
