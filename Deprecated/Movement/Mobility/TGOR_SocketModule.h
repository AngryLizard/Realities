#pragma once

#include "CoreMinimal.h"

#include "Realities/Base/Instances/Creature/TGOR_MovementInstance.h"

#include "RealitiesUtility/Structures/TGOR_Normal.h"
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

	/** Compute world space recursively */
	UFUNCTION(BlueprintPure, Category = "!TGOR Storage", Meta = (Keywords = "C++"))
		FTGOR_MovementPosition ComputePosition() const;

	/** Compute world space recursively */
	UFUNCTION(BlueprintPure, Category = "!TGOR Storage", Meta = (Keywords = "C++"))
		FTGOR_MovementSpace ComputeSpace() const;

	////////////////////////////////////////////////////////////////////////////////////////////////////

	/** Get current socket radius */
	UFUNCTION(BlueprintPure, Category = "!TGOR Storage", Meta = (Keywords = "C++"))
		float GetRadius() const;

	/** Get visibility blend */
	UFUNCTION(BlueprintPure, Category = "!TGOR Storage", Meta = (Keywords = "C++"))
		float GetBlend() const;

	/** Get parent component */
	UFUNCTION(BlueprintPure, Category = "!TGOR Storage", Meta = (Keywords = "C++"))
		UTGOR_SocketComponent* GetParent() const;

	/** Look for suitable parent primitive to attach to */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Storage", Meta = (Keywords = "C++"))
		void RecachePrimitive();

	/** Set attached socket state */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Storage", Meta = (Keywords = "C++"))
		void SetRadius(float SocketRadius);

	/** Set attached socket state */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Storage", Meta = (Keywords = "C++"))
		void SetBlend(float SocketBlend);

	/** Set attached socket parent */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Storage", Meta = (Keywords = "C++"))
		void SetParent(UTGOR_SocketComponent* Parent);

	/** Set attached socket name */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Storage", Meta = (Keywords = "C++"))
		void SetSocketName(const FName& Name);

	////////////////////////////////////////////////////////////////////////////////////////////////////

	/** Compare */
	UFUNCTION(BlueprintPure, Category = "!TGOR Storage", Meta = (Keywords = "C++"))
		float Compare(UTGOR_SocketModule* Other, const FTGOR_MovementThreshold& Threshold) const;

	/** Lerp to another socket */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Storage", Meta = (Keywords = "C++"))
		void SetLerp(UTGOR_SocketModule* Other, float Alpha);

	////////////////////////////////////////////////////////////////////////////////////////////////////
private:
	/** Parent primitive cache */
	UPROPERTY()
		TWeakObjectPtr<UPrimitiveComponent> Cache;

	/** Parent component */
	UPROPERTY()
		TWeakObjectPtr<UTGOR_SocketComponent> Component;

	/** Visual radius */
	UPROPERTY()
		float Radius;

	/** Socket blend (below 0.0 drops this socket) */
	UPROPERTY()
		FTGOR_Normal Blend;

	/** Socket name to attach to */
	UPROPERTY()
		FName Socket;
};