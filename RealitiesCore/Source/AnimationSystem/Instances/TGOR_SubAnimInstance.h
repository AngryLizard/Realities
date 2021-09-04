// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "TGOR_AnimInstance.h"

#include "Animation/AnimInstanceProxy.h"
#include "TGOR_SubAnimInstance.generated.h"

class UTGOR_Animation;
class UTGOR_Performance;

USTRUCT()
struct ANIMATIONSYSTEM_API FTGOR_SubAnimInstanceProxy : public FTGOR_AnimInstanceProxy
{
	GENERATED_BODY()

	FTGOR_SubAnimInstanceProxy();
	FTGOR_SubAnimInstanceProxy(UAnimInstance* Instance);

	void MergeSyncGroupsFrom(const FTGOR_AnimInstanceProxy& Parent, const TArray<FName>& SyncGroups);

	virtual void PreUpdate(UAnimInstance* InAnimInstance, float DeltaSeconds) override;

protected:
	void UpdateSubAnimation();

	friend struct FTGOR_AnimNode_AnimationContentInstance;
};

UCLASS(Transient, Blueprintable)
class ANIMATIONSYSTEM_API UTGOR_SubAnimInstance : public UTGOR_AnimInstance
{
	GENERATED_UCLASS_BODY()

public:
	UTGOR_SubAnimInstance();

	UPROPERTY(BlueprintReadOnly)
		UTGOR_Animation* Animation;

	UPROPERTY(BlueprintReadOnly)
		UTGOR_AnimInstance* ParentInstance;

	UPROPERTY(BlueprintReadOnly)
		UTGOR_Performance* ParentSlot;

	/** Called when this instance got added to a parent */
	UFUNCTION(BlueprintImplementableEvent, Category = "!TGOR Animation", Meta = (Keywords = "C++"))
		void OnAddedToParent();

	/** Called when this instance got removed from a parent */
	UFUNCTION(BlueprintImplementableEvent, Category = "!TGOR Animation", Meta = (Keywords = "C++"))
		void OnRemovedFromParent();


	/** Which syncgroups are inherited from parent */
	UPROPERTY(EditAnywhere)
		TArray<FName> SyncGroupInheritance;

	const FTGOR_SubAnimInstanceProxy& GetSubProxyOnAnyThread() const
	{
		return GetProxyOnAnyThread<FTGOR_SubAnimInstanceProxy>();
	}

protected:
	FTGOR_SubAnimInstanceProxy& GetSubProxyOnAnyThread()
	{
		return GetProxyOnAnyThread<FTGOR_SubAnimInstanceProxy>();
	}

private:
	virtual FAnimInstanceProxy* CreateAnimInstanceProxy() override
	{
		return new FTGOR_SubAnimInstanceProxy(this);
	}

	/*
	virtual void DestroyAnimInstanceProxy(FAnimInstanceProxy* InProxy) override
	{
	}
	*/

	friend struct FTGOR_SubAnimInstanceProxy;
	friend struct FTGOR_AnimNode_LinkedAnimGraph;
	friend struct FTGOR_AnimNode_AnimationContentInstance;
};
