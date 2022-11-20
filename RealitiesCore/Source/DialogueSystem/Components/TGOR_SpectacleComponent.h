// The Gateway of Realities: Planes of Existence.

#pragma once

#include "CoreMinimal.h"
#include "DialogueSystem/TGOR_DialogueInstance.h"
#include "CoreSystem/Utility/TGOR_CoreEnumerations.h"

#include "TargetSystem/Components/TGOR_AimTargetComponent.h"
#include "AnimationSystem/Interfaces/TGOR_AnimationInterface.h"
#include "TGOR_SpectacleComponent.generated.h"

////////////////////////////////////////////// DECL //////////////////////////////////////////////////////

class UTGOR_Performance;
class UTGOR_InstigatorComponent;

/**
 * UTGOR_SpectacleComponent allows running an operation including particiants
 */
UCLASS(ClassGroup = (Custom))
class DIALOGUESYSTEM_API UTGOR_SpectacleComponent : public UTGOR_AimTargetComponent,
	public ITGOR_AnimationInterface
{
	GENERATED_BODY()

public:
	UTGOR_SpectacleComponent();
	virtual void UpdateContent_Implementation(FTGOR_SpawnerDependencies& Dependencies) override;

	virtual TSubclassOf<UTGOR_Performance> GetPerformanceType() const override;
	virtual UTGOR_AnimationComponent* GetAnimationComponent() const override;

	//////////////////////////////////////////// IMPLEMENTABLES ////////////////////////////////////////

	////////////////////////////////////////////////////////////////////////////////////////////////////
public:

	/** Check whether any spectacle can be started for a given instigator */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Dialogue", Meta = (Keywords = "C++"))
		virtual bool ValidateSpectactleFor(UTGOR_InstigatorComponent* Instigator, FTGOR_SpectacleConfiguration& Configuration) const;

	/** Start first available spectacle */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Dialogue", Meta = (Keywords = "C++"))
		virtual void StartSpectacle(UTGOR_InstigatorComponent* Instigator, FTGOR_SpectacleConfiguration& Configuration);

	/** End currently running spectacle */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Dialogue", Meta = (Keywords = "C++"))
		virtual void EndSpectacle();

	////////////////////////////////////////////////////////////////////////////////////////////////////
public:

	/** Animation performance type */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "!TGOR Action")
		TSubclassOf<UTGOR_Performance> PerformanceType;

	////////////////////////////////////////////////////////////////////////////////////////////////////
public:

	/** Gets participant component for first matching participant type */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Dialogue", Meta = (DeterminesOutputType = "ComponentClass", ExpandEnumAsExecs = "Branches", Keywords = "C++"))
		UActorComponent* GetParticipantComponentByType(TSubclassOf<UTGOR_Participant> Participant, TSubclassOf<UActorComponent> ComponentClass, ETGOR_FetchEnumeration& Branches);

	/** Gets participant component for participant */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Dialogue", Meta = (DeterminesOutputType = "ComponentClass", ExpandEnumAsExecs = "Branches", Keywords = "C++"))
		UActorComponent* GetParticipantComponent(UTGOR_Participant* Participant, TSubclassOf<UActorComponent> ComponentClass, ETGOR_FetchEnumeration& Branches);
		UActorComponent* GetParticipantComponent(UTGOR_Participant* Participant) const;

	////////////////////////////////////////////////////////////////////////////////////////////////////
protected:

	/** Participant component cache */
	UPROPERTY(Transient)
		TMap<UTGOR_Participant*, UActorComponent*> ParticipantCache;
};

