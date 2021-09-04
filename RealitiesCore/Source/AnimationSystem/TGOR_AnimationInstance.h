// The Gateway of Realities: Planes of Existence.

#pragma once

#include "CoreSystem/Storage/TGOR_Serialisation.h"
#include "CoreSystem/Storage/TGOR_PackageGroup.h"
#include "CoreSystem/Storage/TGOR_PackageNetwork.h"

#include "RealitiesUtility/Structures/TGOR_Normal.h"
#include "TGOR_AnimationInstance.generated.h"

class UTGOR_Archetype;
class UTGOR_RigParam;

/**
* TGOR_AnimationInstance stores animation customisation information
*/
USTRUCT(BlueprintType)
struct FTGOR_AnimationInstance
{
	GENERATED_USTRUCT_BODY()
		SERIALISE_INIT_HEADER;

	FTGOR_AnimationInstance();

	CTGOR_GroupPackageCache LegacyCache;
	void Write(FTGOR_GroupWritePackage& Package, UTGOR_Singleton* Context) const;
	void Read(FTGOR_GroupReadPackage& Package, UTGOR_Singleton* Context);
	void Send(FTGOR_NetworkWritePackage& Package, UTGOR_Context* Context) const;
	void Recv(FTGOR_NetworkReadPackage& Package, UTGOR_Context* Context);

	////////////////////////////////////////////////////////////////////////////////////////////////////

	/** Currently linked archetype */
	UPROPERTY(BlueprintReadOnly, Category = "!TGOR Animation")
		UTGOR_Archetype* Archetype;

	/** Current rig parameters */
	UPROPERTY(BlueprintReadOnly, Category = "!TGOR Animation")
		TMap<UTGOR_RigParam*, FTGOR_Normal> Params;
};

template<>
struct TStructOpsTypeTraits<FTGOR_AnimationInstance> : public TStructOpsTypeTraitsBase2<FTGOR_AnimationInstance>
{
	enum
	{
		WithNetSerializer = true
	};
};