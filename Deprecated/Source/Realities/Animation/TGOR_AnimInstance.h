// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "Realities/Interfaces/TGOR_SingletonInterface.h"
#include "Animation/AnimInstanceProxy.h"
#include "Animation/AnimInstance.h"
#include "TGOR_AnimInstance.generated.h"

class UTGOR_SubAnimInstance;
class UTGOR_Animation;
class UAnimInstance;


USTRUCT()
struct REALITIES_API FTGOR_AnimInstanceProxy : public FAnimInstanceProxy
{
	GENERATED_BODY()

	FTGOR_AnimInstanceProxy();
	FTGOR_AnimInstanceProxy(UAnimInstance* Instance);

	void PrintSyncGroups(const TArray<FName>& SyncGroups) const;
	bool HasSyncGroupInstance(const FName& SyncGroup) const;
	const FAnimGroupInstance& GetSyncGroupInstance(const FName& SyncGroup) const;

public:
};



/**
 * 
 */
USTRUCT(BlueprintType, Blueprintable)
struct REALITIES_API FTGOR_SubAnimationInstance
{
	GENERATED_BODY()

public:
	FTGOR_SubAnimationInstance();

	UPROPERTY()
		bool IsSwitched;

	UPROPERTY()
		UTGOR_SubAnimInstance* Previous;
	
	UPROPERTY()
		UTGOR_SubAnimInstance* Current;

	UPROPERTY()
		TSubclassOf<UTGOR_SubAnimInstance> DefaultOn;

	UPROPERTY()
		TSubclassOf<UTGOR_SubAnimInstance> DefaultOff;
};

/**
 *
 */
USTRUCT(BlueprintType, Blueprintable)
struct REALITIES_API FTGOR_SubAnimBlend
{
	GENERATED_BODY()

public:
	FTGOR_SubAnimBlend();

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		bool IsSwitched;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float OnBlend;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float OffBlend;
};


UCLASS(Transient, Blueprintable)
class REALITIES_API UTGOR_AnimInstance : public UAnimInstance, public ITGOR_SingletonInterface
{
	GENERATED_UCLASS_BODY()

public:
	UTGOR_AnimInstance();
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;
	
protected:

	UPROPERTY(BlueprintReadOnly)
		TMap<FName, FTGOR_SubAnimationInstance> AnimationInstanceQueues;

public:

	UFUNCTION(BlueprintPure, Category = "TGOR Animation", Meta = (Keywords = "C++"))
		bool HasQueue(FName AnimationContentQueue) const;

	UFUNCTION(BlueprintPure, Category = "TGOR Animation", Meta = (Keywords = "C++"))
		UTGOR_Animation* GetQueue(FName AnimationContentQueue) const;

	UFUNCTION(BlueprintPure, Category = "TGOR Animation", Meta = (Keywords = "C++"))
		FTGOR_SubAnimBlend GetBlendInfo(FName AnimationContentQueue) const;

	UFUNCTION(BlueprintPure, Category = "TGOR Animation", Meta = (Keywords = "C++"))
		UTGOR_SubAnimInstance* GetSubAnimInstance(FName AnimationContentQueue) const;

	UFUNCTION(BlueprintPure, Category = "TGOR Animation", Meta = (Keywords = "C++"))
		FName GetSubAnimName(FName AnimationContentQueue) const;
	
	UFUNCTION(BlueprintCallable, Category = "TGOR Animation", Meta = (Keywords = "C++"))
		void AssignAnimationInstance(FName AnimationContentQueue, UTGOR_Animation* Animation);
	
	/** StopSlotAnimation but actually working */
	UFUNCTION(BlueprintCallable, Category = "TGOR Animation", Meta = (Keywords = "C++"))
		void StopMontageBySlot(float InBlendOutTime, FName SlotNodeName);

	/** Get montage from name */
	UFUNCTION(BlueprintCallable, Category = "TGOR Animation", Meta = (Keywords = "C++"))
		UAnimMontage* GetMontageBySlot(FName SlotNodeName);

	const FTGOR_AnimInstanceProxy& GetInstanceProxy() const
	{
		return GetProxyOnAnyThread<FTGOR_AnimInstanceProxy>();
	}

protected:
	FTGOR_AnimInstanceProxy& GetInstanceProxy()
	{
		return GetProxyOnAnyThread<FTGOR_AnimInstanceProxy>();
	}

	virtual FAnimInstanceProxy* CreateAnimInstanceProxy() override
	{
		return new FTGOR_AnimInstanceProxy(this);
	}

	/*
	virtual void DestroyAnimInstanceProxy(FAnimInstanceProxy* InProxy) override
	{
	}
	*/

	friend struct FTGOR_AnimInstanceProxy;
	friend struct FTGOR_SubAnimInstanceProxy;
	friend struct FTGOR_AnimNode_LinkedAnimGraph;
};
