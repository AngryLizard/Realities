// The Gateway of Realities: Planes of Existence.

#pragma once

#include "Realities/Base/TGOR_GameInstance.h"

#include "Realities/Interfaces/TGOR_SingletonInterface.h"
#include "Components/SceneComponent.h"
#include "TGOR_SceneComponent.generated.h"

/**
* TGOR_SceneComponent provides basic resource retreiving functionality
*/
UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class REALITIES_API UTGOR_SceneComponent : public USceneComponent, public ITGOR_SingletonInterface
{
	GENERATED_BODY()

public:
	UTGOR_SceneComponent();

	////////////////////////////////////////////////////////////////////////////////////////////////////

	/** Checks whether this component has server authority */
	UFUNCTION(BlueprintCallable, Category = "TGOR System", Meta = (Keywords = "C++"))
		bool HasServerAuthority();

	/** Checks whether this component has been spawned to persistent level */
	UFUNCTION(BlueprintCallable, Category = "TGOR System", Meta = (Keywords = "C++"))
		bool WasSpawnedToPersistentLevel();

	/** Get component from owner actor */
	UFUNCTION(BlueprintCallable, Category = "TGOR Actions", Meta = (DeterminesOutputType = "ComponentClass", ExpandEnumAsExecs = "Branches", Keywords = "C++"))
		UActorComponent* GetOwnerComponent(TSubclassOf<UActorComponent> ComponentClass, ETGOR_FetchEnumeration& Branches);

	template<typename T> T* GetOwnerComponent() const
	{
		AActor* Actor = GetOwner();
		return Actor->FindComponentByClass<T>();
	}

	/** Get component from owner actor */
	UFUNCTION(BlueprintCallable, Category = "TGOR Actions", Meta = (DeterminesOutputType = "ComponentClass", ExpandEnumAsExecs = "Branches", Keywords = "C++"))
		TArray<UActorComponent*> GetOwnerComponents(TSubclassOf<UActorComponent> ComponentClass, ETGOR_FetchEnumeration& Branches);

	template<typename T> TArray<T*> GetOwnerComponents() const
	{
		AActor* Actor = GetOwner();
		TArray<T*> Components;
		Actor->GetComponents<T>(Components);
		return Components;
	}
};
