// The Gateway of Realities: Planes of Existence.

#pragma once

#include "CoreMinimal.h"

#include "Realities/CustomisationSystem/TGOR_BodypartInstance.h"

#include "Realities/CoreSystem/Storage/TGOR_SaveInterface.h"
#include "Realities/DimensionSystem/Interfaces/TGOR_SpawnerInterface.h"
#include "TGOR_ChildSkeletalMeshComponent.h"
#include "TGOR_ModularSkeletalMeshComponent.generated.h"

class UTGOR_Armature;
class UTGOR_Skeleton;
class UTGOR_CoreContent;

////////////////////////////////////////////// STRUCT //////////////////////////////////////////////////////

USTRUCT(BlueprintType)
struct FTGOR_ModularMeshHandle
{
	GENERATED_USTRUCT_BODY()

	/** Register to poll for activity */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "!TGOR Bodypart")
		TScriptInterface<ITGOR_RegisterInterface> Register;

	/** Enabled bodypart by this handle */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "!TGOR Bodypart")
		TSubclassOf<UTGOR_Bodypart> Type;
};

///////////////////////////////////////////// DELEGATES ///////////////////////////////////////////////////

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FAppearanceDelegate);

/**
 * 
 */
UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class REALITIES_API UTGOR_ModularSkeletalMeshComponent : public UTGOR_ChildSkeletalMeshComponent, public ITGOR_SaveInterface, public ITGOR_SpawnerInterface
{
	GENERATED_BODY()
	
public:
	UTGOR_ModularSkeletalMeshComponent();

	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	virtual void GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const override;

	virtual bool IsNameStableForNetworking() const override;
	virtual bool IsSupportedForNetworking() const override;

	virtual void Write_Implementation(FTGOR_GroupWritePackage& Package, UTGOR_Singleton* Context) const override;
	virtual bool Read_Implementation(FTGOR_GroupReadPackage& Package, UTGOR_Singleton* Context) override;

	virtual void UpdateContent_Implementation(UTGOR_Spawner* Spawner) override;
	virtual void AttachAndInitialise() override;

	//////////////////////////////////////////// IMPLEMENTABLES ////////////////////////////////////////

	UPROPERTY(BlueprintAssignable, Category = "!TGOR Creature")
		FAppearanceDelegate OnAppearanceChanged;

	////////////////////////////////////////////////////////////////////////////////////////////////////
public:

	/** Previews an appearance to this component without actually setting the local state */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Creature", Meta = (Keywords = "C++"))
		bool PreviewAppearance(const FTGOR_AppearanceInstance& Appearance);

	/** Applies an appearance to this component */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Creature", Meta = (Keywords = "C++"))
		bool ApplyAppearance(const FTGOR_AppearanceInstance& Appearance);

	/** Applies an appearance from the current state of this component */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Creature", Meta = (Keywords = "C++"))
		bool ApplyFromCurrentAppearance();

	/** Applies default appearance to this component */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Creature", Meta = (Keywords = "C++"))
		bool DefaultAppearance();

	/** Whether this component can create child components (i.e. local or authority) */
	UFUNCTION(BlueprintPure, Category = "!TGOR Creature", Meta = (Keywords = "C++"))
		bool CanApplyAppearance();

	////////////////////////////////////////////////////////////////////////////////////////////////////

	/** Returns handle of a given type. */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Creature", Meta = (Keywords = "C++"))
		TSubclassOf<UTGOR_Bodypart> GetHandleOfType(TSubclassOf<UTGOR_Bodypart> Type) const;

	/** Register bodypart to go active. */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Creature", Meta = (Keywords = "C++"))
		void RegisterHandle(TScriptInterface<ITGOR_RegisterInterface> Register, UTGOR_CoreContent* Content, TSubclassOf<UTGOR_Bodypart> Type);

	/** Unregister manually. */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Creature", Meta = (Keywords = "C++"))
		void UnregisterHandle(UTGOR_CoreContent* Content);

	////////////////////////////////////////////////////////////////////////////////////////////////////
public:

	/** Whether master pose is not followed by all children even if enabled by bodypart */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "!TGOR Creature")
		bool IgnoreFollowsMasterPose;

	////////////////////////////////////////////////////////////////////////////////////////////////////
public:

	/** Currently linked armature */
	UPROPERTY(BlueprintReadOnly, Category = "!TGOR Creature")
		UTGOR_Skeleton* RootBodypart;

	/** Serverside appearance information */
	UPROPERTY(ReplicatedUsing = OnReplicateAppearance, BlueprintReadOnly, Category = "!TGOR Creature")
		FTGOR_AppearanceInstance CurrentAppearance;

	/** Called on each client on replication of current appearance structure. */
	UFUNCTION()
		void OnReplicateAppearance();

	////////////////////////////////////////////////////////////////////////////////////////////////////
protected:

	/** Handles for automatic bodypart loader */
	UPROPERTY(BlueprintReadOnly, Category = "!TGOR Creature")
		TMap<UTGOR_CoreContent*, FTGOR_ModularMeshHandle> Handles;

};
