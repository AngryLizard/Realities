// The Gateway of Realities: Planes of Existence.

#pragma once

#include "CoreMinimal.h"

#include "Realities/Utility/Error/TGOR_Error.h"
#include "Realities/Utility/Structs/TGOR_AutoThreshold.h"

#include "CoreMinimal.h"
#include "Realities/Components/Movement/TGOR_MobilityComponent.h"
#include "TGOR_SocketComponent.generated.h"

class UTGOR_StaticSocket;
class UTGOR_PilotComponent;

USTRUCT(BlueprintType)
struct FTGOR_StaticAttachment
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

	/** Target component */
	//UPROPERTY(EditAnywhere, BlueprintReadWrite)
	//	TWeakObjectPtr<UPrimitiveComponent> Target;

	/** Socket this attachment got added from */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		UTGOR_StaticSocket* Socket;
};

template<>
struct TStructOpsTypeTraits<FTGOR_StaticAttachment> : public TStructOpsTypeTraitsBase2<FTGOR_StaticAttachment>
{
	enum
	{
		WithNetSerializer = true
	};
};

USTRUCT(BlueprintType)
struct FTGOR_StaticAttachments
{
	GENERATED_USTRUCT_BODY()
		SERIALISE_INIT_HEADER;
	
	void Write(FTGOR_GroupWritePackage& Package, UTGOR_Singleton* Context) const;
	void Read(FTGOR_GroupReadPackage& Package, UTGOR_Singleton* Context);
	void Send(FTGOR_NetworkWritePackage& Package, UTGOR_Singleton* Context) const;
	void Recv(FTGOR_NetworkReadPackage& Package, UTGOR_Singleton* Context);

	UPROPERTY(BlueprintReadOnly, Category = "TGOR Pipeline")
		TMap<FTGOR_Serial, FTGOR_StaticAttachment> Parts;
};

template<>
struct TStructOpsTypeTraits<FTGOR_StaticAttachments> : public TStructOpsTypeTraitsBase2<FTGOR_StaticAttachments>
{
	enum
	{
		WithNetSerializer = true
	};
};


/**
* TGOR_SocketComponent provides static sockets for things to connect to
*/
UCLASS(ClassGroup = (Custom), Blueprintable, meta = (BlueprintSpawnableComponent))
class REALITIES_API UTGOR_SocketComponent : public UTGOR_MobilityComponent
{
	GENERATED_BODY()
	
public:
	UTGOR_SocketComponent();
	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction) override;
	virtual void GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const override;

	virtual USceneComponent* GetMovementBasis() const override;
	virtual bool IsValidIndex(int32 Index) const override;
	virtual FTGOR_MovementDynamic GetIndexTransform(int32 Index) const override;

	virtual TSet<UTGOR_Content*> GetActiveContent_Implementation() const override;
	virtual bool QuerySocketObjects(UTGOR_PilotComponent* Component, FTGOR_MovementVisuals& Visuals) const override;
	virtual TArray<UTGOR_PilotComponent*> GetSocketObjects(TSubclassOf<UTGOR_Socket> Type) const override;

	////////////////////////////////////////////////////////////////////////////////////////////////////
public:

	/** Finds whether an object can be attached to this pipeline at a given spline point */
	UFUNCTION(BlueprintPure, Category = "TGOR Pipeline", Meta = (Keywords = "C++"))
		bool CanAttachToStatic(UTGOR_PilotComponent* Attachee, UTGOR_StaticSocket* Socket) const;

	/** Attaches an object to this pipeline if it can */
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "TGOR Pipeline", Meta = (ExpandEnumAsExecs = "Branches", Keywords = "C++"))
		void AttachToStatic(UTGOR_PilotComponent* Attachee, UTGOR_StaticSocket* Socket, ETGOR_ComputeEnumeration& Branches);

	/** Detaches an object from this pipeline */
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "TGOR Pipeline", Meta = (Keywords = "C++"))
		void DetachFromStatic(UTGOR_PilotComponent* Attachee);

	/** Detaches an object from this pipeline at given index */
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "TGOR Pipeline", Meta = (Keywords = "C++"))
		void DetachFromStaticAt(FTGOR_Serial Index);

	////////////////////////////////////////////////////////////////////////////////////////////////////
protected:

	/** Attachments to this pipeline */
	UPROPERTY(ReplicatedUsing = RepNotifyAttachments, BlueprintReadOnly, Category = "TGOR Pipeline")
		FTGOR_StaticAttachments Attachments;

	UFUNCTION()
		void RepNotifyAttachments();

	/** Array of automatically loaded socket classes */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TGOR Target")
		TArray<TSubclassOf<UTGOR_StaticSocket>> SocketClasses;
};

