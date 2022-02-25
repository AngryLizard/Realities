
#include "TGOR_InputModule.h"

#include "Realities/Base/Instances/System/TGOR_EventInstance.h"
#include "Realities/Mod/Inputs/TGOR_Input.h"
#include "Realities/Base/Inventory/TGOR_Storage.h"
#include "Realities/CoreSystem/Storage/TGOR_Package.h"

////////////////////////////////////////////////////////////////////////////////////////////////////

UTGOR_InputModule::UTGOR_InputModule()
	: Super()
{
	ModulePriority = ETGOR_NetvarEnumeration::Client;
}

bool UTGOR_InputModule::Equals(const UTGOR_Module* Other) const
{
	if (!Super::Equals(Other))
	{
		return false;
	}

	const UTGOR_InputModule* OtherModule = static_cast<const UTGOR_InputModule*>(Other);
	return Values == OtherModule->Values;
}

bool UTGOR_InputModule::Merge(const UTGOR_Module* Other, ETGOR_NetvarEnumeration Priority)
{
	if (!Super::Merge(Other, Priority))
	{
		return false;
	}

	// Add relevant inputs
	const UTGOR_InputModule* OtherModule = static_cast<const UTGOR_InputModule*>(Other);
	Previous = Values;

	for (int32 Index = 0; Index < Inputs.Num(); Index++)
	{
		if (Priority == ETGOR_NetvarEnumeration::Local || Inputs[Index]->IsReplicated())
		{
			Values[Index] = OtherModule->Values[Index];
		}
	}
	return true;
}

void UTGOR_InputModule::BuildModule(const UTGOR_Storage* Parent)
{
	Super::BuildModule(Parent);

	PopulateInputs(Parent->GetOwner()->GetIListFromType<UTGOR_Input>());
}

void UTGOR_InputModule::Write_Implementation(FTGOR_GroupWritePackage& Package, UTGOR_Singleton* Context) const
{
	Package.WriteEntry("Values", Values);
	Super::Write_Implementation(Package, Context);
}

bool UTGOR_InputModule::Read_Implementation(FTGOR_GroupReadPackage& Package, UTGOR_Singleton* Context)
{
	Package.ReadEntry("Values", Values);
	return Super::Read_Implementation(Package, Context);
}

void UTGOR_InputModule::Send_Implementation(FTGOR_NetworkWritePackage& Package, UTGOR_Singleton* Context) const
{
	for (int32 Index = 0; Index < Inputs.Num(); Index++)
	{
		UTGOR_Input* Input = Inputs[Index];
		if (Input->IsReplicated())
		{
			// TODO: Differentiate binary/gradual
			Package.WriteEntry(Values[Index]);
		}
	}
	Super::Send_Implementation(Package, Context);
}

bool UTGOR_InputModule::Recv_Implementation(FTGOR_NetworkReadPackage& Package, UTGOR_Singleton* Context)
{
	for (int32 Index = 0; Index < Inputs.Num(); Index++)
	{
		UTGOR_Input* Input = Inputs[Index];
		if (Input->IsReplicated())
		{
			// TODO: Differentiate binary/gradual
			Package.ReadEntry(Values[Index]);
		}
	}
	return Super::Recv_Implementation(Package, Context);
}


void UTGOR_InputModule::Clear()
{
	Super::Clear();
	for (FTGOR_Normal& State : Values)
	{
		State = 0.0f;
	}
	Previous = Values;
}

FString UTGOR_InputModule::Print() const
{
	FString Out = "";
	for (int32 Index = 0; Index < Inputs.Num(); Index++)
	{
		Out += FString::Printf(TEXT("%s = %.0f>%.0f,"), *Inputs[Index]->GetDefaultName(), Previous[Index].Value, Values[Index].Value);
	}
	return Out + ", " + Super::Print();
}

void UTGOR_InputModule::Tick(UTGOR_DimensionComponent* Owner, float DeltaSeconds)
{
	Previous = Values;
	Super::Tick(Owner, DeltaSeconds);
}

void UTGOR_InputModule::PopulateInputs(const TArray<UTGOR_Input*>& Register)
{
	Inputs = Register;
	Values.SetNumZeroed(Inputs.Num());
	Previous = Values;
}


void UTGOR_InputModule::SetInput(TSubclassOf<UTGOR_Input> Input, float Value)
{
	for (int32 Index = 0; Index < Inputs.Num(); Index++)
	{
		UTGOR_Input* Content = Inputs[Index];
		if (Content->IsA(Input))
		{
			// Set push if activity changed
			MarkDirty(Content->IsActive(Value) != Content->IsActive(Values[Index]));
			Values[Index] = Value;
		}
	}
}

float UTGOR_InputModule::GetInput(TSubclassOf<UTGOR_Input> Input) const
{
	float Value = 0.0f;
	for (int32 Index = 0; Index < Inputs.Num(); Index++)
	{
		if (Inputs[Index]->IsA(Input))
		{
			Value = FMath::Max(Values[Index].Value, Value);
		}
	}
	return Value;
}

bool UTGOR_InputModule::IsInputOn(TSubclassOf<UTGOR_Input> Input) const
{
	for (int32 Index = 0; Index < Inputs.Num(); Index++)
	{
		UTGOR_Input* Content = Inputs[Index];
		if (Content->IsA(Input) && Content->IsActive(Values[Index]))
		{
			return true;
		}
	}
	return false;
}

bool UTGOR_InputModule::HasInputSwitchedOn(TSubclassOf<UTGOR_Input> Input) const
{
	for (int32 Index = 0; Index < Inputs.Num(); Index++)
	{
		UTGOR_Input* Content = Inputs[Index];
		if (Content->IsA(Input) && Content->IsActive(Values[Index]) && !Content->IsActive(Previous[Index]))
		{
			return true;
		}
	}
	return false;
}

bool UTGOR_InputModule::HasInputSwitchedOff(TSubclassOf<UTGOR_Input> Input) const
{
	for (int32 Index = 0; Index < Inputs.Num(); Index++)
	{
		UTGOR_Input* Content = Inputs[Index];
		if (Content->IsA(Input) && !Content->IsActive(Values[Index]) && Content->IsActive(Previous[Index]))
		{
			return true;
		}
	}
	return false;
}

float UTGOR_InputModule::GetPreviousInput(TSubclassOf<UTGOR_Input> Input) const
{
	float Value = 0.0f;
	for (int32 Index = 0; Index < Inputs.Num(); Index++)
	{
		if (Inputs[Index]->IsA(Input))
		{
			Value = FMath::Max(Previous[Index].Value, Value);
		}
	}
	return Value;
}