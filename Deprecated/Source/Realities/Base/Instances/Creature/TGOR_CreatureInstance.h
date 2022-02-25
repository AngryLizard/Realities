// The Gateway of Realities: Planes of Existence.

#pragma once

#include "Realities/Utility/Error/TGOR_Error.h"
#include "Realities/Utility/TGOR_CustomEnumerations.h"
#include "Realities/Utility/Storage/TGOR_Serialisation.h"
#include "Realities/Base/Instances/Creature/TGOR_BodypartInstance.h"
#include "Realities/Utility/Storage/TGOR_PackageGroup.h"

#include "Engine/UserDefinedStruct.h"
#include "TGOR_CreatureInstance.generated.h"

class UTGOR_Colour;
class UTGOR_Creature;

/**
* TGOR_CreatureAppearanceInstance stores character appearance information
*/
USTRUCT(BlueprintType)
struct FTGOR_CreatureAppearanceInstance
{
	GENERATED_USTRUCT_BODY()

	FTGOR_CreatureAppearanceInstance();

	CTGOR_GroupPackageCache LegacyCache;
	void Write(FTGOR_GroupWritePackage& Package, UTGOR_Singleton* Context) const;
	void Read(FTGOR_GroupReadPackage& Package, UTGOR_Singleton* Context);

	////////////////////////////////////////////////////////////////////////////////////////////////////
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "TGOR Bodypart")
		TArray<FTGOR_BodypartInstance> Bodyparts;

	////////////////////////////////////////////////////////////////////////////////////////////////////

	bool NetSerialize(FArchive& Ar, class UPackageMap* Map, bool& bOutSuccess);
};

template<>
struct TStructOpsTypeTraits<FTGOR_CreatureAppearanceInstance> : public TStructOpsTypeTraitsBase2<FTGOR_CreatureAppearanceInstance>
{
	enum
	{
		WithNetSerializer = true
	};
};



/**
* TGOR_CreatureSetupInstance stores character gameplay information
*/
USTRUCT(BlueprintType)
struct FTGOR_CreatureSetupInstance
{
	GENERATED_USTRUCT_BODY()

	FTGOR_CreatureSetupInstance();

	CTGOR_GroupPackageCache LegacyCache;
	void Write(FTGOR_GroupWritePackage& Package, UTGOR_Singleton* Context) const;
	void Read(FTGOR_GroupReadPackage& Package, UTGOR_Singleton* Context);

	////////////////////////////////////////////////////////////////////////////////////////////////////
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
		FTGOR_CreatureAppearanceInstance Appearance;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
		UTGOR_Creature* Creature;

	bool NetSerialize(FArchive& Ar, class UPackageMap* Map, bool& bOutSuccess);
};

template<>
struct TStructOpsTypeTraits<FTGOR_CreatureSetupInstance> : public TStructOpsTypeTraitsBase2<FTGOR_CreatureSetupInstance>
{
	enum
	{
		WithNetSerializer = true
	};
};