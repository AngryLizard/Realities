// The Gateway of Realities: Planes of Existence. By Salireths.

#include "TGOR_ItemInstance_OLD.h"
#include "Realities.h"

#include "Base/TGOR_Singleton.h"
#include "Mod/Items/TGOR_Item.h"
#include "Base/Instances/Inventory/TGOR_ItemInstance_OLD.h"
#include "Base/Content/TGOR_ContentManager.h"
#include "Base/TGOR_GameInstance.h"


FTGOR_ItemInstance_OLD::FTGOR_ItemInstance_OLD()
{
	Content = nullptr;
	Quantity = 0.0f;
	Update = false;
	Flags = 0;
}

FTGOR_ItemInstance_OLD& FTGOR_ItemInstance_OLD::operator!()
{
	Update = true;
	return(*this);
}

bool FTGOR_ItemInstance_OLD::Write(UTGOR_Singleton* Singleton, FilePack& Pack) const
{
	SET_FILE_SOURCE(0, Item, Content, DIRECT)
	SET_FILE_NUMBER(1, FLOAT, Quantity, f, float, DIRECT)
	SET_FILE_NUMBER(2, BOOL, Update, b, bool, DIRECT)
	SET_FILE_NUMBER(3, SIGNED, Flags, i, int32, DIRECT)
	SET_FILE_BUFFER(4, Data, DIRECT)

	return(true);
}

bool FTGOR_ItemInstance_OLD::Read(UTGOR_Singleton* Singleton, const FilePack& Pack)
{
	GET_FILE_SOURCE(0, Item, Content, DIRECT)
	GET_FILE_NUMBER(1, FLOAT, Quantity, f, float, DIRECT)
	GET_FILE_NUMBER(2, BOOL, Update, b, bool, DIRECT)
	GET_FILE_NUMBER(3, SIGNED, Flags, i, int32, DIRECT)
	GET_FILE_BUFFER(4, Data, DIRECT)

	return(true);
}

bool FTGOR_ItemInstance_OLD::NetSerialize(FArchive& Ar, class UPackageMap* Map, bool& bOutSuccess)
{
	uint8 Mask;
	SERIALISE_INIT_SOURCE 

	SERIALISE_MASK_SOURCE(Content, Item)
	SERIALISE_MASK_ATTR(Quantity, 0.0f)
	SERIALISE_MASK_BOOL(Update)
	SERIALISE_MASK_ATTR(Flags, 0)
	SERIALISE_MASK_LIST(Data)

	return(bOutSuccess);
}