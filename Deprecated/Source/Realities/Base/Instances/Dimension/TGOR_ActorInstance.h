// The Gateway of Realities: Planes of Existence.

#pragma once

#include "Realities/Utility/Error/TGOR_Error.h"
#include "Realities/Utility/Storage/TGOR_Serialisation.h"
#include "Realities/Base/Instances/Dimension/TGOR_ElementInstance.h"
#include "Realities/Utility/Storage/TGOR_PackageGroup.h"
#include "Realities/Utility/Storage/TGOR_PackageNetwork.h"

#include "Engine/UserDefinedStruct.h"
#include "TGOR_ActorInstance.generated.h"

class UTGOR_RegionComponent;
class UTGOR_IdentityComponent;
class UTGOR_Spawner;
class UTGOR_Dimension;
class UTGOR_Singleton;

UENUM(BlueprintType)
enum class ETGOR_ActorStorageMode : uint8
{
	Map,
	Spawn
};

/**
* TGOR_ActorState stores region components (in regions registered actors)
 */
USTRUCT(BlueprintType)
struct REALITIES_API FTGOR_ActorState
{
	GENERATED_USTRUCT_BODY()
	FTGOR_ActorState();
	FTGOR_ActorState(UTGOR_RegionComponent* Source);

	////////////////////////////////////////////////////////////////////////////////////////////////////

	/** Component this struct belongs to */
	UPROPERTY(BlueprintReadOnly)
		UTGOR_RegionComponent* Component;

	/** Location cache of Component for improved cache locality, updated every tick */
	UPROPERTY(BlueprintReadOnly)
		FVector2D Cache;
};


/**
* TGOR_SpawnInstance allows spawning actors
*/
USTRUCT(BlueprintType)
struct REALITIES_API FTGOR_SpawnInstance
{
	GENERATED_USTRUCT_BODY()
		FTGOR_SpawnInstance();

	CTGOR_GroupPackageCache LegacyCache;
	void Write(FTGOR_GroupWritePackage& Package, UTGOR_Singleton* Context) const;
	void Read(FTGOR_GroupReadPackage& Package, UTGOR_Singleton* Context);

	////////////////////////////////////////////////////////////////////////////////////////////////////
	
	/** Spawn type */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		UTGOR_Spawner* SpawnActor;

	/** Location this actor was spawned with */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FVector SpawnLocation;

	/** Rotation this actor was spawn with */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FRotator SpawnRotation;

	/** Associated identity */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		UTGOR_IdentityComponent* Identity;

};


/**
*
*/
USTRUCT(BlueprintType)
struct REALITIES_API FTGOR_SpawnIdentity
{
	GENERATED_USTRUCT_BODY()
	SERIALISE_INIT_HEADER;

	FTGOR_SpawnIdentity();

	void Send(FTGOR_NetworkWritePackage& Package, UTGOR_Singleton* Context) const;
	void Recv(FTGOR_NetworkReadPackage& Package, UTGOR_Singleton* Context);

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		UTGOR_Spawner* Spawner;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		UTGOR_Dimension* Dimension;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		int32 Identifier;
};

template<>
struct TStructOpsTypeTraits<FTGOR_SpawnIdentity> : public TStructOpsTypeTraitsBase2<FTGOR_SpawnIdentity>
{
	enum
	{
		WithNetSerializer = true
	};
};