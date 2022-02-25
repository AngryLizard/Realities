// The Gateway of Realities: Planes of Existence.

#pragma once

#include "Realities/Utility/TGOR_Math.h"
#include "Realities/Utility/Error/TGOR_Error.h"
#include "Realities/Base/Instances/Creature/TGOR_MovementInstance.h"
#include "Realities/Base/Instances/Combat/TGOR_AimInstance.h"

#include "Components/BoxComponent.h"
#include "Components/SphereComponent.h"
#include "Components/CapsuleComponent.h"

#include "CoreMinimal.h"
#include "GameFramework/MovementComponent.h"
#include "Realities/Interfaces/TGOR_RegisterInterface.h"
#include "Realities/Components/TGOR_SceneComponent.h"
#include "Realities/Interfaces/TGOR_SingletonInterface.h"
#include "TGOR_MobilityComponent.generated.h"

class UTGOR_MobilityComponent;
class UTGOR_Socket;

////////////////////////////////////////////// STRUCT //////////////////////////////////////////////////////

USTRUCT(BlueprintType)
struct FTGOR_AttachmentHandle
{
	GENERATED_USTRUCT_BODY()

	/** Register to poll for activity */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TScriptInterface<ITGOR_RegisterInterface> Register;

	/** Mobility we're parented to */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TWeakObjectPtr<UTGOR_MobilityComponent> Parent;

	/** Part we're parented to */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		int32 Index;
};

/**
* TGOR_MobilityComponent handles movement states
*/
UCLASS(ClassGroup = (Custom), Blueprintable, meta = (BlueprintSpawnableComponent))
class REALITIES_API UTGOR_MobilityComponent : public UTGOR_SceneComponent, public ITGOR_RegisterInterface
{
	GENERATED_BODY()

		friend struct FTGOR_MovementRecord;

public:
	UTGOR_MobilityComponent();
	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual TSet<UTGOR_Content*> GetActiveContent_Implementation() const override;

	////////////////////////////////////////////////////////////////////////////////////////////////////
protected:

	/** Current movement base */
	UPROPERTY(ReplicatedUsing = RepNotifyBase)
		FTGOR_MovementBase Base;
	
	UFUNCTION()
		void RepNotifyBase(const FTGOR_MovementBase& Old);

	/** Current movement body */
	UPROPERTY(EditAnywhere, ReplicatedUsing = RepNotifyBody, Category = "TGOR Movement")
		FTGOR_MovementBody Body;

	UFUNCTION()
		virtual void RepNotifyBody(const FTGOR_MovementBody& Old);

	////////////////////////////////////////////////////////////////////////////////////////////////////
public:

	/** Sets body structure */
	UFUNCTION(BlueprintCallable, Category = "TGOR Movement", Meta = (Keywords = "C++"))
		void SetBody(const FTGOR_MovementBody& NewBody);

	////////////////////////////////////////////////////////////////////////////////////////////////////
protected:

	/** Adjust movement on sudden movement changes (e.g. smooth lerp) */
	UFUNCTION(BlueprintCallable, Category = "TGOR Movement", Meta = (Keywords = "C++"))
		virtual void MovementAdjust(const FTGOR_MovementBase& Old, const FTGOR_MovementBase& New);

	/** Attach a component to this movement */
	UFUNCTION(BlueprintCallable, Category = "TGOR Movement", Meta = (Keywords = "C++"))
		virtual void AttachToMovement(USceneComponent* Component, UTGOR_Socket* Socket, int32 Index);

	/** Detach a component from any movement */
	UFUNCTION(BlueprintCallable, Category = "TGOR Movement", Meta = (Keywords = "C++"))
		virtual void DetachFromMovement(USceneComponent* Component);

	/** Attachment sockets used by this actor */
	UPROPERTY(BlueprintReadOnly, Category = "TGOR Movement")
		TArray<UTGOR_Socket*> Sockets;

	////////////////////////////////////////////////////////////////////////////////////////////////////
public:

	/** Get socket matching type */
	UFUNCTION(BlueprintPure, Category = "TGOR Movement", Meta = (DeterminesOutputType = "Type", Keywords = "C++"))
		UTGOR_Socket* GetSocket(TSubclassOf<UTGOR_Socket> Type) const;
		template<typename T> T* GetSOfType(TSubclassOf<T> Type) const
		{
			return Cast<T>(GetSocket(Type));
		}
		template<typename T> T* GetSOfType() const
		{
			return GetSOfType<T>(T::StaticClass());
		}

	/** Get socket matching type */
	UFUNCTION(BlueprintPure, Category = "TGOR Movement", Meta = (DeterminesOutputType = "Type", Keywords = "C++"))
		TArray<UTGOR_Socket*> GetSocketList(TSubclassOf<UTGOR_Socket> Type) const;
		template<typename T> TArray<T*> GetSListOfType(TSubclassOf<T> Type) const
		{
			TArray<T*> Output;
			TArray<UTGOR_Socket*> List = GetSocketList(Type);
			for (UTGOR_Socket* Socket : List)
			{
				Output.Emplace(Cast<T>(Socket));
			}
			return Output;
		}
		template<typename T> TArray<T*> GetSListOfType() const
		{
			return GetSListOfType<T>(T::StaticClass());
		}

	/** Check whether socket type is current unavailable for parenting */
	UFUNCTION(BlueprintPure, Category = "TGOR Movement", Meta = (Keywords = "C++"))
		virtual bool IsSocketOccupied(UTGOR_Socket* Socket) const;

	/** Check whether given component is parented to this component and return socket visuals */
	UFUNCTION(BlueprintPure, Category = "TGOR Movement", Meta = (Keywords = "C++"))
		virtual bool QuerySocketObjects(UTGOR_PilotComponent* Component, FTGOR_MovementVisuals& Visuals) const;

	/** Get all objects attached to a matching type */
	UFUNCTION(BlueprintPure, Category = "TGOR Movement", Meta = (Keywords = "C++"))
		virtual TArray<UTGOR_PilotComponent*> GetSocketObjects(TSubclassOf<UTGOR_Socket> Type) const;

	////////////////////////////////////////////////////////////////////////////////////////////////////
public:

	/** Component moved by this mobility */
	UFUNCTION(BlueprintPure, Category = "TGOR Movement", Meta = (Keywords = "C++"))
		virtual USceneComponent* GetMovementBasis() const;

	/** Get current shape of this movement component */
	UFUNCTION(BlueprintPure, Category = "TGOR Movement", Meta = (Keywords = "C++"))
		const FTGOR_MovementBody& GetBody() const;

	/** Get scale applied to all thresholds (already part of body) */
	UFUNCTION(BlueprintPure, Category = "TGOR Movement", Meta = (Keywords = "C++"))
		const float GetScale() const;

	////////////////////////////////////////////////////////////////////////////////////////////////////
public:

	/** Inflicts an impulse to a given location and part index (does nothing by default) */
	UFUNCTION(BlueprintCallable, Category = "TGOR Movement", Meta = (Keywords = "C++"))
		virtual void InflictPointImpact(const FVector& Point, const FVector& Impulse, int32 Index = -1);

	/** Inflicts a force to a given location (does nothing by default) */
	UFUNCTION(BlueprintCallable, Category = "TGOR Movement", Meta = (Keywords = "C++"))
		virtual void InflictPointForce(const FVector& Point, const FVector& Force, float DeltaTime, int32 Index = -1);

	/** Computes inertial torque factor along a normal (used by collision response) */
	UFUNCTION(BlueprintCallable, Category = "TGOR Movement", Meta = (Keywords = "C++"))
		virtual float ComputeInertial(const FVector& Point, const FVector& Normal) const;

	/** Whether base gets automatically updated */
	UFUNCTION(BlueprintCallable, Category = "TGOR Movement", Meta = (Keywords = "C++"))
		virtual bool UpdatesBase() const;


	/** Register attachement to go active. */
	//UFUNCTION(BlueprintCallable, Category = "TGOR Movement", Meta = (Keywords = "C++"))
	//	void RegisterHandle(TScriptInterface<ITGOR_RegisterInterface> Register, UTGOR_Content* Content, UTGOR_MobilityComponent* Parent, int32 Index);

	/** Unregister a parent manually. */
	//UFUNCTION(BlueprintCallable, Category = "TGOR Movement", Meta = (Keywords = "C++"))
	//	void UnregisterHandleWith(UTGOR_MobilityComponent* Parent);

	/** Unregister manually. */
	//UFUNCTION(BlueprintCallable, Category = "TGOR Movement", Meta = (Keywords = "C++"))
	//	void UnregisterHandle(UTGOR_Content* Content);

	/** Unregister all handles manually. */
	//UFUNCTION(BlueprintCallable, Category = "TGOR Movement", Meta = (Keywords = "C++"))
	//	void UnregisterAll();

	////////////////////////////////////////////////////////////////////////////////////////////////////
public:

	/** Set attachment to given values. */
	UFUNCTION(BlueprintCallable, Category = "TGOR Movement", Meta = (Keywords = "C++"))
		void AttachTo(UTGOR_MobilityComponent* Parent, UTGOR_Socket* Socket, int32 Index);

	/** Reset attachment. */
	UFUNCTION(BlueprintCallable, Category = "TGOR Movement", Meta = (Keywords = "C++"))
		void DetachFrom(UTGOR_MobilityComponent* Parent, UTGOR_Socket* Socket, int32 Index);

	/** Handles for aim suspension */
	//UPROPERTY(BlueprintReadOnly, Category = "TGOR Movement")
	//	TMap<UTGOR_Content*, FTGOR_AttachmentHandle> Handles;


	////////////////////////////////////////////////////////////////////////////////////////////////////
public:

	/** Gets index for a given part name (e.g. bone) */
	UFUNCTION(BlueprintCallable, Category = "TGOR Movement", Meta = (Keywords = "C++"))
		virtual int32 GetIndexFromName(const FName& Name) const;

	/** Gets name for a given part index (e.g. bone) */
	UFUNCTION(BlueprintCallable, Category = "TGOR Movement", Meta = (Keywords = "C++"))
		virtual FName GetNameFromIndex(int32 Index) const;

	////////////////////////////////////////////////////////////////////////////////////////////////////
protected:

	/** Get this or associated component's position */
	UFUNCTION(BlueprintCallable, Category = "TGOR Movement", Meta = (Keywords = "C++"))
		FTGOR_MovementPosition GetComponentPosition() const;

	/** Moves this or associated component to a given position */
	UFUNCTION(BlueprintCallable, Category = "TGOR Movement", Meta = (Keywords = "C++"))
		void SetComponentPosition(const FTGOR_MovementPosition& Position);

	/** Called when this mobility changed position */
	UFUNCTION(BlueprintCallable, Category = "TGOR Movement", Meta = (Keywords = "C++"))
		virtual void OnPositionChange(const FTGOR_MovementPosition& Position);

	/** Called when this mobility changed parent, mainly for component to transform properties that are stored in local space. */
	UFUNCTION(BlueprintCallable, Category = "TGOR Movement", Meta = (Keywords = "C++"))
		virtual void OnReparent(const FTGOR_MovementDynamic& Previous);

	////////////////////////////////////////////////////////////////////////////////////////////////////
public:

	/** Reset base to current position of base component */
	UFUNCTION(BlueprintCallable, Category = "TGOR Movement", Meta = (Keywords = "C++"))
		void ResetToComponent();

	/** Set movement base and update the component */
	UFUNCTION(BlueprintCallable, Category = "TGOR Movement", Meta = (Keywords = "C++"))
		bool SetBase(const FTGOR_MovementBase& NewBase);

	/** Simulates this component to match a part (or none) to a given dynamic, only works if not attached */
	UFUNCTION(BlueprintCallable, Category = "TGOR Movement", Meta = (Keywords = "C++"))
		void SimulateDynamic(const FTGOR_MovementDynamic& Dynamic, UTGOR_Socket* Socket = nullptr, int32 Index = -1);

	////////////////////////////////////////////////////////////////////////////////////////////////////
public:

	/** Check whether there are any handles attaching this */
	UFUNCTION(BlueprintPure, Category = "TGOR Movement", Meta = (Keywords = "C++"))
		bool IsAttached() const;

	/** Check whether whether this component can be attach to given parent (not already attached in chain) */
	UFUNCTION(BlueprintPure, Category = "TGOR Movement", Meta = (Keywords = "C++"))
		bool CanAttachTo(const UTGOR_MobilityComponent* Parent) const;

	/** Check whether a given mobility is currently attached to top-level handle */
	UFUNCTION(BlueprintPure, Category = "TGOR Movement", Meta = (Keywords = "C++"))
		bool IsAttachedTo(const UTGOR_MobilityComponent* Parent, UTGOR_Socket* Socket, int32 Index) const;


	/** Transform according to an indexed component (e.g. bone) and relative socket (can be null) in local space */
	UFUNCTION(BlueprintCallable, Category = "TGOR Movement", Meta = (Keywords = "C++"))
		virtual FTGOR_MovementDynamic GetIndexTransform(UTGOR_Socket* Socket, int32 Index) const;


	/** Compute current movement space */
	UFUNCTION(BlueprintPure, Category = "TGOR Movement", Meta = (Keywords = "C++"))
		const FTGOR_MovementSpace ComputeSpace() const;

	/** Compute current movement position */
	UFUNCTION(BlueprintPure, Category = "TGOR Movement", Meta = (Keywords = "C++"))
		const FTGOR_MovementPosition ComputePosition() const;

	/** Get movement base */
	UFUNCTION(BlueprintPure, Category = "TGOR Movement", Meta = (Keywords = "C++"))
		const FTGOR_MovementBase& GetBase() const;

	////////////////////////////////////////////////////////////////////////////////////////////////////
public:

	/** Get parent component */
	UFUNCTION(BlueprintCallable, Category = "TGOR Movement", Meta = (Keywords = "C++"))
		UTGOR_MobilityComponent* GetParent() const;

	/** Override parent component and index (e.g. for bones) */
	UFUNCTION(BlueprintCallable, Category = "TGOR Movement", Meta = (Keywords = "C++"))
		bool SetParent(UTGOR_MobilityComponent* Parent, UTGOR_Socket* Socket, int32 Index);

	/** Check whether this mobility has a given component in its parent chain */
	UFUNCTION(BlueprintPure, Category = "TGOR Movement", Meta = (Keywords = "C++"))
		bool HasParent(const UTGOR_MobilityComponent* Component) const;

	////////////////////////////////////////////////////////////////////////////////////////////////////
public:

	/** Convert dynamic into position */
	UFUNCTION(BlueprintPure, Category = "TGOR Math", meta = (DisplayName = "ToPosition (pos)", CompactNodeTitle = "->"))
		static FTGOR_MovementPosition Conv_DynamicToPosition(const FTGOR_MovementDynamic& Dynamic);

	/** Convert space into dynamic */
	UFUNCTION(BlueprintPure, Category = "TGOR Math", meta = (DisplayName = "ToDynamic (dynamic)", CompactNodeTitle = "->"))
		static FTGOR_MovementDynamic Conv_SpaceToDynamic(const FTGOR_MovementSpace& Space);

	/** Create body from box */
	UFUNCTION(BlueprintPure, Category = "TGOR Math", meta = (DisplayName = "FromBox (box)"))
		static FTGOR_MovementBody Conv_BodyFromBox(UBoxComponent* Box, const FVector& SurfaceSparsity, float Density);

	/** Create body from sphere */
	UFUNCTION(BlueprintPure, Category = "TGOR Math", meta = (DisplayName = "FromSphere (sphere)"))
		static FTGOR_MovementBody Conv_BodyFromSphere(USphereComponent* Sphere, const FVector& SurfaceSparsity, float Density);

	/** Create body from capsule */
	UFUNCTION(BlueprintPure, Category = "TGOR Math", meta = (DisplayName = "FromCapsule (capsule)"))
		static FTGOR_MovementBody Conv_BodyFromCapsule(UCapsuleComponent* Capsule, const FVector& SurfaceSparsity, float Density);
};
