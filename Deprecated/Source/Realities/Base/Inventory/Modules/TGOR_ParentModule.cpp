
#include "TGOR_ParentModule.h"

#include "Realities/Components/Movement/TGOR_MobilityComponent.h"
#include "Realities/Base/Inventory/Modules/TGOR_Module.h"
#include "Realities/Base/Inventory/TGOR_Storage.h"
#include "Realities/Mod/Inputs/TGOR_Input.h"

#include "Realities/Utility/Storage/TGOR_Serialisation.h"
#include "Realities/Utility/Storage/TGOR_Package.h"
#include "Realities/Base/TGOR_Singleton.h"
#include "Realities/Base/TGOR_GameState.h"

#include "Realities/Base/System/Data/TGOR_WorldData.h"

////////////////////////////////////////////////////////////////////////////////////////////////////

UTGOR_ParentModule::UTGOR_ParentModule()
	: Super()
{
}

bool UTGOR_ParentModule::Equals(const UTGOR_Module* Other) const
{
	if (!Super::Equals(Other))
	{
		return false;
	}

	const UTGOR_ParentModule* OtherModule = static_cast<const UTGOR_ParentModule*>(Other);
	return Component == OtherModule->Component;
}

bool UTGOR_ParentModule::Merge(const UTGOR_Module* Other, ETGOR_NetvarEnumeration Priority)
{
	if (!Super::Merge(Other, Priority))
	{
		return false;
	}

	const UTGOR_ParentModule* OtherModule = static_cast<const UTGOR_ParentModule*>(Other);
	Component = OtherModule->Component;
	return true;
}

void UTGOR_ParentModule::Write_Implementation(FTGOR_GroupWritePackage& Package, UTGOR_Singleton* Context) const
{
	Package.WriteEntry("Component", Component);
	Super::Write_Implementation(Package, Context);
}

bool UTGOR_ParentModule::Read_Implementation(FTGOR_GroupReadPackage& Package, UTGOR_Singleton* Context)
{
	Package.ReadEntry("Component", Component);
	return Super::Read_Implementation(Package, Context);
}

void UTGOR_ParentModule::Send_Implementation(FTGOR_NetworkWritePackage& Package, UTGOR_Singleton* Context) const
{
	Package.WriteEntry(Component);
	Super::Send_Implementation(Package, Context);
}

bool UTGOR_ParentModule::Recv_Implementation(FTGOR_NetworkReadPackage& Package, UTGOR_Singleton* Context)
{
	Package.ReadEntry(Component);
	return Super::Recv_Implementation(Package, Context);
}


void UTGOR_ParentModule::Clear()
{
	Super::Clear();
	Component.Reset();
}

FString UTGOR_ParentModule::Print() const
{
	return FString::Printf(TEXT("%s"), *Super::Print());
}

FName UTGOR_ParentModule::GetSocketName() const
{
	if (Component.IsValid())
	{
		return Component->GetNameFromIndex(BoneIndex);
	}
	return FName();
}


UTGOR_MobilityComponent* UTGOR_ParentModule::GetParent() const
{
	return Component.Get();
}


FTGOR_MovementPosition UTGOR_ParentModule::GetTransform() const
{
	FTGOR_MovementPosition Position;
	if (Component.IsValid() && BoneIndex)
	{
		return Component->GetIndexTransform(nullptr, BoneIndex.Index);
	}
	return Position;
}

void UTGOR_ParentModule::SetParent(UTGOR_MobilityComponent* Parent, FTGOR_Index Index)
{
	MarkDirty();
	Component = Parent;
	BoneIndex = Index;
}