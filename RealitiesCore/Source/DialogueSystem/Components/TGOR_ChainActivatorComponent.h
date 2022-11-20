// The Gateway of Realities: Planes of Existence.

#pragma once

#include "CoreMinimal.h"

#include "TGOR_ActivatorComponent.h"
#include "TGOR_ChainActivatorComponent.generated.h"

USTRUCT(BlueprintType)
struct FTGOR_ChainCurve
{
	GENERATED_USTRUCT_BODY();

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "!TGOR Interaction")
		FTransform Transform;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "!TGOR Interaction")
		float Tangent = 0.0f;
};

/**
 * UTGOR_ChainActivatorComponent
 */
UCLASS(ClassGroup = (Custom), Blueprintable, meta = (BlueprintSpawnableComponent))
class DIALOGUESYSTEM_API UTGOR_ChainActivatorComponent : public UTGOR_ActivatorComponent
{
	GENERATED_BODY()

public:
	UTGOR_ChainActivatorComponent();
	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	virtual FVector WorldToTarget(const FVector& Location) const override;
	virtual FVector TargetToWorld(const FVector& Local) const override;

	//////////////////////////////////////////// IMPLEMENTABLES ////////////////////////////////////////

	////////////////////////////////////////////////////////////////////////////////////////////////////
protected:

	/** Whether to debug draw every tick */
	UPROPERTY(EditDefaultsOnly, Category = "!TGOR Interaction")
		bool bEnableDebugDraw = false;

	/** Whether chain can be touched from this chain link */
	UPROPERTY(EditDefaultsOnly, Category = "!TGOR Interaction")
		bool bCanEnterChain = false;

	/** Index of this link within the chain */
	UPROPERTY()
		int32 ChainIndex = INDEX_NONE;

	////////////////////////////////////////////////////////////////////////////////////////////////////
public:

	/** Prev tangent length */
	UPROPERTY(EditAnywhere, Category = "!TGOR Interaction")
		float PrevChainTangent = 0.0f;

	/** Prev link component name */
	UPROPERTY(EditAnywhere, Category = "!TGOR Interaction")
		FName PrevChainLink;

	/** Next tangent length */
	UPROPERTY(EditAnywhere, Category = "!TGOR Interaction")
		float NextChainTangent = 0.0f;

	/** Next link component name */
	UPROPERTY(EditAnywhere, Category = "!TGOR Interaction")
		FName NextChainLink;

	////////////////////////////////////////////////////////////////////////////////////////////////////
public:

	UTGOR_ChainActivatorComponent* FindChainLink(const FName& Name) const;
	static void GetCurve(const UTGOR_ChainActivatorComponent* PrevLink, const UTGOR_ChainActivatorComponent* NextLink, FTGOR_ChainCurve& From, FTGOR_ChainCurve& To);
	void GetPrevCurve(FTGOR_ChainCurve& From, FTGOR_ChainCurve& To) const;
	void GetNextCurve(FTGOR_ChainCurve& From, FTGOR_ChainCurve& To) const;

	//~ Begin UObject Interface
#if WITH_EDITOR
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif // WITH_EDITOR
	//~ End UObject Interface

	//~ Begin UPrimitiveComponent Interface.
	virtual FPrimitiveSceneProxy* CreateSceneProxy() override;
	//~ End UPrimitiveComponent Interface.

	////////////////////////////////////////////////////////////////////////////////////////////////////
private:

	/** Ordered list of chain links in this branch */
	UPROPERTY()
		TArray<UTGOR_ChainActivatorComponent*> ChainCache;
};

