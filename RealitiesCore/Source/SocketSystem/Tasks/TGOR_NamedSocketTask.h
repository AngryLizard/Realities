// TGOR (C) // CHECKED //
#pragma once

#include "CoreMinimal.h"
#include "RealitiesUtility/Structures/TGOR_Normal.h"

#include "TGOR_SocketPilotTask.h"
#include "TGOR_NamedSocketTask.generated.h"

////////////////////////////////////////////// DECL //////////////////////////////////////////////////////

class UTGOR_SocketComponent;
class UTGOR_NamedSocket;


/**
*
*/
USTRUCT(BlueprintType)
struct SOCKETSYSTEM_API FTGOR_NamedSocketParenting
{
	GENERATED_USTRUCT_BODY();
	FTGOR_NamedSocketParenting();
	bool operator==(const FTGOR_NamedSocketParenting& Other) const;
	bool operator!=(const FTGOR_NamedSocketParenting& Other) const;

	/** Socket type */
	UPROPERTY()
		UTGOR_NamedSocket* Socket;

	/** Parent component */
	UPROPERTY()
		TWeakObjectPtr<UTGOR_SocketComponent> Component;
};

/**
* 
*/
UCLASS(Blueprintable)
class SOCKETSYSTEM_API UTGOR_NamedSocketTask : public UTGOR_SocketPilotTask
{
	GENERATED_BODY()

	friend class UTGOR_SocketComponent;

public:
	
	////////////////////////////////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////// ENGINE ////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////////////////////////////

	UTGOR_NamedSocketTask();
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	////////////////////////////////////////////////////////////////////////////////////////////////////
public:

	virtual UTGOR_MobilityComponent* GetParent() const override;
	virtual FTGOR_MovementPosition ComputePosition() const override;
	virtual FTGOR_MovementSpace ComputeSpace() const override;
	virtual void Update(float DeltaTime) override;
	virtual void Unparent() override;

	virtual void Register() override;
	virtual void Unregister() override;

	virtual UTGOR_Socket* GetSocket() const override;

	////////////////////////////////////////////////////////////////////////////////////////////////////

	/** Look for suitable parent primitive to attach to */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Storage", Meta = (Keywords = "C++"))
		void RecachePrimitive();

	////////////////////////////////////////////////////////////////////////////////////////////////////
protected:

	/** Parent this task to a given socket */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Movement", Meta = (Keywords = "C++"))
		void Parent(UTGOR_SocketComponent* Parent, UTGOR_NamedSocket* Socket);

	/** Whether this task can parent to a given socket */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Movement", Meta = (Keywords = "C++"))
		bool CanParent(const UTGOR_SocketComponent* Parent, UTGOR_NamedSocket* Socket) const;

	////////////////////////////////////////////////////////////////////////////////////////////////////
private:

	/** Parenting data */
	UPROPERTY(ReplicatedUsing = RepNotifyParenting)
		FTGOR_NamedSocketParenting Parenting;

	/** Properly call reparent */
	UFUNCTION()
		void RepNotifyParenting(const FTGOR_NamedSocketParenting& Old)
	{
		FTGOR_NamedSocketParenting New = Parenting;
		Parenting = Old;
		Parent(New.Component.Get(), New.Socket);
	}

	/** Parent primitive cache */
	UPROPERTY()
		TWeakObjectPtr<UPrimitiveComponent> Cache;

	/** Visual radius */
	UPROPERTY()
		float Radius;

	/** Socket blend (below 0.0 drops this socket) */
	UPROPERTY()
		FTGOR_Normal Blend;

};
