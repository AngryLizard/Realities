// The Gateway of Realities: Planes of Existence.

#pragma once

#include "CoreMinimal.h"
#include "Realities/Base/Instances/System/TGOR_HandleInstance.h"

#include "Realities/Components/Movement/TGOR_MobilityComponent.h"
#include "Realities/Interfaces/TGOR_RegistrarInterface.h"
#include "TGOR_SocketComponent.generated.h"

class UTGOR_NamedSocket;

USTRUCT(BlueprintType)
struct FTGOR_NamedAttachment
{
	GENERATED_USTRUCT_BODY()
		SERIALISE_INIT_HEADER;

	void Write(FTGOR_GroupWritePackage& Package, UTGOR_Singleton* Context) const;
	void Read(FTGOR_GroupReadPackage& Package, UTGOR_Singleton* Context);
	void Send(FTGOR_NetworkWritePackage& Package, UTGOR_Singleton* Context) const;
	void Recv(FTGOR_NetworkReadPackage& Package, UTGOR_Singleton* Context);

	/** Attached component */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TWeakObjectPtr<UTGOR_PilotComponent> Component;

	/** Socket this is parented to */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		UTGOR_NamedSocket* Socket;

	/** Visual radius */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float Radius;

	/** Visual blend */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float Blend;
};

template<>
struct TStructOpsTypeTraits<FTGOR_NamedAttachment> : public TStructOpsTypeTraitsBase2<FTGOR_NamedAttachment>
{
	enum
	{
		WithNetSerializer = true
	};
};

USTRUCT(BlueprintType)
struct FTGOR_NamedAttachments
{
	GENERATED_USTRUCT_BODY()
		SERIALISE_INIT_HEADER;

	void Write(FTGOR_GroupWritePackage& Package, UTGOR_Singleton* Context) const;
	void Read(FTGOR_GroupReadPackage& Package, UTGOR_Singleton* Context);
	void Send(FTGOR_NetworkWritePackage& Package, UTGOR_Singleton* Context) const;
	void Recv(FTGOR_NetworkReadPackage& Package, UTGOR_Singleton* Context);

	UPROPERTY(BlueprintReadOnly, Category = "TGOR Pipeline")
		TMap<UTGOR_Socket*, FTGOR_NamedAttachment> Parts;
};

template<>
struct TStructOpsTypeTraits<FTGOR_NamedAttachments> : public TStructOpsTypeTraitsBase2<FTGOR_NamedAttachments>
{
	enum
	{
		WithNetSerializer = true
	};
};

///////////////////////////////////////////// DELEGATES ///////////////////////////////////////////////////

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FNamedSocketDelegate, UTGOR_NamedSocket*, Socket);

/**
 * TGOR_SocketComponent allows parenting to sockets of attached primitives.
 */
UCLASS(ClassGroup = (Custom), Blueprintable, meta = (BlueprintSpawnableComponent))
class REALITIES_API UTGOR_SocketComponent : public UTGOR_MobilityComponent
{
	GENERATED_BODY()
	
public:
	UTGOR_SocketComponent();

	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	virtual FTGOR_MovementDynamic GetIndexTransform(UTGOR_Socket* Socket, int32 Index) const override;

	virtual bool IsSocketOccupied(UTGOR_Socket* Socket) const override;
	virtual bool QuerySocketObjects(UTGOR_PilotComponent* Component, FTGOR_MovementVisuals& Visuals) const override;
	virtual TArray<UTGOR_PilotComponent*> GetSocketObjects(TSubclassOf<UTGOR_Socket> Type) const override;

	//////////////////////////////////////////// IMPLEMENTABLES ////////////////////////////////////////

	UPROPERTY(BlueprintAssignable, Category = "TGOR Movement")
		FNamedSocketDelegate OnSocketChanged;

	////////////////////////////////////////////////////////////////////////////////////////////////////
protected:

	/** Attachments to this component */
	UPROPERTY(ReplicatedUsing = RepNotifyAttachments, BlueprintReadOnly, Category = "TGOR Movement")
		FTGOR_NamedAttachments Attachments;

	UFUNCTION()
		void RepNotifyAttachments();

	////////////////////////////////////////////////////////////////////////////////////////////////////
public:

	/** Current transform of given socket in world space */
	UFUNCTION(BlueprintPure, Category = "TGOR Movement", Meta = (Keywords = "C++"))
		FTGOR_MovementPosition GetTransformOfSocket(UTGOR_NamedSocket* Socket) const;

	/** Finds whether a shape can be attached to this creature at a given socket */
	UFUNCTION(BlueprintPure, Category = "TGOR Movement", Meta = (Keywords = "C++"))
		bool CanFitToSocket(const FTGOR_MovementShape& Shape, UTGOR_NamedSocket* Socket) const;

	/** Finds whether an object can be attached to this creature at a given socket */
	UFUNCTION(BlueprintPure, Category = "TGOR Movement", Meta = (Keywords = "C++"))
		bool CanAttachToSocket(UTGOR_PilotComponent* Attachee, UTGOR_NamedSocket* Socket) const;

	/** Finds whether an object is currently attached to this creature at a given socket */
	UFUNCTION(BlueprintPure, Category = "TGOR Movement", Meta = (Keywords = "C++"))
		bool IsAttachedToSocket(UTGOR_PilotComponent* Attachee, UTGOR_NamedSocket* Socket) const;

	/** Gets socket a given object is attached to or null if it is not attached to any socket */
	UFUNCTION(BlueprintPure, Category = "TGOR Movement", Meta = (Keywords = "C++"))
		UTGOR_NamedSocket* GetAttachedSocket(UTGOR_PilotComponent* Attachee) const;

	/** Gets currently to a socket attached mobility */
	UFUNCTION(BlueprintPure, Category = "TGOR Movement", Meta = (Keywords = "C++"))
		UTGOR_PilotComponent* GetAttached(UTGOR_NamedSocket* Socket) const;

	/** Gets accumulate shape of all bodies attach to a socket */
	UFUNCTION(BlueprintPure, Category = "TGOR Movement", Meta = (Keywords = "C++"))
		FTGOR_MovementShape GetDisplacedShape(UTGOR_NamedSocket* Socket) const;

	/** Set radius and blend of socket */
	UFUNCTION(BlueprintCallable, Category = "TGOR Movement", Meta = (Keywords = "C++"))
		void SetSocketVisuals(UTGOR_NamedSocket* Socket, float Radius, float Blend);

	////////////////////////////////////////////////////////////////////////////////////////////////////
public:

	/** Attaches an object to this creature if it can */
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "TGOR Movement", Meta = (ExpandEnumAsExecs = "Branches", Keywords = "C++"))
		void AttachToSocket(UTGOR_PilotComponent* Attachee, UTGOR_NamedSocket* Socket, ETGOR_ComputeEnumeration& Branches);

	/** Detaches an object from this creature */
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "TGOR Movement", Meta = (Keywords = "C++"))
		void DetachFromSocket(UTGOR_PilotComponent* Attachee);

	/** Detaches an object from this creature at given index */
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "TGOR Movement", Meta = (Keywords = "C++"))
		void DetachFromSocketAt(UTGOR_NamedSocket* Socket);

};
