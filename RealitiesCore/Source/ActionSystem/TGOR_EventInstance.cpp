// The Gateway of Realities: Planes of Existence.


#include "TGOR_EventInstance.h"


void FTGOR_EventInstance::Clear()
{
	Data.Reset();
}

bool FTGOR_EventInstance::IsValid() const
{
	return Data.IsValid();
}


/** Returns debug string */
FString FTGOR_EventInstance::ToString() const
{
	return IsValid() ? Data->ToString() : FString(TEXT("NONE"));
}

/** Comparison operator */
bool FTGOR_EventInstance::operator==(FTGOR_EventInstance const& Other) const
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
bool FTGOR_EventInstance::operator!=(FTGOR_EventInstance const& Other) const
{
	return !(FTGOR_EventInstance::operator==(Other));
}


struct FTGOR_EventParamsBaseDeleter
{
	FORCEINLINE void operator()(FTGOR_EventParamsBase* Object) const
	{
		check(Object);
		UScriptStruct* ScriptStruct = Object->GetScriptStruct();
		check(ScriptStruct);
		ScriptStruct->DestroyStruct(Object);
		FMemory::Free(Object);
	}
};

bool FTGOR_EventInstance::NetSerialize(FArchive& Ar, class UPackageMap* Map, bool& bOutSuccess)
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
					FTGOR_EventParamsBase* NewData = (FTGOR_EventParamsBase*)FMemory::Malloc(ScriptStruct->GetStructureSize());
					ScriptStruct->InitializeStruct(NewData);

					Data = TSharedPtr<FTGOR_EventParamsBase>(NewData, FTGOR_EventParamsBaseDeleter());
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