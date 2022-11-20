// The Gateway of Realities: Planes of Existence.

#pragma once

#include "CoreMinimal.h"
#include "DialogueSystem/TGOR_DialogueInstance.h"

#include "CoreSystem/Interfaces/TGOR_RegisterInterface.h"
#include "DialogueSystem/Components/TGOR_SpectacleComponent.h"
#include "TGOR_ActivatorComponent.generated.h"

class UTGOR_Activator;
class UTGOR_ActivatorTask;

/**
 * UTGOR_ActivatorComponent represent components that can be interacted with by cursor
 */
UCLASS(ClassGroup = (Custom), Blueprintable, meta = (BlueprintSpawnableComponent))
class DIALOGUESYSTEM_API UTGOR_ActivatorComponent : public UTGOR_SpectacleComponent, public ITGOR_RegisterInterface
{
	GENERATED_BODY()

public:
	UTGOR_ActivatorComponent();
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

	/** Local rotation added to the Target Transform */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "!TGOR Dialogue")
		FRotator OffsetRotation = FRotator::ZeroRotator;

	/** Size of the preview rectangle */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "!TGOR Dialogue")
		float PreviewSize = 20.f;

	////////////////////////////////////////////////////////////////////////////////////////////////////
public:

	/** Activator types this component spawns with in order of priority. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "!TGOR Dialogue")
		TArray<TSubclassOf<UTGOR_Activator>> SpawnActivators;

	////////////////////////////////////////////////////////////////////////////////////////////////////
public:

	/** Get attached Activator task */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Dialogue", Meta = (Keywords = "C++"))
		UTGOR_ActivatorTask* GetActivatorTask() const;

	/** Check whether a given Activator type is active */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Dialogue", Meta = (Keywords = "C++"))
		bool HasActivatorWith(int32 Identifier) const;

	/** Get attached Activator task */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Dialogue", Meta = (DeterminesOutputType = "Type", Keywords = "C++"))
		UTGOR_ActivatorTask* GetCurrentActivatorOfType(TSubclassOf<UTGOR_ActivatorTask> Type) const;

	template<typename T> T* GetCurrentMOfType() const
	{
		return Cast<T>(GetCurrentActivatorOfType(T::StaticClass()));
	}

	/** Get all Activator tasks of given type */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Dialogue", Meta = (DeterminesOutputType = "Type", Keywords = "C++"))
		TArray<UTGOR_ActivatorTask*> GetActivatorListOfType(TSubclassOf<UTGOR_ActivatorTask> Type) const;

	template<typename T> TArray<T*> GetAListOfType() const
	{
		TArray<T*> Output;
		TArray<UTGOR_ActivatorTask*> Activators = GetActivatorListOfType(T::StaticClass());
		for (UTGOR_ActivatorTask* Activator : Activators)
		{
			Output.Emplace(Cast<T>(Activator));
		}
		return Output;
	}

	////////////////////////////////////////////////////////////////////////////////////////////////////
private:

	/** Start given Activator */
	UFUNCTION()
		void StartActivatorWith(int32 Identifier, const FTGOR_SpectacleConfiguration& Configuration);

	////////////////////////////////////////////////////////////////////////////////////////////////////
public:

	/** Update participant domain influence. */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Dialogue", Meta = (Keywords = "C++"))
		void SetParticipantDomain(UTGOR_Participant* Participant, const FVector& Local);

	////////////////////////////////////////////////////////////////////////////////////////////////////
protected:

	/** Currently assigned containers. */
	UPROPERTY(Replicated)
		TArray<UTGOR_ActivatorTask*> ActivatorSlots;

	/** Currently scheduled Dialogue */
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = RepNotifyActivatorTaskState)
		FTGOR_SpectacleState ActivatorTaskState;

	/** Update Activator on replication */
	UFUNCTION()
		void RepNotifyActivatorTaskState(const FTGOR_SpectacleState& Old);

	////////////////////////////////////////////////////////////////////////////////////////////////////
public:

	//~ Begin UPrimitiveComponent Interface.
	virtual FPrimitiveSceneProxy* CreateSceneProxy() override;
	virtual bool AreSymmetricRotations(const FQuat& A, const FQuat& B, const FVector& Scale3D) const override;
	//~ End UPrimitiveComponent Interface.
	
	class FDrawActivatorSceneProxy : public UTGOR_AimTargetComponent::FDrawAimTargetSceneProxy
	{
	public:
		FDrawActivatorSceneProxy(const UTGOR_ActivatorComponent* InComponent);

		virtual void DrawTargetSpace(const FMatrix& Transform, const FLinearColor& DrawColor, FPrimitiveDrawInterface* PDI) const override;
		void DrawRect(const FMatrix& Transform, FPrimitiveDrawInterface* PDI, const FLinearColor& ColorX, const FLinearColor& ColorY, const FVector& ScaledX, const FVector& ScaledY, const  float thickness, float border) const;

	protected:
		const float PreviewSize;
		const FVector LocalX;
		const FVector LocalY;
	};
};

