// The Gateway of Realities: Planes of Existence.

#include "TGOR_PilotInstance.h"

#include "CoreSystem/TGOR_Singleton.h"
#include "CoreSystem/Storage/TGOR_Package.h"
#include "CoreSystem/Gameplay/TGOR_GameInstance.h"

#include "DimensionSystem/Content/TGOR_Pilot.h"
#include "DimensionSystem/Content/TGOR_Primitive.h"


SERIALISE_INIT_SOURCE_IMPLEMENT(FTGOR_PilotInstance);
SERIALISE_INIT_SOURCE_IMPLEMENT(FTGOR_AttachInstance);
SERIALISE_INIT_SOURCE_IMPLEMENT(FTGOR_PilotState);


FTGOR_PilotInstance::FTGOR_PilotInstance()
: Primitive(nullptr)
{
}

void FTGOR_PilotInstance::Write(FTGOR_GroupWritePackage& Package, UTGOR_Singleton* Context) const
{
	Package.WriteEntry("Primitive", Primitive);
}

void FTGOR_PilotInstance::Read(FTGOR_GroupReadPackage& Package, UTGOR_Singleton* Context)
{
	Package.ReadEntry("Primitive", Primitive);
}

void FTGOR_PilotInstance::Send(FTGOR_NetworkWritePackage& Package, UTGOR_Context* Context) const
{
	Package.WriteEntry(Primitive);
}

void FTGOR_PilotInstance::Recv(FTGOR_NetworkReadPackage& Package, UTGOR_Context* Context)
{
	Package.ReadEntry(Primitive);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

FTGOR_AttachInstance::FTGOR_AttachInstance()
: Primitive(nullptr)
{
}

void FTGOR_AttachInstance::Write(FTGOR_GroupWritePackage& Package, UTGOR_Singleton* Context) const
{
	Package.WriteEntry("Primitive", Primitive);
}

void FTGOR_AttachInstance::Read(FTGOR_GroupReadPackage& Package, UTGOR_Singleton* Context)
{
	Package.ReadEntry("Primitive", Primitive);
}

void FTGOR_AttachInstance::Send(FTGOR_NetworkWritePackage& Package, UTGOR_Context* Context) const
{
	Package.WriteEntry(Primitive);
}

void FTGOR_AttachInstance::Recv(FTGOR_NetworkReadPackage& Package, UTGOR_Context* Context)
{
	Package.ReadEntry(Primitive);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

FTGOR_PilotState::FTGOR_PilotState()
	: ActiveSlot(INDEX_NONE)
{
}

void FTGOR_PilotState::Send(FTGOR_NetworkWritePackage& Package, UTGOR_Singleton* Context) const
{
	Package.WriteEntry(ActiveSlot);
}

void FTGOR_PilotState::Recv(FTGOR_NetworkReadPackage& Package, UTGOR_Singleton* Context)
{
	Package.ReadEntry(ActiveSlot);
}


struct FTGOR_PilotParamsBaseDeleter
{
	FORCEINLINE void operator()(FTGOR_PilotParamsBase* Object) const
	{
		check(Object);
		UScriptStruct* ScriptStruct = Object->GetScriptStruct();
		check(ScriptStruct);
		ScriptStruct->DestroyStruct(Object);
		FMemory::Free(Object);
	}
};

bool FTGOR_PilotUpdate::NetSerialize(FArchive& Ar, class UPackageMap* Map, bool& bOutSuccess)
{
	State.NetSerialize(Ar, Map, bOutSuccess);

	uint8 DataNum = FMath::Min<int32>(Data.Num(), MAX_uint8);
	Ar << DataNum;
	Data.SetNum(DataNum);

	for (uint8 DataIndex = 0; DataIndex < DataNum && !Ar.IsError(); DataIndex++)
	{

		TSharedPtr<FTGOR_PilotParamsBase>& Handle = Data[DataIndex];
		TCheckedObjPtr<UScriptStruct> ScriptStruct = Handle.IsValid() ? Handle->GetScriptStruct() : nullptr;
		Ar << ScriptStruct;

		if (ScriptStruct.IsValid())
		{
			if (Ar.IsLoading())
			{
				// Recreate struct if invalid or not the same type
				if (!Handle.IsValid() || ScriptStruct.Get() != Handle->GetScriptStruct())
				{
					FTGOR_PilotParamsBase* NewData = (FTGOR_PilotParamsBase*)FMemory::Malloc(ScriptStruct->GetStructureSize());
					ScriptStruct->InitializeStruct(NewData);

					Handle = TSharedPtr<FTGOR_PilotParamsBase>(NewData, FTGOR_PilotParamsBaseDeleter());
				}
			}

			void* ContainerPtr = Handle.Get();
			if (ScriptStruct->StructFlags & STRUCT_NetSerializeNative)
			{
				ScriptStruct->GetCppStructOps()->NetSerialize(Ar, Map, bOutSuccess, ContainerPtr);
			}
			else
			{
				ScriptStruct->SerializeItem(Ar, ContainerPtr, nullptr);

				/*
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
				*/
			}
		}
		else if (ScriptStruct.IsError())
		{
			Ar.SetError();
		}
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

FTGOR_VisualContact::FTGOR_VisualContact()
	: Location(FVector::ZeroVector),
	Normal(FVector::UpVector),
	Blocking(false)
{
}

FTGOR_MovementParent::FTGOR_MovementParent()
	: Mobility(nullptr),
	Index(INDEX_NONE)
{
}
