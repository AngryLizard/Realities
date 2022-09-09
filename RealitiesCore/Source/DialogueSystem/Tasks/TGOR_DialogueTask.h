// TGOR (C) // CHECKED //
#pragma once

#include "CoreMinimal.h"
#include "Net/UnrealNetwork.h"

#include "DialogueSystem/TGOR_DialogueInstance.h"

#include "AnimationSystem/Tasks/TGOR_AnimatedTask.h"
#include "TGOR_DialogueTask.generated.h"

////////////////////////////////////////////// DECL //////////////////////////////////////////////////////

class UTGOR_Dialogue;
class UTGOR_DialogueComponent;

/**
*
*/
USTRUCT(BlueprintType)
struct DIALOGUESYSTEM_API FTGOR_DialogueTaskIdentifier
{
	GENERATED_USTRUCT_BODY();

	/** Dialogue content */
	UPROPERTY(BlueprintReadOnly, Category = "!TGOR Dialogue")
		UTGOR_Dialogue* Content = nullptr;

	/** Owning movement component */
	UPROPERTY(BlueprintReadOnly, Category = "!TGOR Dialogue")
		UTGOR_DialogueComponent* Component = nullptr;

	/** Slot this movement resides in */
	UPROPERTY(BlueprintReadOnly, Category = "!TGOR Dialogue")
		int32 Slot = -1;
};

/**
* 
*/
UCLASS(Abstract, Blueprintable)
class DIALOGUESYSTEM_API UTGOR_DialogueTask : public UTGOR_AnimatedTask
{
	GENERATED_BODY()

public:
	
	////////////////////////////////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////// ENGINE ////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////////////////////////////

	UTGOR_DialogueTask();
	void GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const override;
	virtual TScriptInterface<ITGOR_AnimationInterface> GetAnimationOwner() const override;

	////////////////////////////////////////////////////////////////////////////////////////////////////
public:

	/** Gets assigned movement content */
	UFUNCTION(BlueprintPure, Category = "!TGOR Dialogue", Meta = (Keywords = "C++"))
		UTGOR_Dialogue* GetDialogue() const;

	/** Gets assigned movement component */
	UFUNCTION(BlueprintPure, Category = "!TGOR Dialogue", Meta = (Keywords = "C++"))
		UTGOR_DialogueComponent* GetOwnerComponent() const;

	/** Gets assigned movement slot index */
	UFUNCTION(BlueprintPure, Category = "!TGOR Dialogue", Meta = (Keywords = "C++"))
		int32 GetSlotIndex() const;

	/** Get attribute for movement */
	UFUNCTION(BlueprintPure, Category = "!TGOR Movement", Meta = (Keywords = "C++"))
		float GetAttribute(TSubclassOf<UTGOR_Attribute> Type, float Default) const;

	////////////////////////////////////////////////////////////////////////////////////////////////////
public:

	/** Called once when this task is created */
	virtual void Initialise();

	/** Called once movement is invoked */
	virtual void PrepareStart();

	/** Called once movement is stopped */
	virtual void Interrupt();

	////////////////////////////////////////////////////////////////////////////////////////////////////

	/** Called once when this task is assigned to a component. Should usually only happen once. */
	UFUNCTION(BlueprintImplementableEvent, Category = "!TGOR Dialogue", Meta = (Keywords = "C++"))
		void OnInitialise();

	////////////////////////////////////////////////////////////////////////////////////////////////////
public:

	/** Owning movement component */
	UPROPERTY(ReplicatedUsing = RepNotifyIdentifier, Transient, BlueprintReadOnly, Category = "!TGOR Dialogue")
		FTGOR_DialogueTaskIdentifier Identifier;

	/** Initialise parenting */
	UFUNCTION()
		void RepNotifyIdentifier() { Initialise(); }

};
