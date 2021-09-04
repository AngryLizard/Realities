// The Gateway of Realities: Planes of Existence.

#pragma once

#include "CoreMinimal.h"
#include "Engine/Canvas.h"
#include "Engine/TextureRenderTarget2D.h"
#include "Kismet/KismetRenderingLibrary.h"

#include "CoreSystem/Storage/TGOR_Serialisation.h"
#include "CoreSystem/Storage/TGOR_PackageGroup.h"
#include "CoreSystem/Storage/TGOR_PackageNetwork.h"

#include "Engine/UserDefinedStruct.h"
#include "TGOR_SimulationInstance.generated.h"


/**
*
*/
USTRUCT(BlueprintType)
struct SIMULATIONSYSTEM_API FTGOR_SimulationInstance
{
	GENERATED_USTRUCT_BODY();
	SERIALISE_INIT_HEADER;

	FTGOR_SimulationInstance();

	CTGOR_GroupPackageCache LegacyCache;
	void Write(FTGOR_GroupWritePackage& Package, UTGOR_Singleton* Context) const;
	void Read(FTGOR_GroupReadPackage& Package, UTGOR_Singleton* Context);
	void Send(FTGOR_NetworkWritePackage& Package, UTGOR_Context* Context) const;
	void Recv(FTGOR_NetworkReadPackage& Package, UTGOR_Context* Context);

	////////////////////////////////////////////////////////////////////////////////////////////////////

	////////////////////////////////////////////////////////////////////////////////////////////////////
};

template<>
struct TStructOpsTypeTraits<FTGOR_SimulationInstance> : public TStructOpsTypeTraitsBase2<FTGOR_SimulationInstance>
{
	enum
	{
		WithNetSerializer = true
	};
};