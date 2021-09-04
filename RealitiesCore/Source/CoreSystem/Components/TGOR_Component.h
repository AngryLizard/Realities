// The Gateway of Realities: Planes of Existence.

#pragma once

#include "CoreMinimal.h"

#include "CoreSystem/Interfaces/TGOR_SingletonInterface.h"
#include "Components/ActorComponent.h"
#include "TGOR_Component.generated.h"

/**
* TGOR_Component provides basic resource retreiving functionality
*/
UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class CORESYSTEM_API UTGOR_Component : public UActorComponent, public ITGOR_SingletonInterface
{
	GENERATED_BODY()

public:
	UTGOR_Component(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	////////////////////////////////////////////////////////////////////////////////////////////////////
	
	/** Checks whether this component has server authority */
	UFUNCTION(BlueprintPure, Category = "!TGOR System", Meta = (Keywords = "C++"))
		bool HasServerAuthority() const;

	/** Checks whether this component has been spawned to persistent level */
	UFUNCTION(BlueprintPure, Category = "!TGOR System", Meta = (Keywords = "C++"))
		bool WasSpawnedToPersistentLevel() const;

	/** Get component from owner actor */
	UFUNCTION(BlueprintCallable, Category = "!TGOR System", Meta = (DeterminesOutputType = "ComponentClass", ExpandEnumAsExecs = "Branches", Keywords = "C++"))
		UActorComponent* GetOwnerComponent(TSubclassOf<UActorComponent> ComponentClass, ETGOR_FetchEnumeration& Branches);

	template<typename T> T* GetOwnerComponent() const
	{
		AActor* Actor = GetOwner();
		return Actor->FindComponentByClass<T>();
	}

	/** Get component from owner actor */
	UFUNCTION(BlueprintCallable, Category = "!TGOR System", Meta = (DeterminesOutputType = "ComponentClass", ExpandEnumAsExecs = "Branches", Keywords = "C++"))
		TArray<UActorComponent*> GetOwnerComponents(TSubclassOf<UActorComponent> ComponentClass, ETGOR_FetchEnumeration& Branches);

	template<typename T> TArray<T*> GetOwnerComponents() const
	{
		AActor* Actor = GetOwner();
		TArray<T*> Components;
		Actor->GetComponents<T>(Components);
		return Components;
	}

	/** Get responsible scene of owning actor with given type. Priority depends on attachment hierarchy (parent-first) */
	UFUNCTION(BlueprintCallable, Category = "!TGOR System", Meta = (DeterminesOutputType = "ComponentClass", ExpandEnumAsExecs = "Branches", Keywords = "C++"))
		USceneComponent* GetOwnerRootScene(TSubclassOf<USceneComponent> ComponentClass, ETGOR_FetchEnumeration& Branches) const;
	template<typename T> T* GetOwnerRootScene() const
	{
		ETGOR_FetchEnumeration State;
		return Cast<T>(GetOwnerRootScene(T::StaticClass(), State));
	}
};
