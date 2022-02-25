#pragma once

#include "CoreMinimal.h"

#include "Realities/Utility/Datastructures/TGOR_Normal.h"
#include "Realities/Base/Inventory/Modules/TGOR_Module.h"
#include "TGOR_SocketModule.generated.h"

class UTGOR_SocketComponent;

/**
 * 
 */
UCLASS(BlueprintType, Blueprintable)
class REALITIES_API UTGOR_SocketModule : public UTGOR_Module
{
	GENERATED_BODY()

public:

	UTGOR_SocketModule();
	virtual bool Equals(const UTGOR_Module* Other) const override;
	virtual bool Merge(const UTGOR_Module* Other, ETGOR_NetvarEnumeration Priority) override;

	virtual void Write_Implementation(FTGOR_GroupWritePackage& Package, UTGOR_Singleton* Context) const override;
	virtual bool Read_Implementation(FTGOR_GroupReadPackage& Package, UTGOR_Singleton* Context) override;
	virtual void Send_Implementation(FTGOR_NetworkWritePackage& Package, UTGOR_Singleton* Context) const override;
	virtual bool Recv_Implementation(FTGOR_NetworkReadPackage& Package, UTGOR_Singleton* Context) override;

	virtual void Clear() override;
	virtual FString Print() const override;

	////////////////////////////////////////////////////////////////////////////////////////////////////

	/** Get current socket radius */
	UFUNCTION(BlueprintPure, Category = "TGOR Storage", Meta = (Keywords = "C++"))
		float GetRadius() const;

	/** Get visibility blend */
	UFUNCTION(BlueprintPure, Category = "TGOR Storage", Meta = (Keywords = "C++"))
		float GetBlend() const;

	/** Get parent component */
	UFUNCTION(BlueprintPure, Category = "TGOR Storage", Meta = (Keywords = "C++"))
		UTGOR_SocketComponent* GetParent() const;

	/** Get transform of this socket */
	UFUNCTION(BlueprintPure, Category = "TGOR Storage", Meta = (Keywords = "C++"))
		FTGOR_MovementPosition GetTransform() const;

	////////////////////////////////////////////////////////////////////////////////////////////////////

	/** Set attached socket state */
	UFUNCTION(BlueprintCallable, Category = "TGOR Storage", Meta = (Keywords = "C++"))
		void SetState(float SocketRadius, float SocketBlend);

	/** Set attached socket parent */
	UFUNCTION(BlueprintCallable, Category = "TGOR Storage", Meta = (Keywords = "C++"))
		void SetParent(UTGOR_SocketComponent* Parent);

	/** Set attached socket name */
	UFUNCTION(BlueprintCallable, Category = "TGOR Storage", Meta = (Keywords = "C++"))
		void SetSocketName(const FName& Name);

	////////////////////////////////////////////////////////////////////////////////////////////////////
private:

	/** Parent component */
	UPROPERTY()
		TWeakObjectPtr<UTGOR_SocketComponent> Component;

	/** Socket owner */
	UPROPERTY()
		TWeakObjectPtr<USceneComponent> Scene;

	/** Visual radius */
	UPROPERTY()
		float Radius;

	/** Visual blend */
	UPROPERTY()
		FTGOR_Normal Blend;

	/** Socket name to attach to */
	UPROPERTY()
		FName Socket;
};