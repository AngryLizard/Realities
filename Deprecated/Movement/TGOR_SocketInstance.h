#pragma once

#include "CoreMinimal.h"
#include "Realities/Utility/Storage/TGOR_PackageGroup.h"
#include "Realities/Utility/Storage/TGOR_PackageNetwork.h"

#include "Realities/Utility/Storage/TGOR_Serialisation.h"
#include "TGOR_SocketInstance.generated.h"

class UTGOR_Module;
class UTGOR_Socket;
class UTGOR_SocketStorage;
class UTGOR_DimensionComponent;

/**
 * The instance of a socket. Contains the serializable data to make the socket individual.
 */
USTRUCT(BlueprintType, Blueprintable)
struct REALITIES_API FTGOR_SocketInstance
{
	GENERATED_BODY()
	SERIALISE_INIT_HEADER;

public:

	FTGOR_SocketInstance();
	FTGOR_SocketInstance(UTGOR_Socket* NewSocket);
	FTGOR_SocketInstance(UTGOR_SocketStorage* NewStorage);

	CTGOR_GroupPackageCache LegacyCache;
	void Write(FTGOR_GroupWritePackage& Package, UTGOR_Singleton* Context) const;
	void Read(FTGOR_GroupReadPackage& Package, UTGOR_Singleton* Context);
	void Send(FTGOR_NetworkWritePackage& Package, UTGOR_Singleton* Context) const;
	void Recv(FTGOR_NetworkReadPackage& Package, UTGOR_Singleton* Context);

	bool operator<(const FTGOR_SocketInstance& Other) const; // Compares held item storage index (for weak ordering)
	bool operator==(const FTGOR_SocketInstance& Other) const; // Compares storage pointer and serial
	bool Update(UTGOR_DimensionComponent* Owner, float DeltaSeconds); // Updates storage (tick and serial increment for replication), return true if something changed
	void Post(); // Post-process storage, done after update and side-effects are handled

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, DisplayName = "SocketStorage", Category = "Inventory")
		UTGOR_SocketStorage* Storage;

	/** Serial to promote replication. TODO: Replace with custom NetDelta implementation */
	UPROPERTY()
		uint16 Serial;

	/** Instantiate storage from given content. */
	void SetupStorage(UTGOR_Socket* NewSocket);
};

template<>
struct TStructOpsTypeTraits<FTGOR_SocketInstance> : public TStructOpsTypeTraitsBase2<FTGOR_SocketInstance>
{
	enum
	{
		WithNetSerializer = true
	};
};
