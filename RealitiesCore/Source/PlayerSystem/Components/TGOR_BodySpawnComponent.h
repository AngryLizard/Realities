// The Gateway of Realities: Planes of Existence.

#pragma once

#include "CoreMinimal.h"
#include "CreatureSystem/TGOR_CreatureInstance.h"

#include "CoreSystem/Interfaces/TGOR_RegisterInterface.h"
#include "DimensionSystem/Interfaces/TGOR_SpawnerInterface.h"
#include "AttributeSystem/Interfaces/TGOR_AttributeInterface.h"
#include "DimensionSystem/Components/TGOR_ConnectionComponent.h"
#include "TGOR_BodySpawnComponent.generated.h"

////////////////////////////////////////////// DECL //////////////////////////////////////////////////////

class ATGOR_OnlineController;

////////////////////////////////////////////// STRUCT //////////////////////////////////////////////////////

USTRUCT(BlueprintType)
struct PLAYERSYSTEM_API FTGOR_CreateBodySetup
{
	GENERATED_BODY()
		SERIALISE_INIT_HEADER();
	FTGOR_CreateBodySetup();

	void Send(FTGOR_NetworkWritePackage& Package, UTGOR_Singleton* Context) const;
	void Recv(FTGOR_NetworkReadPackage& Package, UTGOR_Singleton* Context);

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		UTGOR_Creature* Creature;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FTGOR_CreatureSetupInstance Setup;
};

template<>
struct TStructOpsTypeTraits<FTGOR_CreateBodySetup> : public TStructOpsTypeTraitsBase2<FTGOR_CreateBodySetup>
{
	enum
	{
		WithNetSerializer = true
	};
};

////////////////////////////////////////////// DELEGATES //////////////////////////////////////////////////////

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FBodySpawnDelegate, APawn*, Pawn);

/**
* TGOR_BodySpawnComponent handles spawning of creature pawns in line with the dimension and userbody system
*/
UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PLAYERSYSTEM_API UTGOR_BodySpawnComponent : public UTGOR_ConnectionComponent, public ITGOR_SpawnerInterface
{
	GENERATED_BODY()

public:	
	UTGOR_BodySpawnComponent();

	virtual void UpdateContent_Implementation(FTGOR_SpawnerDependencies& Dependencies) override;
	virtual TMap<int32, UTGOR_SpawnModule*> GetModuleType_Implementation() const override;

	////////////////////////////////////////////////////////////////////////////////////////////////////
public:

	/** Request creation of a new body, specify whether owning player should switch to the new body automatically */
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "!TGOR Pawn", Meta = (Keywords = "C++"))
		int32 RequestNewBody(ATGOR_OnlineController* OwnerController, FTGOR_CreateBodySetup Setup, bool AutoSwitch);

	/** Notifies spawner about body having been spawned */
	UPROPERTY(BlueprintAssignable, Category = "!TGOR Dimension")
		FBodySpawnDelegate OnBodySpawned;

	////////////////////////////////////////////////////////////////////////////////////////////////////
protected:

	UFUNCTION(NetMulticast, Reliable)
		void NotifyBodySpawned(APawn* Pawn);
};