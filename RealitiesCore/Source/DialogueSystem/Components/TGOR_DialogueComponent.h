// The Gateway of Realities: Planes of Existence.

#pragma once

#include "CoreMinimal.h"
#include "DialogueSystem/TGOR_DialogueInstance.h"

#include "CoreSystem/Interfaces/TGOR_RegisterInterface.h"
#include "DialogueSystem/Components/TGOR_SpectacleComponent.h"
#include "TGOR_DialogueComponent.generated.h"

class UTGOR_Dialogue;
class UTGOR_DialogueTask;
class UTGOR_Performance;
class UTGOR_Participant;
class UTGOR_ParticipantComponent;

/**
 * UTGOR_DialogueComponent allows running a dialogue node
 */
UCLASS(ClassGroup = (Custom), Blueprintable, meta = (BlueprintSpawnableComponent))
class DIALOGUESYSTEM_API UTGOR_DialogueComponent : public UTGOR_SpectacleComponent, public ITGOR_RegisterInterface, public ITGOR_ParticipantInterface
{
	GENERATED_BODY()

public:
	UTGOR_DialogueComponent();
	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	virtual void GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const override;

	virtual TSet<UTGOR_CoreContent*> GetActiveContent_Implementation() const override;

	virtual void UpdateContent_Implementation(FTGOR_SpawnerDependencies& Dependencies) override;
	virtual TMap<int32, UTGOR_SpawnModule*> GetModuleType_Implementation() const override;

	virtual bool ValidateSpectactleFor(UTGOR_InstigatorComponent* Instigator, FTGOR_SpectacleConfiguration& Configuration) const override;
	virtual void StartSpectacle(UTGOR_InstigatorComponent* Instigator, FTGOR_SpectacleConfiguration& Configuration) override;
	virtual void EndSpectacle() override;

	//////////////////////////////////////////// IMPLEMENTABLES ////////////////////////////////////////

	////////////////////////////////////////////////////////////////////////////////////////////////////
public:

	/** Dialogues types this component spawns with in order of priority. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "!TGOR Dialogue")
		TArray<TSubclassOf<UTGOR_Dialogue>> SpawnDialogues;

	////////////////////////////////////////////////////////////////////////////////////////////////////
public:

	/** Get attached Dialogue task */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Dialogue", Meta = (Keywords = "C++"))
		UTGOR_DialogueTask* GetDialogueTask() const;

	/** Check whether a given dialogue type is active */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Dialogue", Meta = (Keywords = "C++"))
		bool HasDialogueWith(int32 Identifier) const;

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

	template<typename T> TArray<T*> GetDListOfType() const
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
private:

	/** Start given dialogue */
	UFUNCTION()
		void StartDialogueWith(int32 Identifier, const FTGOR_SpectacleConfiguration& Configuration);

	////////////////////////////////////////////////////////////////////////////////////////////////////
protected:

	/** Currently assigned containers. */
	UPROPERTY(Replicated)
		TArray<UTGOR_DialogueTask*> DialogueSlots;

	/** Currently scheduled Dialogue */
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = RepNotifyDialogueTaskState)
		FTGOR_SpectacleState DialogueTaskState;

	/** Update Dialogue on replication */
	UFUNCTION()
		void RepNotifyDialogueTaskState(const FTGOR_SpectacleState& Old);

};

