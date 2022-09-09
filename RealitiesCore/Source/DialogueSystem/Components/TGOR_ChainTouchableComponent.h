// The Gateway of Realities: Planes of Existence.

#pragma once

#include "CoreMinimal.h"

#include "TGOR_TouchableComponent.h"
#include "TGOR_ChainTouchableComponent.generated.h"

USTRUCT(BlueprintType)
struct FTGOR_ChainCurve
{
	GENERATED_USTRUCT_BODY();

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "!TGOR Interaction")
		FTransform Transform;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "!TGOR Interaction")
		float Tangent;
};

/**
 * UTGOR_ChainTouchableComponent
 */
UCLASS(ClassGroup = (Custom), Blueprintable, meta = (BlueprintSpawnableComponent))
class DIALOGUESYSTEM_API UTGOR_ChainTouchableComponent : public UTGOR_TouchableComponent
{
	GENERATED_BODY()

public:
	UTGOR_ChainTouchableComponent();
	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	virtual FTransform GetTargetTransform() const override;

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

	/** How full this chain link is */
	UPROPERTY(BlueprintReadOnly, Category = "!TGOR Interaction")
		float Fillment;

	////////////////////////////////////////////////////////////////////////////////////////////////////
public:

	/** Get current world transform of the curve state*/
	UFUNCTION(BlueprintPure, Category = "!TGOR Interaction", Meta = (Keywords = "C++"))
		FTransform GetCurrentCurveTransform() const;

	UTGOR_ChainTouchableComponent* FindChainLink(const FName& Name) const;
	static void GetCurve(const UTGOR_ChainTouchableComponent* PrevLink, const UTGOR_ChainTouchableComponent* NextLink, FTGOR_ChainCurve& From, FTGOR_ChainCurve& To);
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
		TArray<UTGOR_ChainTouchableComponent*> ChainCache;

	/** Current chain position of interaction */
	UPROPERTY()
		float ChainPosition = 0.f;

	/** Current chain velocity of interaction */
	UPROPERTY()
		float ChainVelocity = 0.f;

	/** Currently accumulated applied force */
	UPROPERTY()
		float ChainForce = 0.f;


	UPROPERTY()
		UTGOR_ChainTouchableComponent* CurrentFromLink = nullptr;

	UPROPERTY()
		UTGOR_ChainTouchableComponent* CurrentToLink = nullptr;

	UPROPERTY()
		float CurrentCurveTime = 0.f;
};

