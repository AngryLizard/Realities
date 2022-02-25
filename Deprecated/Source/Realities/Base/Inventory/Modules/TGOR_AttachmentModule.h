#pragma once

#include "CoreMinimal.h"
#include "Realities/Base/Instances/System/TGOR_ActionInstance.h"

#include "Realities/Base/Inventory/Modules/TGOR_Module.h"
#include "TGOR_AttachmentModule.generated.h"

class UTGOR_PilotComponent;

/**
 * 
 */
UCLASS(BlueprintType, Blueprintable)
class REALITIES_API UTGOR_AttachmentModule : public UTGOR_Module
{
	GENERATED_BODY()

public:

	UTGOR_AttachmentModule();
	virtual bool Equals(const UTGOR_Module* Other) const override;
	virtual bool Merge(const UTGOR_Module* Other, ETGOR_NetvarEnumeration Priority) override;

	virtual void Write_Implementation(FTGOR_GroupWritePackage& Package, UTGOR_Singleton* Context) const override;
	virtual bool Read_Implementation(FTGOR_GroupReadPackage& Package, UTGOR_Singleton* Context) override;
	virtual void Send_Implementation(FTGOR_NetworkWritePackage& Package, UTGOR_Singleton* Context) const override;
	virtual bool Recv_Implementation(FTGOR_NetworkReadPackage& Package, UTGOR_Singleton* Context) override;

	virtual void Clear() override;
	virtual FString Print() const override;

	////////////////////////////////////////////////////////////////////////////////////////////////////

	/** Set attachment. */
	UFUNCTION(BlueprintCallable, Category = "TGOR Action", Meta = (Keywords = "C++"))
		void SetAttachment(UTGOR_PilotComponent* Component);

	/** Get attachment. */
	UFUNCTION(BlueprintCallable, Category = "TGOR Action", Meta = (Keywords = "C++"))
		UTGOR_PilotComponent* GetAttachment() const;

	/** Get whether we store an attachment. */
	UFUNCTION(BlueprintPure, Category = "TGOR Action", Meta = (Keywords = "C++"))
		bool HasAttachement() const;

	/** Whether locked flag is true. */
	UFUNCTION(BlueprintPure, Category = "TGOR Action", Meta = (Keywords = "C++"))
		bool IsLocked() const;

	/** Set lock flag to true. */
	UFUNCTION(BlueprintCallable, Category = "TGOR Action", Meta = (Keywords = "C++"))
		void Lock();

	/** Set lock flag to false. */
	UFUNCTION(BlueprintCallable, Category = "TGOR Action", Meta = (Keywords = "C++"))
		void Unlock();

	////////////////////////////////////////////////////////////////////////////////////////////////////
private:

	/** Currently attached mobility */
	UPROPERTY()
		TWeakObjectPtr<UTGOR_PilotComponent> Attachment;

	/** General purpose flag to set custom attachment behaviour */
	UPROPERTY()
		bool Locked;
};