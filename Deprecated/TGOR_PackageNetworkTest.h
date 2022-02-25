// The Gateway of Realities: Planes of Existence.

#pragma once

#include "CoreMinimal.h"

#include "Realities/Utility/Storage/TGOR_PackageNetwork.h"
#include "Realities/Utility/Storage/TGOR_Buffer.h"

#include "Realities/Base/TGOR_Object.h"
#include "FunctionalTesting/Classes/FunctionalTest.h"
#include "Realities/Interfaces/TGOR_SingletonInterface.h"
#include "Realities/Interfaces/TGOR_SaveInterface.h"
#include "TGOR_PackageNetworkTest.generated.h"

class UTGOR_Item;

/**
 *
 */
UENUM(BlueprintType)
enum class ETGOR_PackageNetworkTestEnum : uint8
{
	A,
	B,
	C
};

/**
 *
 */
USTRUCT(BlueprintType, Blueprintable)
struct REALITIESEXTENSION_API FTGOR_PackageNetworkTestPacket
{
	GENERATED_BODY()
	SERIALISE_INIT_HEADER;

public:

	FTGOR_PackageNetworkTestPacket();
	FTGOR_PackageNetworkTestPacket(FRandomStream& Stream);

	void Send(FTGOR_NetworkWritePackage& Package, UTGOR_Singleton* Context) const;
	void Recv(FTGOR_NetworkReadPackage& Package, UTGOR_Singleton* Context);

	/** Compare two objects and raise assertions */
	void Assert(const FTGOR_PackageNetworkTestPacket& Other, AFunctionalTest* Test) const;

	UPROPERTY()
		int32 Index;

	UPROPERTY()
		float Float;

	UPROPERTY()
		int16 Serial;

	UPROPERTY()
		uint8 Byte;
};

/**
 * Test instance to serialise
 */
USTRUCT(BlueprintType, Blueprintable)
struct REALITIESEXTENSION_API FTGOR_PackageNetworkTestInstance
{
	GENERATED_BODY()
	SERIALISE_INIT_HEADER;

public:

	FTGOR_PackageNetworkTestInstance();
	FTGOR_PackageNetworkTestInstance(FRandomStream& Stream, UTGOR_Singleton* Context);
	
	void Send(FTGOR_NetworkWritePackage& Package, UTGOR_Singleton* Context) const;
	void Recv(FTGOR_NetworkReadPackage& Package, UTGOR_Singleton* Context);

	/** Compare two objects and raise assertions */
	void Assert(const FTGOR_PackageNetworkTestInstance& Other, AFunctionalTest* Test) const;

	UPROPERTY()
		FVector Vector;

	UPROPERTY()
		FVector2D Vector2D;

	UPROPERTY()
		FRotator Rotation;

	UPROPERTY()
		FLinearColor LinearColour;

	UPROPERTY()
		FColor Colour;

	UPROPERTY()
		FString String;

	UPROPERTY()
		FName Name;

	UPROPERTY()
		ETGOR_PackageNetworkTestEnum Enum;

	UPROPERTY()
		TArray<UTGOR_Item*> ContentArray;

	UPROPERTY()
		TMap<FString, FTGOR_PackageNetworkTestPacket> PacketMap;

	UPROPERTY()
		FTGOR_Buffer Buffer;

public:
};

/**
 *
 */
UCLASS(Blueprintable)
class REALITIESEXTENSION_API UTGOR_PackageNetworkTestObject : public UTGOR_Object, public ITGOR_NetInterface
{
	GENERATED_BODY()

public:

	UTGOR_PackageNetworkTestObject();

	virtual void Send_Implementation(FTGOR_NetworkWritePackage& Package, UTGOR_Singleton* Context) const override;
	virtual bool Recv_Implementation(FTGOR_NetworkReadPackage& Package, UTGOR_Singleton* Context) override;

	/** Fill data with random data */
	UFUNCTION(BlueprintCallable, Category = "TGOR System", Meta = (Keywords = "C++"))
		void Initialise(int32 Seed, UTGOR_Singleton* Context);

	/** Compare two objects and raise assertions */
	UFUNCTION(BlueprintCallable, Category = "TGOR System", Meta = (Keywords = "C++"))
		void Assert(UTGOR_PackageNetworkTestObject* Other, AFunctionalTest* Test) const;

	UPROPERTY()
		FTGOR_PackageNetworkTestInstance Instance;
};

/**
 * 
 */
UCLASS()
class REALITIESEXTENSION_API ATGOR_PackageNetworkTest : public AFunctionalTest, public ITGOR_SingletonInterface
{
	GENERATED_BODY()

public:
	ATGOR_PackageNetworkTest();
	

	virtual void StartTest() override;
	virtual bool IsReady_Implementation() override;

	////////////////////////////////////////////////////////////////////////////////////////////////////
protected:


private:

};
