#pragma once

#include "CoreMinimal.h"
#include "RealitiesUtility/Structures/TGOR_Normal.h"

#include "Realities/Base/Inventory/Modules/TGOR_Module.h"
#include "TGOR_InputModule.generated.h"

class TGOR_Input;

/**
 * 
 */
UCLASS(BlueprintType, Blueprintable)
class REALITIES_API UTGOR_InputModule : public UTGOR_Module
{
	GENERATED_BODY()

public:

	UTGOR_InputModule();
	virtual bool Equals(const UTGOR_Module* Other) const override;
	virtual bool Merge(const UTGOR_Module* Other, ETGOR_NetvarEnumeration Priority) override;
	virtual void BuildModule(const UTGOR_Storage* Parent) override;

	virtual void Write_Implementation(FTGOR_GroupWritePackage& Package, UTGOR_Singleton* Context) const override;
	virtual bool Read_Implementation(FTGOR_GroupReadPackage& Package, UTGOR_Singleton* Context) override;
	virtual void Send_Implementation(FTGOR_NetworkWritePackage& Package, UTGOR_Singleton* Context) const override;
	virtual bool Recv_Implementation(FTGOR_NetworkReadPackage& Package, UTGOR_Singleton* Context) override;

	virtual void Clear() override;
	virtual FString Print() const override;
	virtual void Tick(UTGOR_DimensionComponent* Owner, float DeltaSeconds) override;

	////////////////////////////////////////////////////////////////////////////////////////////////////

	/** Set input values. */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Storage", Meta = (Keywords = "C++"))
		void PopulateInputs(const TArray<UTGOR_Input*>& Register);

	////////////////////////////////////////////////////////////////////////////////////////////////////

	/** Set input value. */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Storage", Meta = (Keywords = "C++"))
		void SetInput(TSubclassOf<UTGOR_Input> Input, float Value);

	/** Gets input state */
	UFUNCTION(BlueprintPure, Category = "!TGOR Storage", Meta = (Keywords = "C++"))
		float GetInput(TSubclassOf<UTGOR_Input> Input) const;

	/** Gets input state */
	UFUNCTION(BlueprintPure, Category = "!TGOR Storage", Meta = (Keywords = "C++"))
		bool IsInputOn(TSubclassOf<UTGOR_Input> Input) const;

	/** Checks whether input has just overcome its threshold */
	UFUNCTION(BlueprintPure, Category = "!TGOR Storage", Meta = (Keywords = "C++"))
		bool HasInputSwitchedOn(TSubclassOf<UTGOR_Input> Input) const;

	/** Checks whether input has just overcome its threshold */
	UFUNCTION(BlueprintPure, Category = "!TGOR Storage", Meta = (Keywords = "C++"))
		bool HasInputSwitchedOff(TSubclassOf<UTGOR_Input> Input) const;

	/** Gets last tick's input state */
	UFUNCTION(BlueprintPure, Category = "!TGOR Storage", Meta = (Keywords = "C++"))
		float GetPreviousInput(TSubclassOf<UTGOR_Input> Input) const;

	////////////////////////////////////////////////////////////////////////////////////////////////////
private:

	/** Current state of inputs */
	UPROPERTY()
		TArray<FTGOR_Normal> Values;

	/** Previous state of inputs */
	UPROPERTY()
		TArray<FTGOR_Normal> Previous;

	/** List of possible events */
	UPROPERTY()
		TArray<UTGOR_Input*> Inputs;
};