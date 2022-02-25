// The Gateway of Realities: Planes of Existence.

#pragma once

#include "CoreMinimal.h"
#include "Realities/Utility/Error/TGOR_Error.h"
#include "Realities/Utility/Storage/TGOR_PackageGroup.h"
#include "Realities/Utility/Storage/TGOR_PackageNetwork.h"
#include "Realities/Utility/Storage/TGOR_Serialisation.h"

#include "Engine/UserDefinedStruct.h"
#include "TGOR_Normal.generated.h"

/**
* A floating point value between 0 and 1
*/
USTRUCT(BlueprintType)
struct REALITIES_API FTGOR_Normal
{
	GENERATED_USTRUCT_BODY()
	SERIALISE_INIT_HEADER;
	FTGOR_Normal();
	FTGOR_Normal(float V);
	bool operator==(const FTGOR_Normal& Normal) const;

	void Write(FTGOR_GroupWritePackage& Package, UTGOR_Singleton* Context) const;
	void Read(FTGOR_GroupReadPackage& Package, UTGOR_Singleton* Context);
	void Send(FTGOR_NetworkWritePackage& Package, UTGOR_Singleton* Context) const;
	void Recv(FTGOR_NetworkReadPackage& Package, UTGOR_Singleton* Context);

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float Value;
};

template<> struct TStructOpsTypeTraits<FTGOR_Normal> : public TStructOpsTypeTraitsBase2<FTGOR_Normal>
{
	enum { WithNetSerializer = true };
};