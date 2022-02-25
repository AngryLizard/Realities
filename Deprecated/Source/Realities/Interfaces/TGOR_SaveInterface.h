// The Gateway of Realities: Planes of Existence.

#pragma once

#include "CoreMinimal.h"
#include "Realities/Utility/Storage/TGOR_PackageGroup.h"
#include "Realities/Utility/Storage/TGOR_PackageNetwork.h"

#include "UObject/Interface.h"
#include "TGOR_SaveInterface.generated.h"

class UTGOR_Singleton;

UINTERFACE(MinimalAPI)
class UTGOR_SaveInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class REALITIES_API ITGOR_SaveInterface
{
	GENERATED_BODY()

public:
	CTGOR_GroupPackageCache LegacyCache;

	/** Write to a buffer */
	UFUNCTION(BlueprintNativeEvent, Category = "TGOR Game", Meta = (Keywords = "C++"))
		void Write(UPARAM(ref) FTGOR_GroupWritePackage& Package, UTGOR_Singleton* Context) const;
		virtual void Write_Implementation(UPARAM(ref) FTGOR_GroupWritePackage& Package, UTGOR_Singleton* Context) const;

	/** Read a buffer */
	UFUNCTION(BlueprintNativeEvent, Category = "TGOR Game", Meta = (Keywords = "C++"))
		bool Read(UPARAM(ref) FTGOR_GroupReadPackage& Package, UTGOR_Singleton* Context);
		virtual bool Read_Implementation(UPARAM(ref) FTGOR_GroupReadPackage& Package, UTGOR_Singleton* Context);

};

UINTERFACE(MinimalAPI)
class UTGOR_NetInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 *
 */
class REALITIES_API ITGOR_NetInterface
{
	GENERATED_BODY()

public:
	/** Write to a buffer */
	UFUNCTION(BlueprintNativeEvent, Category = "TGOR Game", Meta = (Keywords = "C++"))
		void Send(UPARAM(ref) FTGOR_NetworkWritePackage& Package, UTGOR_Singleton* Context) const;
	virtual void Send_Implementation(UPARAM(ref) FTGOR_NetworkWritePackage& Package, UTGOR_Singleton* Context) const;

	/** Read a buffer */
	UFUNCTION(BlueprintNativeEvent, Category = "TGOR Game", Meta = (Keywords = "C++"))
		bool Recv(UPARAM(ref) FTGOR_NetworkReadPackage& Package, UTGOR_Singleton* Context);
	virtual bool Recv_Implementation(UPARAM(ref) FTGOR_NetworkReadPackage& Package, UTGOR_Singleton* Context);

};
