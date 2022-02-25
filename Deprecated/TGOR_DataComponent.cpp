// The Gateway of Realities: Planes of Existence.


#include "TGOR_DataComponent.h"
#include "GameFramework/Actor.h"

#include "Net/UnrealNetwork.h"
#include "Engine/NetConnection.h"
#include "Engine/ActorChannel.h"


void FTGOR_EventParamContextHandle::Clear()
{
	Data.Reset();
}

bool FTGOR_EventParamContextHandle::IsValid() const
{
	return Data.IsValid();
}


/** Returns debug string */
FString FTGOR_EventParamContextHandle::ToString() const
{
	return IsValid() ? Data->ToString() : FString(TEXT("NONE"));
}

/** Comparison operator */
bool FTGOR_EventParamContextHandle::operator==(FTGOR_EventParamContextHandle const& Other) const
{
	if (Data.IsValid() != Other.Data.IsValid())
	{
		return false;
	}
	if (Data.Get() != Other.Data.Get())
	{
		return false;
	}
	return true;
}

/** Comparison operator */
bool FTGOR_EventParamContextHandle::operator!=(FTGOR_EventParamContextHandle const& Other) const
{
	return !(FTGOR_EventParamContextHandle::operator==(Other));
}


struct FTGOR_EventParamContextDeleter
{
	FORCEINLINE void operator()(FTGOR_EventParamContext* Object) const
	{
		check(Object);
		UScriptStruct* ScriptStruct = Object->GetScriptStruct();
		check(ScriptStruct);
		ScriptStruct->DestroyStruct(Object);
		FMemory::Free(Object);
	}
};

bool FTGOR_EventParamContextHandle::NetSerialize(FArchive& Ar, class UPackageMap* Map, bool& bOutSuccess)
{
	bool ValidData = Data.IsValid();
	Ar.SerializeBits(&ValidData, 1);

	if (ValidData)
	{
		TCheckedObjPtr<UScriptStruct> ScriptStruct = Data.IsValid() ? Data->GetScriptStruct() : nullptr;
		Ar << ScriptStruct;

		if (ScriptStruct.IsValid())
		{
			if (Ar.IsLoading())
			{
				// Recreate struct if invalid or not the same type
				if (!Data.IsValid() || ScriptStruct.Get() != Data->GetScriptStruct())
				{
					FTGOR_EventParamContext* NewData = (FTGOR_EventParamContext*)FMemory::Malloc(ScriptStruct->GetStructureSize());
					ScriptStruct->InitializeStruct(NewData);

					Data = TSharedPtr<FTGOR_EventParamContext>(NewData, FTGOR_EventParamContextDeleter());
				}
			}

			void* ContainerPtr = Data.Get();
			if (ScriptStruct->StructFlags & STRUCT_NetSerializeNative)
			{
				ScriptStruct->GetCppStructOps()->NetSerialize(Ar, Map, bOutSuccess, ContainerPtr);
			}
			else
			{
				// See FGameplayAbilityTargetDataHandle::NetSerialize, apparently this is deprecated
				for (TFieldIterator<FProperty> It(ScriptStruct.Get()); It; ++It)
				{
					if (It->PropertyFlags & CPF_RepSkip)
					{
						continue;
					}

					void* PropertyData = It->ContainerPtrToValuePtr<void*>(ContainerPtr);

					It->NetSerializeItem(Ar, Map, PropertyData);
				}
			}
		}
		else if (ScriptStruct.IsError())
		{
			Ar.SetError();
		}
	}
	else
	{
		Data.Reset();
	}

	// Make sure to not keep invalid data on rep error
	if (Ar.IsError())
	{
		RPORT("Error replicating EventParamContextHandle");
		Data.Reset();
		bOutSuccess = false;
		return false;
	}

	bOutSuccess = true;
	return true;

}




UTGOR_TestRepData::UTGOR_TestRepData()
{
}

void UTGOR_TestRepData::GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION(UTGOR_TestRepData, Thing, COND_None);

	// Blueprint properties are not added automatically
	if (const UBlueprintGeneratedClass* BPClass = Cast<UBlueprintGeneratedClass>(GetClass()))
	{
		BPClass->GetLifetimeBlueprintReplicationList(OutLifetimeProps);
	}
}

bool UTGOR_TestRepData::IsSupportedForNetworking() const
{
	return true;
}



int32 UTGOR_TestRepData::GetFunctionCallspace(UFunction* Function, FFrame* Stack)
{
	if (HasAnyFlags(RF_ClassDefaultObject) || !IsSupportedForNetworking())
	{
		// This handles absorbing authority/cosmetic
		return GEngine->GetGlobalFunctionCallspace(Function, this, Stack);
	}
	check(GetOuter() != nullptr);
	return GetOuter()->GetFunctionCallspace(Function, Stack);
}

bool UTGOR_TestRepData::CallRemoteFunction(UFunction* Function, void* Parameters, FOutParmRec* OutParms, FFrame* Stack)
{
	check(!HasAnyFlags(RF_ClassDefaultObject));
	check(GetOuter() != nullptr);

	AActor* Owner = CastChecked<AActor>(GetOuter());

	bool bProcessed = false;

	FWorldContext* const Context = GEngine->GetWorldContextFromWorld(GetWorld());
	if (Context != nullptr)
	{
		for (FNamedNetDriver& Driver : Context->ActiveNetDrivers)
		{
			if (Driver.NetDriver != nullptr && Driver.NetDriver->ShouldReplicateFunction(Owner, Function))
			{
				Driver.NetDriver->ProcessRemoteFunction(Owner, Function, Parameters, OutParms, Stack, this);
				bProcessed = true;
			}
		}
	}

	return bProcessed;
}



void UTGOR_TestRepData::ServerCall_Implementation(int32 Identifier)
{
	RPORT("Server call with");
	VPORT(Identifier);
}

bool UTGOR_TestRepData::ServerCall_Validate(int32 Identifier)
{
	return true;
}

void UTGOR_TestRepData::ClientCall_Implementation(int32 Identifier)
{
	RPORT("Client call with");
	VPORT(Identifier);
}

void UTGOR_TestRepData::ClientParamCall_Implementation(FTGOR_EventParamContextHandle Param)
{

	RPORT("Client call with param");
	VPORT(Param.Get<FTGOR_TestWithSerialEventParamContext>()->TestFloat);
}



UTGOR_PrimaryTestRepData::UTGOR_PrimaryTestRepData()
{

}

void UTGOR_PrimaryTestRepData::GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION(UTGOR_PrimaryTestRepData, Stuff, COND_None);
}



UTGOR_SecondaryTestRepData::UTGOR_SecondaryTestRepData()
{

}

void UTGOR_SecondaryTestRepData::GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION(UTGOR_SecondaryTestRepData, Barks, COND_None);
}




UTGOR_DataComponent::UTGOR_DataComponent()
	: Super()
{
	SetIsReplicatedByDefault(true);
}

void UTGOR_DataComponent::BeginPlay()
{
	Super::BeginPlay();
	
}

void UTGOR_DataComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

}

void UTGOR_DataComponent::GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION(UTGOR_DataComponent, SubobjectData, COND_None);
}

void UTGOR_DataComponent::GetSubobjectsWithStableNamesForNetworking(TArray<UObject*>& Objs)
{
	if (SubobjectData && SubobjectData->IsNameStableForNetworking())
	{
		Objs.Add(const_cast<UTGOR_TestRepData*>(SubobjectData));
	}
}

bool UTGOR_DataComponent::ReplicateSubobjects(class UActorChannel* Channel, class FOutBunch* Bunch, FReplicationFlags* RepFlags)
{
	bool WroteSomething = Super::ReplicateSubobjects(Channel, Bunch, RepFlags);
	if (IsValid(SubobjectData))
	{
		WroteSomething |= Channel->ReplicateSubobject(SubobjectData, *Bunch, *RepFlags);
	}
	return WroteSomething;
}

FTGOR_EventParamContextHandle UTGOR_DataComponent::CreateParams(float Param) const
{
	FTGOR_EventParamContextHandle Handle = FTGOR_EventParamContextHandle::Create<FTGOR_TestWithSerialEventParamContext>();

	FTGOR_TestWithSerialEventParamContext* Dat = Handle.Get<FTGOR_TestWithSerialEventParamContext>();
	if (Dat)
	{
		Dat->TestFloat = Param;
	}
	else
	{
		RPORT("Param creation is null");
	}
	
	return Handle;
}


void UTGOR_DataComponent::ClientParamCall_Implementation(UTGOR_TestRepData* Param)
{

	RPORT("Client call with param");

	if (IsValid(Param))
	{
		VPORT(Param->Thing);
	}
	else
	{

		RPORT("None");
	}
}