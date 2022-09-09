// The Gateway of Realities: Planes of Existence.

#pragma once

#include "CoreMinimal.h"

#include "DialogueSystem/TGOR_DialogueInstance.h"
#include "CoreSystem/Interfaces/TGOR_RegisterInterface.h"
#include "AttributeSystem/Interfaces/TGOR_AttributeInterface.h"
#include "AnimationSystem/Interfaces/TGOR_AnimationInterface.h"
#include "TargetSystem/Components/TGOR_AimTargetComponent.h"
#include "TGOR_DialogueComponent.generated.h"

class UTGOR_Dialogue;
class UTGOR_DialogueTask;
class UTGOR_Performance;

/**
 * UTGOR_DialogueComponent allows running a dialogue node
 */
UCLASS(ClassGroup = (Custom))
class DIALOGUESYSTEM_API UTGOR_DialogueComponent : public UTGOR_AimTargetComponent, public ITGOR_AnimationInterface, public ITGOR_RegisterInterface, public ITGOR_AttributeInterface
{
	GENERATED_BODY()

public:
	UTGOR_DialogueComponent();

	virtual TSet<UTGOR_CoreContent*> GetActiveContent_Implementation() const override;

	virtual void UpdateContent_Implementation(FTGOR_SpawnerDependencies& Dependencies) override;
	virtual TMap<int32, UTGOR_SpawnModule*> GetModuleType_Implementation() const override;

	virtual TSubclassOf<UTGOR_Performance> GetPerformanceType() const override;
	virtual UTGOR_AnimationComponent* GetAnimationComponent() const override;

	//////////////////////////////////////////// IMPLEMENTABLES ////////////////////////////////////////

	////////////////////////////////////////////////////////////////////////////////////////////////////
public:

	/** Dialogues types this component spawns with. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "!TGOR Dialogue")
		TArray<TSubclassOf<UTGOR_Dialogue>> SpawnDialogues;

	////////////////////////////////////////////////////////////////////////////////////////////////////
protected:

	/** Animation performance type */
	UPROPERTY(EditAnywhere, Category = "!TGOR Dialogue")
		TSubclassOf<UTGOR_Performance> PerformanceType;

	////////////////////////////////////////////////////////////////////////////////////////////////////
public:

	/** Get attached Dialogue task */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Dialogue", Meta = (Keywords = "C++"))
		UTGOR_DialogueTask* GetDialogueTask() const;

	/** Check whether a given dialogue type is active */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Dialogue", Meta = (Keywords = "C++"))
		bool HasDialogueWith(int32 Identifier) const;

	/** Start given dialogue */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Dialogue", Meta = (Keywords = "C++"))
		void StartDialogueWith(int32 Identifier);

	/** Get attached Dialogue task */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Dialogue", Meta = (DeterminesOutputType = "Type", Keywords = "C++"))
		UTGOR_DialogueTask* GetCurrentDialogueOfType(TSubclassOf<UTGOR_DialogueTask> Type) const;

	template<typename T> T* GetCurrentMOfType() const
	{
		return Cast<T>(GetCurrentDialogueOfType(T::StaticClass()));
	}

	/** Get all Dialogue tasks of given type */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Dialogue", Meta = (DeterminesOutputType = "Type", Keywords = "C++"))
		TArray<UTGOR_DialogueTask*> GetDialogueListOfType(TSubclassOf<UTGOR_DialogueTask> Type) const;

	template<typename T> TArray<T*> GetMListOfType() const
	{
		TArray<T*> Output;
		TArray<UTGOR_DialogueTask*> Dialogues = GetDialogueListOfType(T::StaticClass());
		for (UTGOR_DialogueTask* Dialogue : Dialogues)
		{
			Output.Emplace(Cast<T>(Dialogue));
		}
		return Output;
	}

	////////////////////////////////////////////////////////////////////////////////////////////////////
protected:

	/** Currently assigned containers. */
	UPROPERTY(Replicated)
		TArray<UTGOR_DialogueTask*> DialogueSlots;

	/** Currently scheduled Dialogue */
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = RepNotifyDialogueTaskState)
		FTGOR_DialogueState DialogueTaskState;

	/** Update Dialogue on replication */
	UFUNCTION()
		void RepNotifyDialogueTaskState(const FTGOR_DialogueState& Old);
};

