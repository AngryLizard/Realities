// The Gateway of Realities: Planes of Existence.

#pragma once

#include "Realities/Base/Content/TGOR_Content.h"
#include "TGOR_Animation.generated.h"

class UTGOR_SubAnimInstance;

/**
 * 
 */

UCLASS(Blueprintable)
class REALITIES_API UTGOR_Animation : public UTGOR_Content
{
	GENERATED_BODY()
	
public:
	UTGOR_Animation();

	////////////////////////////////////////////////////////////////////////////////////////////////////

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
		float BlendTime;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
		TSubclassOf<UTGOR_SubAnimInstance> InstanceClass;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
		float PhysicsStrength;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
		TMap<FName, FName> PhysicsProfiles;

	////////////////////////////////////////////////////////////////////////////////////////////////////

	UFUNCTION(BlueprintCallable, Category = "TGOR Animation", Meta = (Keywords = "C++"))
		UTGOR_SubAnimInstance* GetAnimationInstance(USkeletalMeshComponent* SkeletonComponent, FName Slot);


#if WITH_EDITOR
	
	virtual void PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent) override;

#endif // WITH_EDITOR
};
