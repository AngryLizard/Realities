// TGOR (C) // CHECKED //
#pragma once

#include "CoreMinimal.h"
#include "Net/UnrealNetwork.h"

#include "DialogueSystem/TGOR_DialogueInstance.h"

#include "DialogueSystem/Tasks/TGOR_SpectacleTask.h"
#include "TGOR_DialogueTask.generated.h"

////////////////////////////////////////////// DECL //////////////////////////////////////////////////////

class UTGOR_Dialogue;
class UTGOR_DialogueComponent;
class UTGOR_ActivatorComponent;
class UTGOR_InstigatorComponent;

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
class DIALOGUESYSTEM_API UTGOR_DialogueTask : public UTGOR_SpectacleTask
{
	GENERATED_BODY()

public:
	
	////////////////////////////////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////// ENGINE ////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////////////////////////////

	UTGOR_DialogueTask();
	void GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const override;
	virtual TScriptInterface<ITGOR_AnimationInterface> GetAnimationOwner() const override;

	virtual bool Initialise() override;
	virtual void Prepare(FTGOR_SpectacleState& DialogueState) override;
	virtual bool Update(FTGOR_SpectacleState& DialogueState, float DeltaTime) override;
	virtual void Interrupt(FTGOR_SpectacleState& DialogueState) override;

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
	UFUNCTION(BlueprintPure, Category = "!TGOR Dialogue", Meta = (Keywords = "C++"))
		float GetAttribute(TSubclassOf<UTGOR_Attribute> Type, float Default) const;

	////////////////////////////////////////////////////////////////////////////////////////////////////
protected:

	/** Called once when dialogue starts, used for setup. */
	UFUNCTION(BlueprintImplementableEvent, Category = "!TGOR Dialogue", Meta = (Keywords = "C++"))
		void OnPrepare();

	/** Called every tick while dialogue is active. */
	UFUNCTION(BlueprintImplementableEvent, Category = "!TGOR Dialogue", Meta = (Keywords = "C++"))
		void OnUpdate(float DeltaTime, ETGOR_SpectacleStateEnumeration& Branches);

	/** Always called when dialogue is stopped/descheduled, used for cleanup. */
	UFUNCTION(BlueprintImplementableEvent, Category = "!TGOR Dialogue", Meta = (Keywords = "C++"))
		void OnInterrupt();

	////////////////////////////////////////////////////////////////////////////////////////////////////
public:

	/** Owning movement component */
	UPROPERTY(ReplicatedUsing = RepNotifyIdentifier, Transient, BlueprintReadOnly, Category = "!TGOR Dialogue")
		FTGOR_DialogueTaskIdentifier Identifier;

	/** Initialise parenting */
	UFUNCTION()
		void RepNotifyIdentifier() { Initialise(); }

};
