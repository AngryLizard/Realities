// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "RealitiesUtility/Structures/TGOR_Normal.h"

#include "Animation/AnimInstanceProxy.h"
#include "CoreSystem/Interfaces/TGOR_SingletonInterface.h"
#include "Animation/AnimInstance.h"
#include "TGOR_AnimInstance.generated.h"

class UTGOR_AttachComponent;
class UTGOR_HandleComponent;
class UTGOR_SubAnimInstance;
class UTGOR_AnimatedTask;
class UTGOR_Performance;
class UTGOR_RigParam;
class UTGOR_Archetype;
class UAnimInstance;
struct FAnimNode_ControlRig;

USTRUCT()
struct ANIMATIONSYSTEM_API FTGOR_AnimInstanceProxy : public FAnimInstanceProxy
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
struct ANIMATIONSYSTEM_API FTGOR_SubAnimationInstance
{
	GENERATED_BODY()

public:
	FTGOR_SubAnimationInstance();

	UPROPERTY()
		bool IsSwitched;

	UPROPERTY()
		TWeakObjectPtr<UTGOR_SubAnimInstance> Previous;

	UPROPERTY()
		TWeakObjectPtr<UTGOR_SubAnimInstance> Current;

	UPROPERTY()
		TSubclassOf<UTGOR_SubAnimInstance> DefaultOn;

	UPROPERTY()
		TSubclassOf<UTGOR_SubAnimInstance> DefaultOff;

	// Lock preventing us from switching blends multiple times per frame
	bool AnimationTaskLock = false;
};

/**
 *
 */
USTRUCT(BlueprintType, Blueprintable)
struct ANIMATIONSYSTEM_API FTGOR_SubAnimBlend
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
class ANIMATIONSYSTEM_API UTGOR_AnimInstance : public UAnimInstance, public ITGOR_SingletonInterface
{
	GENERATED_UCLASS_BODY()

		friend class UTGOR_AnimationComponent;

public:
	virtual void NativeInitializeAnimation() override;
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;
	
protected:

	UPROPERTY(BlueprintReadOnly)
		TMap<UTGOR_Performance*, FTGOR_SubAnimationInstance> AnimationInstanceQueues;

public:

	UFUNCTION()
		bool HasQueue(UTGOR_Performance* Type) const;

	UFUNCTION()
		UTGOR_AnimatedTask* GetQueue(UTGOR_Performance* Type) const;

	UFUNCTION(BlueprintPure, Category = "!TGOR Animation", Meta = (Keywords = "C++"))
		FTGOR_SubAnimBlend GetBlendInfo(TSubclassOf<UTGOR_Performance> Type) const;

	UFUNCTION()
		UTGOR_SubAnimInstance* GetSubAnimInstance(UTGOR_Performance* Performance) const;

	UFUNCTION()
		FName GetSubAnimName(UTGOR_Performance* Performance) const;
	
	UFUNCTION()
		void AddAnimationInstance(UTGOR_Performance* Performance, UTGOR_AnimatedTask* AnimatedTask);

	UFUNCTION()
		void RemoveAnimationInstance(UTGOR_Performance* Performance, UTGOR_AnimatedTask* AnimatedTask);

	UFUNCTION()
		void ClearQueues();

	UFUNCTION()
		void UpdateHandles();

	UFUNCTION()
		void AssignArchetype(UTGOR_Archetype* InArchetype, const TMap<UTGOR_RigParam*, FTGOR_Normal>& Params);
	
protected:

	/** Default blend time for undefined animation slots */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "!TGOR Animation")
		float DefaultBlendTime;

	UPROPERTY(BlueprintReadOnly)
		UTGOR_Archetype* Archetype;

	/** Called when a new archetype was defined. */
	UFUNCTION(BlueprintImplementableEvent, Category = "!TGOR Animation", Meta = (Keywords = "C++"))
		void OnSetupArchetype();


	/** StopSlotAnimation but actually working */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Animation", Meta = (Keywords = "C++"))
		void StopMontageBySlot(float InBlendOutTime, FName SlotNodeName);

	/** Get montage from name */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Animation", Meta = (Keywords = "C++"))
		UAnimMontage* GetMontageBySlot(FName SlotNodeName);

	const FTGOR_AnimInstanceProxy& GetInstanceProxy() const
	{
		return GetProxyOnAnyThread<FTGOR_AnimInstanceProxy>();
	}

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

	UPROPERTY(Transient)
		TArray<UTGOR_AttachComponent*> AttachComponents;

	UPROPERTY(Transient)
		TArray<UTGOR_HandleComponent*> HandleComponents;

private:

	// TODO: This pointer might be unsafe to keep?
	FAnimNode_ControlRig* LinkedControlRig;

	friend struct FTGOR_AnimInstanceProxy;
	friend struct FTGOR_SubAnimInstanceProxy;
	friend struct FTGOR_AnimNode_LinkedAnimGraph;
};
