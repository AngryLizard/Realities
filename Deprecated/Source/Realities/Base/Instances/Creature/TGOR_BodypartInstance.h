// The Gateway of Realities: Planes of Existence.

#pragma once

#include "CoreMinimal.h"
#include "Realities/Utility/Storage/TGOR_Serialisation.h"
#include "Realities/Utility/Storage/TGOR_PackageGroup.h"
#include "Realities/Utility/Error/TGOR_Error.h"

#include "Engine/UserDefinedStruct.h"
#include "TGOR_BodypartInstance.generated.h"

class UTGOR_BodypartNode;
class UTGOR_Customisation;
class UTGOR_Bodypart;

/**
* 
*/
USTRUCT(BlueprintType)
struct REALITIES_API FTGOR_CustomisationInstance
{
	GENERATED_USTRUCT_BODY();

	FTGOR_CustomisationInstance();

	CTGOR_GroupPackageCache LegacyCache;
	void Write(FTGOR_GroupWritePackage& Package, UTGOR_Singleton* Context) const;
	void Read(FTGOR_GroupReadPackage& Package, UTGOR_Singleton* Context);

	////////////////////////////////////////////////////////////////////////////////////////////////////

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		UTGOR_Customisation* Content;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		uint8 Index;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TArray<uint8> Payload;

	bool NetSerialize(FArchive& Ar, class UPackageMap* Map, bool& bOutSuccess);
};

template<>
struct TStructOpsTypeTraits<FTGOR_CustomisationInstance> : public TStructOpsTypeTraitsBase2<FTGOR_CustomisationInstance>
{
	enum
	{
		WithNetSerializer = true
	};
};

/**
*
*/
USTRUCT(BlueprintType)
struct FTGOR_BodypartInstance
{
	GENERATED_USTRUCT_BODY()

	FTGOR_BodypartInstance();

	CTGOR_GroupPackageCache LegacyCache;
	void Write(FTGOR_GroupWritePackage& Package, UTGOR_Singleton* Context) const;
	void Read(FTGOR_GroupReadPackage& Package, UTGOR_Singleton* Context);

	////////////////////////////////////////////////////////////////////////////////////////////////////

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		UTGOR_Bodypart* Content;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		uint8 Depth;

	////////////////////////////////////////////////////////////////////////////////////////////////////

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TArray<FTGOR_CustomisationInstance> CustomisationInstances;

	bool NetSerialize(FArchive& Ar, class UPackageMap* Map, bool& bOutSuccess);
};

template<>
struct TStructOpsTypeTraits<FTGOR_BodypartInstance> : public TStructOpsTypeTraitsBase2<FTGOR_BodypartInstance>
{
	enum
	{
		WithNetSerializer = true
	};
};



UENUM(BlueprintType)
enum class ETGOR_BodypartChildType : uint8
{
	Optional, // Optionally loaded, changed by the user
	Required, // Required to be loaded, changed by the user
	Automatic // Optionally loaded, changed by code
};


USTRUCT(BlueprintType)
struct FTGOR_BodypartChild
{
	GENERATED_USTRUCT_BODY()

	/** Bodypart class */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "TGOR Bodypart")
		TSubclassOf<UTGOR_Bodypart> Class;

	/** Loacing behaviour of this child */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "TGOR Bodypart")
		ETGOR_BodypartChildType Type;
};