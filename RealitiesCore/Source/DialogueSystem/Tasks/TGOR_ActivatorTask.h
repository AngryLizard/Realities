// TGOR (C) // CHECKED //
#pragma once

#include "CoreMinimal.h"
#include "Net/UnrealNetwork.h"

#include "DialogueSystem/TGOR_DialogueInstance.h"

#include "DialogueSystem/Tasks/TGOR_SpectacleTask.h"
#include "TGOR_ActivatorTask.generated.h"

////////////////////////////////////////////// DECL //////////////////////////////////////////////////////

class UTGOR_Activator;
class UTGOR_Participant;
class UTGOR_ActivatorComponent;
class UTGOR_ParticipantComponent;
class UTGOR_InstigatorComponent;

/**
*
*/
USTRUCT(BlueprintType)
struct DIALOGUESYSTEM_API FTGOR_ActivatorTaskIdentifier
{
	GENERATED_USTRUCT_BODY();

	/** Activator content */
	UPROPERTY(BlueprintReadOnly, Category = "!TGOR Activator")
		UTGOR_Activator* Content = nullptr;

	/** Owning movement component */
	UPROPERTY(BlueprintReadOnly, Category = "!TGOR Activator")
		UTGOR_ActivatorComponent* Component = nullptr;

	/** Slot this movement resides in */
	UPROPERTY(BlueprintReadOnly, Category = "!TGOR Activator")
		int32 Slot = -1;
};

/**
* 
*/
UCLASS(Abstract, Blueprintable)
class DIALOGUESYSTEM_API UTGOR_ActivatorTask : public UTGOR_SpectacleTask
{
	GENERATED_BODY()

public:
	
	////////////////////////////////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////// ENGINE ////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////////////////////////////

	UTGOR_ActivatorTask();
	void GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const override;
	virtual TScriptInterface<ITGOR_AnimationInterface> GetAnimationOwner() const override;
	
	virtual bool Initialise() override;
	virtual void Prepare(FTGOR_SpectacleState& DialogueState) override;
	virtual bool Update(FTGOR_SpectacleState& DialogueState, float DeltaTime) override;
	virtual void Interrupt(FTGOR_SpectacleState& DialogueState) override;

	////////////////////////////////////////////////////////////////////////////////////////////////////
public:

	/** Gets assigned movement content */
	UFUNCTION(BlueprintPure, Category = "!TGOR Activator", Meta = (Keywords = "C++"))
		UTGOR_Activator* GetActivator() const;

	/** Gets assigned movement component */
	UFUNCTION(BlueprintPure, Category = "!TGOR Activator", Meta = (Keywords = "C++"))
		UTGOR_ActivatorComponent* GetOwnerComponent() const;

	/** Gets assigned movement slot index */
	UFUNCTION(BlueprintPure, Category = "!TGOR Activator", Meta = (Keywords = "C++"))
		int32 GetSlotIndex() const;

	/** Gets current progress value */
	UFUNCTION(BlueprintPure, Category = "!TGOR Activator", Meta = (Keywords = "C++"))
		virtual float GetProgress() const;

	////////////////////////////////////////////////////////////////////////////////////////////////////
public:

	UFUNCTION(BlueprintCallable, Category = "!TGOR Activator", Meta = (Keywords = "C++"))
		void SetDomain(UTGOR_Participant* Participant, const FVector& Local);

	UFUNCTION(BlueprintPure, Category = "!TGOR Activator", Meta = (Keywords = "C++"))
		FVector GetDomain(TSubclassOf<UTGOR_Participant> Participant) const;

	////////////////////////////////////////////////////////////////////////////////////////////////////
protected:

	/** Called once when activator starts, used for setup. */
	UFUNCTION(BlueprintImplementableEvent, Category = "!TGOR Dialogue", Meta = (Keywords = "C++"))
		void OnPrepare();

	/** Called every tick while activator is active. */
	UFUNCTION(BlueprintImplementableEvent, Category = "!TGOR Dialogue", Meta = (Keywords = "C++"))
		void OnUpdate(float DeltaTime, ETGOR_SpectacleStateEnumeration& Branches, float& Progress);

	/** Called when activator progress is bigger than or equal to 1.0 */
	UFUNCTION(BlueprintImplementableEvent, Category = "!TGOR Dialogue", Meta = (Keywords = "C++"))
		void OnCompleted();

	/** Always called when activator is stopped/descheduled, used for cleanup. */
	UFUNCTION(BlueprintImplementableEvent, Category = "!TGOR Dialogue", Meta = (Keywords = "C++"))
		void OnInterrupt();

	////////////////////////////////////////////////////////////////////////////////////////////////////
public:

	/** Owning movement component */
	UPROPERTY(ReplicatedUsing = RepNotifyIdentifier, Transient, BlueprintReadOnly, Category = "!TGOR Activator")
		FTGOR_ActivatorTaskIdentifier Identifier;

	/** Initialise parenting */
	UFUNCTION()
		void RepNotifyIdentifier() { Initialise(); }

	////////////////////////////////////////////////////////////////////////////////////////////////////
private:

	/** Current progress value. */
	UPROPERTY()
		float CurrentProgress = 0.0f;

	/** Current domain values for each participant. */
	UPROPERTY(Transient)
		TMap<UTGOR_Participant*, FVector> DomainValues;
};
