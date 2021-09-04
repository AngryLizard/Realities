// The Gateway of Realities: Planes of Existence.

#pragma once

#include "CoreMinimal.h"

#include "AttributeSystem/TGOR_AttributeInstance.h"

#include "AttributeSystem/Interfaces/TGOR_AttributeInterface.h"
#include "DimensionSystem/Components/TGOR_PilotComponent.h"
#include "TGOR_SocketComponent.generated.h"

class UTGOR_Target;
class UTGOR_NamedSocket;
class UTGOR_NamedSocketTask;

///////////////////////////////////////////// DELEGATES ///////////////////////////////////////////////////

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FNamedSocketDelegate, UTGOR_NamedSocket*, Socket);

/**
 * TGOR_SocketComponent allows parenting to sockets of attached primitives.
 */
UCLASS(ClassGroup = (Custom), Blueprintable, meta = (BlueprintSpawnableComponent))
class SOCKETSYSTEM_API UTGOR_SocketComponent : public UTGOR_PilotComponent, public ITGOR_AttributeInterface
{
	GENERATED_BODY()

	friend class UTGOR_NamedSocket;
	friend class UTGOR_NamedSocketTask;

public:
	UTGOR_SocketComponent();

	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	virtual void UpdateAttributes_Implementation(const UTGOR_AttributeComponent* Component) override;
	virtual float GetAttribute_Implementation(UTGOR_Attribute* Attribute, float Default) const override;

	virtual void UpdateContent_Implementation(UTGOR_Spawner* Spawner) override;

	//////////////////////////////////////////// IMPLEMENTABLES ////////////////////////////////////////

	UPROPERTY(BlueprintAssignable, Category = "!TGOR Movement")
		FNamedSocketDelegate OnSocketChanged;

	////////////////////////////////////////////////////////////////////////////////////////////////////
protected:

	/** Attachments to this component */
	UPROPERTY()
		TMap<UTGOR_NamedSocket*, TWeakObjectPtr<UTGOR_NamedSocketTask>> Attachments;

	/** Current socket attributes */
	UPROPERTY()
		FTGOR_AttributeInstance Attributes;


	////////////////////////////////////////////////////////////////////////////////////////////////////
public:

	/** Current transform of given socket in world space */
	UFUNCTION(BlueprintPure, Category = "!TGOR Movement", Meta = (Keywords = "C++"))
		FTGOR_MovementPosition GetTransformOfSocket(const FName& Socket) const;

	/** Gets socket a given object is attached to or null if it is not attached to any socket */
	UFUNCTION(BlueprintPure, Category = "!TGOR Movement", Meta = (Keywords = "C++"))
		UTGOR_NamedSocket* GetAttachedSocket(UTGOR_PilotComponent* Attachee) const;

	/** Gets socket whose target insertions contain the given target */
	UFUNCTION(BlueprintPure, Category = "!TGOR Movement", Meta = (Keywords = "C++"))
		UTGOR_NamedSocket* GetTargetedSocket(UTGOR_Target* Target) const;

	/** Gets currently to a socket attached mobility */
	UFUNCTION(BlueprintPure, Category = "!TGOR Movement", Meta = (Keywords = "C++"))
		UTGOR_PilotComponent* GetAttached(UTGOR_NamedSocket* Socket) const;

	/** Gets currently to a socket type attached mobilities */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Movement", Meta = (Keywords = "C++"))
		TArray<UTGOR_PilotComponent*> GetAttachedList(TSubclassOf<UTGOR_NamedSocket> Type) const;

	/** Gets first available socket of given type */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Movement", Meta = (DeterminesOutputType = "Type", Keywords = "C++"))
		UTGOR_NamedSocket* GetSocketOfType(TSubclassOf<UTGOR_NamedSocket> Type) const;
		template<typename T> T* GetSOfType(TSubclassOf<T> Type) const
		{
			return Cast<T>(GetSocketOfType(Type));
		}
		template<typename T> T* GetSOfType() const
		{
			return GetSOfType<T>(T::StaticClass());
		}

		/** Gets first available socket of given type */
		UFUNCTION(BlueprintCallable, Category = "!TGOR Movement", Meta = (DeterminesOutputType = "Type", Keywords = "C++"))
		TArray<UTGOR_NamedSocket*> GetSocketListOfType(TSubclassOf<UTGOR_NamedSocket> Type) const;
		template<typename T> TArray<T*> GetSListOfType(TSubclassOf<T> Type) const
		{
			TArray<T*> List;
			TArray<UTGOR_NamedSocket*> Sockets = GetSocketListOfType(Type);
			for (UTGOR_NamedSocket* Socket : Sockets) {
				List.Emplace(Cast<T>(Socket));
			}
			return List;
		}
		template<typename T> TArray<T*> GetSListOfType() const
		{
			return GetSListOfType<T>(T::StaticClass());
		}

	/** Gets accumulate shape of all bodies attached to a socket */
	UFUNCTION(BlueprintPure, Category = "!TGOR Movement", Meta = (Keywords = "C++"))
		FTGOR_MovementShape GetDisplacedShape(UTGOR_NamedSocket* Socket) const;

	/** Set radius and blend of socket */
	UFUNCTION(BlueprintCallable,  Category = "!TGOR Movement", Meta = (Keywords = "C++"))
		void SetSocketVisuals(UTGOR_NamedSocket* Socket, float Radius, float Blend);

	////////////////////////////////////////////////////////////////////////////////////////////////////
protected:

	/** Finds whether a shape can be attached to this component at a given socket */
	UFUNCTION(BlueprintPure, Category = "!TGOR Movement", Meta = (Keywords = "C++"))
		bool CanFitToSocket(const FTGOR_MovementShape& Shape, UTGOR_NamedSocket* Socket) const;

	/** Finds whether an object can be attached to this component at a given socket */
	UFUNCTION(BlueprintPure, Category = "!TGOR Movement", Meta = (Keywords = "C++"))
		bool CanAttachToSocket(UTGOR_PilotComponent* Attachee, UTGOR_NamedSocket* Socket) const;

	/** Finds whether an object is currently attached to this component at a given socket */
	UFUNCTION(BlueprintPure, Category = "!TGOR Movement", Meta = (Keywords = "C++"))
		bool IsAttachedToSocket(UTGOR_PilotComponent* Attachee, UTGOR_NamedSocket* Socket) const;

	////////////////////////////////////////////////////////////////////////////////////////////////////
public:

	/** Parents a component to this mobility using the Parent socket. */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Movement", Meta = (Keywords = "C++"))
		bool ParentNamedSocket(UTGOR_PilotComponent* Attachee, UTGOR_NamedSocket* Socket);

	/** Parents a component to this mobility using the Parent socket. */
	UFUNCTION(BlueprintPure, Category = "!TGOR Movement", Meta = (Keywords = "C++"))
		bool CanParentNamedSocket(UTGOR_PilotComponent* Attachee, UTGOR_NamedSocket* Socket) const;

	////////////////////////////////////////////////////////////////////////////////////////////////////
private:

	/** Primitive bindings to this component */
	UPROPERTY()
		TMap<FName, TWeakObjectPtr<UPrimitiveComponent>> PrimitiveCache;

	/** Add a child to attachment list */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Movement", Meta = (Keywords = "C++"))
		void RegisterAttachToSocket(UTGOR_NamedSocketTask* Attachee, UTGOR_NamedSocket* Socket);

	/** Remove a child from attachment list */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Movement", Meta = (Keywords = "C++"))
		void RegisterDetachFromSocket(UTGOR_NamedSocketTask* Attachee, UTGOR_NamedSocket* Socket);

	////////////////////////////////////////////////////////////////////////////////////////////////////
public:

	/** Update primitive cache to include new primitives and exclude old ones.
		This is automatically called on Socket changes.
		Call manually if new components with sockets have been added. */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Movement|Internal", Meta = (Keywords = "C++"))
		void UpdatePrimitiveCache();

	/** Check current attachments again for whether they fit their socket.
		This is automatically called on socket and attribute changes.
		Call manually if something has changed externally that could invalidate existing sockets. */
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "!TGOR Movement|Internal", Meta = (Keywords = "C++"))
		void UpdateCurrentAttachments();


};
