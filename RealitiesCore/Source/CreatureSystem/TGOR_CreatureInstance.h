// The Gateway of Realities: Planes of Existence.

#pragma once

#include "CoreMinimal.h"
#include "Coresystem/Storage/TGOR_Serialisation.h"
#include "CoreSystem/Storage/TGOR_PackageNetwork.h"
#include "CoreSystem/Storage/TGOR_PackageGroup.h"

#include "ActionSystem/TGOR_ActionInstance.h"
#include "CustomisationSystem/TGOR_BodypartInstance.h"
#include "SimulationSystem/TGOR_SimulationInstance.h"

#include "Engine/UserDefinedStruct.h"
#include "TGOR_CreatureInstance.generated.h"

class UTGOR_Palette;
class UTGOR_Creature;


/**
* TGOR_CreatureSetupInstance stores character gameplay information
*/
USTRUCT(BlueprintType)
struct CREATURESYSTEM_API FTGOR_CreatureSetupInstance
{
	GENERATED_USTRUCT_BODY()
		SERIALISE_INIT_HEADER();

	FTGOR_CreatureSetupInstance();

	CTGOR_GroupPackageCache LegacyCache;
	void Write(FTGOR_GroupWritePackage& Package, UTGOR_Singleton* Context) const;
	void Read(FTGOR_GroupReadPackage& Package, UTGOR_Singleton* Context);
	void Send(FTGOR_NetworkWritePackage& Package, UTGOR_Context* Context) const;
	void Recv(FTGOR_NetworkReadPackage& Package, UTGOR_Context* Context);

	////////////////////////////////////////////////////////////////////////////////////////////////////
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
		FTGOR_AppearanceInstance Appearance;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
		FTGOR_LoadoutInstance Actions;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
		FTGOR_SimulationInstance Simulations;

};

template<>
struct TStructOpsTypeTraits<FTGOR_CreatureSetupInstance> : public TStructOpsTypeTraitsBase2<FTGOR_CreatureSetupInstance>
{
	enum
	{
		WithNetSerializer = true
	};
};