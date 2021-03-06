// The Gateway of Realities: Planes of Existence.

#pragma once

#include "Realities/Base/System/Data/TGOR_WorldData.h"
#include "Realities/Base/Instances/Dimension/TGOR_ActorInstance.h"

#include "Realities/Interfaces/TGOR_DimensionInterface.h"
#include "Realities/Interfaces/TGOR_RegisterInterface.h"
#include "Realities/Interfaces/TGOR_SaveInterface.h"
#include "Realities/Components/TGOR_Component.h"
#include "TGOR_IdentityComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FIdentityDelegate, UTGOR_Spawner*, Spawner);

/**
* TGOR_IdentityComponent gives an actor an identity inside a dimension and allows loading/storing data
*/
UCLASS(ClassGroup = (Custom), Blueprintable, meta = (BlueprintSpawnableComponent))
class REALITIES_API UTGOR_IdentityComponent : public UTGOR_Component, public ITGOR_DimensionInterface, public ITGOR_SaveInterface, public ITGOR_RegisterInterface
{
	GENERATED_BODY()

	friend class UTGOR_Loader;
	friend class UTGOR_WorldData;
	friend class UTGOR_DimensionData;
	friend class UTGOR_BlueprintFunctionLibrary;

public:	
	UTGOR_IdentityComponent();
	virtual void PostInitProperties() override;
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	void GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const override;

	virtual bool PreAssemble(UTGOR_DimensionData* Dimension) override;
	virtual bool Assemble(UTGOR_DimensionData* Dimension) override;
	virtual bool PostAssemble(UTGOR_DimensionData* Dimension) override;
	virtual TSet<UTGOR_Content*> GetActiveContent_Implementation() const override;

	virtual void Write_Implementation(FTGOR_GroupWritePackage& Package, UTGOR_Singleton* Context) const override;
	virtual bool Read_Implementation(FTGOR_GroupReadPackage& Package, UTGOR_Singleton* Context) override;

	////////////////////////////////////////////////////////////////////////////////////////////////////
public:

	UPROPERTY(BlueprintAssignable, Category = "TGOR Influence")
		FIdentityDelegate OnIdentityUpdate;

	/** Get current actor identifier */
	UFUNCTION(BlueprintPure, Category = "TGOR Dimension", Meta = (Keywords = "C++"))
		int32 GetActorIdentifier() const;

	/** Get current actor spawner */
	UFUNCTION(BlueprintPure, Category = "TGOR Dimension", Meta = (Keywords = "C++"))
		UTGOR_Spawner* GetActorSpawner() const;

	/** Set current spawner (can be done safely during PreAssembly) */
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "TGOR Dimension", Meta = (Keywords = "C++"))
		void SetActorSpawner(UTGOR_Spawner* Spawner);

	/** Get current actor dimension */
	UFUNCTION(BlueprintPure, Category = "TGOR Dimension", Meta = (Keywords = "C++"))
		UTGOR_Dimension* GetActorDimension() const;
	
	/** Switch whether this component is ignored during storage */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TGOR System")
		bool IgnoreStorage;

	////////////////////////////////////////////////////////////////////////////////////////////////////	
protected:
	
	/** Actor identification in current dimension */
	UPROPERTY(ReplicatedUsing = OnIdentityRepNotify)
		FTGOR_SpawnIdentity Identity;

	UFUNCTION()
		void OnIdentityRepNotify();


	/** Associated Dimension objects */
	UPROPERTY(BlueprintReadOnly, Category = "TGOR Dimension")
		TArray<TScriptInterface<ITGOR_DimensionInterface>> DimensionObjects;

	/** Associated Save objects */
	UPROPERTY(BlueprintReadOnly, Category = "TGOR System")
		TMap<FString, TScriptInterface<ITGOR_SaveInterface>> SaveObjects;

};
