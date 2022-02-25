#pragma once

#include "CoreMinimal.h"
#include "Realities/Base/Instances/Creature/TGOR_MovementInstance.h"

#include "Realities/Base/Inventory/TGOR_Storage.h"
#include "TGOR_SocketStorage.generated.h"

class UTGOR_Socket;
class UTGOR_SocketComponent;

/**
 * The abstract base class for all socket storages.
 */
UCLASS(Abstract, Blueprintable)
class REALITIES_API UTGOR_SocketStorage : public UTGOR_Storage
{
	GENERATED_BODY()

	friend struct FTGOR_SocketInstance; 
	friend class UTGOR_Socket;

public:
	
	UTGOR_SocketStorage() {}
	virtual void Clear() override;
	virtual FString Print() const override;
	virtual void Tick(UTGOR_DimensionComponent* Owner, float DeltaSeconds) override;
	virtual bool Compatible(const UTGOR_Container* Other) const override;

	virtual void Write_Implementation(FTGOR_GroupWritePackage& Package, UTGOR_Singleton* Context) const override;
	virtual bool Read_Implementation(FTGOR_GroupReadPackage& Package, UTGOR_Singleton* Context) override;
	virtual void Send_Implementation(FTGOR_NetworkWritePackage& Package, UTGOR_Singleton* Context) const override;
	virtual bool Recv_Implementation(FTGOR_NetworkReadPackage& Package, UTGOR_Singleton* Context) override;

	virtual bool Equals(const UTGOR_Storage* Other) const override;
	virtual UTGOR_Content* GetOwner() const override;

	////////////////////////////////////////////////////////////////////////////////////////////////////

	/** Return the content socket that is stored inside this storage. */
	UFUNCTION(BlueprintPure, Category = "TGOR Storage", Meta = (Keywords = "C++"))
		virtual UTGOR_Socket* GetSocket() const;

	/** Whether this storage is parented to the given movement. */
	UFUNCTION(BlueprintPure, Category = "TGOR Storage", Meta = (Keywords = "C++"))
		bool HasParent(UTGOR_MobilityComponent* Parent) const;

	/** Get current position of this socket. */
	UFUNCTION(BlueprintCallable, Category = "TGOR Storage", Meta = (Keywords = "C++"))
		FTGOR_MovementPosition ComputePosition(UTGOR_MobilityComponent* Owner) const;

	/** Get current space of this socket. */
	UFUNCTION(BlueprintCallable, Category = "TGOR Storage", Meta = (Keywords = "C++"))
		FTGOR_MovementSpace ComputeSpace(UTGOR_MobilityComponent* Owner) const;

	/** Prepares this socket, i.e. attach owner to parent socket. */
	UFUNCTION(BlueprintCallable, Category = "TGOR Storage", Meta = (Keywords = "C++"))
		void Prepare(UTGOR_MobilityComponent* Owner);

	////////////////////////////////////////////////////////////////////////////////////////////////////
protected:

	/** The Socket of this storage. */
	UPROPERTY(VisibleAnywhere, Category = "TGOR Storage")
		UTGOR_Socket* SocketRef;
};
