// The Gateway of Realities: Planes of Existence.

#pragma once

#include "CoreMinimal.h"

#include "Realities/Utility/TGOR_CustomEnumerations.h"
#include "Realities/Base/Instances/Creature/TGOR_CreatureInstance.h"
#include "Realities/Base/Instances/Creature/TGOR_BodypartInstance.h"

#include "Realities/Interfaces/TGOR_UnlockInterface.h"
#include "Realities/Interfaces/TGOR_RegisterInterface.h"
#include "Realities/Interfaces/TGOR_SingletonInterface.h"
#include "Components/SkeletalMeshComponent.h"
#include "TGOR_ModularSkeletalMeshComponent.generated.h"

class UTGOR_BodypartNode;

////////////////////////////////////////////// STRUCT //////////////////////////////////////////////////////

USTRUCT(BlueprintType)
struct FTGOR_ModularMeshHandle
{
	GENERATED_USTRUCT_BODY()

	/** Register to poll for activity */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "TGOR Bodypart")
		TScriptInterface<ITGOR_RegisterInterface> Register;

	/** Enabled bodypart by this handle */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "TGOR Bodypart")
		TSubclassOf<UTGOR_Bodypart> Type;
};

/**
 * 
 */
UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class REALITIES_API UTGOR_ModularSkeletalMeshComponent : public USkeletalMeshComponent, public ITGOR_SingletonInterface, public ITGOR_UnlockInterface
{
	GENERATED_BODY()
	
public:
	UTGOR_ModularSkeletalMeshComponent();

	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	virtual bool IsNameStableForNetworking() const override;
	virtual bool IsSupportedForNetworking() const override;

	virtual void UpdateUnlocks_Implementation(UTGOR_UnlockComponent* Component) override;
	virtual bool SupportsContent_Implementation(UTGOR_Content* Content) const override;


	////////////////////////////////////////////////////////////////////////////////////////////////////
protected:

	/** Allowed content types (Empty if there are no constraints) */
	UPROPERTY(BlueprintReadOnly, Category = "TGOR Bodypart")
		TSet<UTGOR_Content*> Constraints;

	////////////////////////////////////////////////////////////////////////////////////////////////////
public:

	/** Stores the current content of the AppearanceRoot into an appearance instance */
	UFUNCTION(BlueprintCallable, Category = "TGOR Creature", Meta = (Keywords = "C++"))
		FTGOR_CreatureAppearanceInstance ConstructCurrentAppearance();
	
	/** Checks whether or not Appearance has ever been initialised from its default value. */
	UFUNCTION(BlueprintPure, Category = "TGOR Creature", Meta = (Keywords = "C++"))
		bool CheckAppearance(const FTGOR_CreatureAppearanceInstance& Appearance, UTGOR_Creature* Creature);

	/** Applies an appearance to this component */
	UFUNCTION(BlueprintCallable, Category = "TGOR Creature", Meta = (Keywords = "C++"))
		bool ApplyAppearance(const FTGOR_CreatureAppearanceInstance& Appearance, UTGOR_Creature* Creature);

	////////////////////////////////////////////////////////////////////////////////////////////////////

	/** Returns handle of a given type. */
	UFUNCTION(BlueprintCallable, Category = "TGOR Creature", Meta = (Keywords = "C++"))
		TSubclassOf<UTGOR_Bodypart> GetHandleOfType(TSubclassOf<UTGOR_Bodypart> Type) const;

	/** Register bodypart to go active. */
	UFUNCTION(BlueprintCallable, Category = "TGOR Creature", Meta = (Keywords = "C++"))
		void RegisterHandle(TScriptInterface<ITGOR_RegisterInterface> Register, UTGOR_Content* Content, TSubclassOf<UTGOR_Bodypart> Type);

	/** Unregister manually. */
	UFUNCTION(BlueprintCallable, Category = "TGOR Creature", Meta = (Keywords = "C++"))
		void UnregisterHandle(UTGOR_Content* Content);

	////////////////////////////////////////////////////////////////////////////////////////////////////
protected:

	/** Handles for automatic bodypart loader */
	UPROPERTY(BlueprintReadOnly, Category = "TGOR Bodypart")
		TMap<UTGOR_Content*, FTGOR_ModularMeshHandle> Handles;

	/** Root of appearancetree */
	UPROPERTY(BlueprintReadOnly, Category = "TGOR Bodypart")
		UTGOR_BodypartNode* AppearanceRoot;

	////////////////////////////////////////////////////////////////////////////////////////////////////
public:

	/** Get appearance root */
	UFUNCTION(BlueprintCallable, Category = "TGOR Bodypart", Meta = (Keywords = "C++"))
		UTGOR_BodypartNode* GetAppearanceRoot() const;

};
