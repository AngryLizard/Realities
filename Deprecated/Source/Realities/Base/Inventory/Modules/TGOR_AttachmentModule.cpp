
#include "TGOR_AttachmentModule.h"

#include "Realities/Components/Movement/TGOR_PilotComponent.h"
#include "Realities/Base/Inventory/Modules/TGOR_Module.h"
#include "Realities/Base/Inventory/TGOR_Storage.h"
#include "Realities/Mod/Inputs/TGOR_Input.h"

#include "Realities/Utility/Storage/TGOR_Serialisation.h"
#include "Realities/Utility/Storage/TGOR_Package.h"
#include "Realities/Base/System/Data/TGOR_WorldData.h"
#include "Realities/Base/TGOR_Singleton.h"
#include "Realities/Base/TGOR_GameState.h"

////////////////////////////////////////////////////////////////////////////////////////////////////

UTGOR_AttachmentModule::UTGOR_AttachmentModule()
	: Super()
{
	ModulePriority = ETGOR_NetvarEnumeration::Server;
	Locked = false;
}

bool UTGOR_AttachmentModule::Equals(const UTGOR_Module* Other) const
{
	if (!Super::Equals(Other))
	{
		return false;
	}

	const UTGOR_AttachmentModule* OtherModule = static_cast<const UTGOR_AttachmentModule*>(Other);
	return Attachment == OtherModule->Attachment && Locked == OtherModule->Locked;
}

bool UTGOR_AttachmentModule::Merge(const UTGOR_Module* Other, ETGOR_NetvarEnumeration Priority)
{
	if (!Super::Merge(Other, Priority))
	{
		return false;
	}

	// Add relevant inputs
	const UTGOR_AttachmentModule* OtherModule = static_cast<const UTGOR_AttachmentModule*>(Other);
	Attachment = OtherModule->Attachment;
	Locked = OtherModule->Locked;

	return true;
}

void UTGOR_AttachmentModule::Write_Implementation(FTGOR_GroupWritePackage& Package, UTGOR_Singleton* Context) const
{
	Package.WriteEntry("Attachment", Attachment);
	Package.WriteEntry("Locked", Locked);
	Super::Write_Implementation(Package, Context);
}

bool UTGOR_AttachmentModule::Read_Implementation(FTGOR_GroupReadPackage& Package, UTGOR_Singleton* Context)
{
	Package.ReadEntry("Attachment", Attachment);
	Package.ReadEntry("Locked", Locked);
	return Super::Read_Implementation(Package, Context);
}

void UTGOR_AttachmentModule::Send_Implementation(FTGOR_NetworkWritePackage& Package, UTGOR_Singleton* Context) const
{
	Package.WriteEntry(Attachment);
	Package.WriteEntry(Locked);
	Super::Send_Implementation(Package, Context);
}

bool UTGOR_AttachmentModule::Recv_Implementation(FTGOR_NetworkReadPackage& Package, UTGOR_Singleton* Context)
{
	Package.ReadEntry(Attachment);
	Package.ReadEntry(Locked);
	return Super::Recv_Implementation(Package, Context);
}


void UTGOR_AttachmentModule::Clear()
{
	Super::Clear();
	Attachment = nullptr;
	Locked = false;
}

FString UTGOR_AttachmentModule::Print() const
{
	FString Name = (Attachment.IsValid() ? Attachment->GetName() : "None");
	return FString::Printf(TEXT("Attachment = %s, Locked = %d, %s"), *Name, Locked, *Super::Print());
}

void UTGOR_AttachmentModule::SetAttachment(UTGOR_PilotComponent* Component)
{
	Attachment = Component;
}

UTGOR_PilotComponent* UTGOR_AttachmentModule::GetAttachment() const
{
	return Attachment.Get();
}

bool UTGOR_AttachmentModule::HasAttachement() const
{
	return Attachment.IsValid();
}

bool UTGOR_AttachmentModule::IsLocked() const
{
	return Locked;
}

void UTGOR_AttachmentModule::Lock()
{
	Locked = true;
}

void UTGOR_AttachmentModule::Unlock()
{
	Locked = false;
}