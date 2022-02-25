// The Gateway of Realities: Planes of Existence.

#include "TGOR_IndexInstance.h"
#include "Realities/Utility/Error/TGOR_Error.h"
#include "Realities/Utility/Storage/TGOR_Package.h"


const FTGOR_ContainerIndex FTGOR_ContainerIndex::ZeroContainer = FTGOR_ContainerIndex(0);
const FTGOR_SlotIndex FTGOR_SlotIndex::ZeroSlot = FTGOR_SlotIndex(ZeroContainer, 0);
const FTGOR_ItemIndex FTGOR_ItemIndex::ZeroItem = FTGOR_ItemIndex(ZeroSlot, 0);

SERIALISE_INIT_IMPLEMENT(FTGOR_ContainerIndex)
SERIALISE_INIT_IMPLEMENT(FTGOR_SlotIndex)
SERIALISE_INIT_IMPLEMENT(FTGOR_ItemIndex)

FTGOR_ContainerIndex::FTGOR_ContainerIndex()
	: Container(0), Valid(false)
{
}

FTGOR_ContainerIndex::FTGOR_ContainerIndex(uint8 Container)
	: Container(Container), Valid(true)
{
}

FTGOR_ContainerIndex::FTGOR_ContainerIndex(const FTGOR_ContainerIndex& Container)
	: Container(Container.Container), Valid(Container.Valid)
{
}

void FTGOR_ContainerIndex::Write(FTGOR_GroupWritePackage& Package, UTGOR_Singleton* Context) const
{
	Package.WriteEntry("Container", Container);
	Package.WriteEntry("Valid", Valid);
}

void FTGOR_ContainerIndex::Read(FTGOR_GroupReadPackage& Package, UTGOR_Singleton* Context)
{
	Package.ReadEntry("Container", Container);
	Package.ReadEntry("Valid", Valid);
}

void FTGOR_ContainerIndex::Send(FTGOR_NetworkWritePackage& Package, UTGOR_Singleton* Context) const
{
	const uint8 Byte = CompressIntoByte();
	Package.WriteEntry(Byte);
}

void FTGOR_ContainerIndex::Recv(FTGOR_NetworkReadPackage& Package, UTGOR_Singleton* Context)
{
	uint8 Byte;
	Package.ReadEntry(Byte);
	DecompressFromByte(Byte);
}


bool FTGOR_ContainerIndex::operator==(const FTGOR_ContainerIndex& Other) const
{
	return(Container == Other.Container && Valid == Other.Valid);
};

bool FTGOR_ContainerIndex::operator!=(const FTGOR_ContainerIndex& Other) const
{
	return(Container != Other.Container || Valid != Other.Valid);
}

bool FTGOR_ContainerIndex::operator<(const FTGOR_ContainerIndex& Other) const
{
	return Container < Other.Container;
}

bool FTGOR_ContainerIndex::IsValid() const
{
	return(Valid);
}

FString FTGOR_ContainerIndex::Print() const
{
	return FString(Valid ? "t" : "f") + "-" + FString::FromInt(Container);
}

uint8 FTGOR_ContainerIndex::CompressIntoByte() const
{
	const uint8 Con = Container & 0b0111'1111;
	const uint8 Val = (Valid & 0b1) << 7;
	return(Con | Val);
}

void FTGOR_ContainerIndex::DecompressFromByte(uint8 Byte)
{
	Container = (Byte & 0b0111'1111);
	Valid = (Byte >> 7) & 0b1;
}


FTGOR_SlotIndex::FTGOR_SlotIndex()
	: FTGOR_ContainerIndex(), Slot(0)
{
}

FTGOR_SlotIndex::FTGOR_SlotIndex(uint8 Byte)
	: FTGOR_SlotIndex()
{
	DecompressFromByte(Byte);
}

FTGOR_SlotIndex::FTGOR_SlotIndex(uint8 Container, uint8 Slot)
	: FTGOR_ContainerIndex(Container), Slot(Slot)
{
}

FTGOR_SlotIndex::FTGOR_SlotIndex(const FTGOR_ContainerIndex& Container, uint8 Slot)
	: FTGOR_ContainerIndex(Container), Slot(Slot)
{
}

FTGOR_SlotIndex::FTGOR_SlotIndex(const FTGOR_SlotIndex& Slot)
	: FTGOR_ContainerIndex(Slot), Slot(Slot.Slot)
{
}

void FTGOR_SlotIndex::Write(FTGOR_GroupWritePackage& Package, UTGOR_Singleton* Context) const
{
	FTGOR_ContainerIndex::Write(Package, Context);
	Package.WriteEntry("Slot", Slot);
}

void FTGOR_SlotIndex::Read(FTGOR_GroupReadPackage& Package, UTGOR_Singleton* Context)
{
	FTGOR_ContainerIndex::Read(Package, Context);
	Package.ReadEntry("Slot", Slot);
}

void FTGOR_SlotIndex::Send(FTGOR_NetworkWritePackage& Package, UTGOR_Singleton* Context) const
{
	const int16 Word = CompressIntoWord();
	Package.WriteEntry(Word);
}

void FTGOR_SlotIndex::Recv(FTGOR_NetworkReadPackage& Package, UTGOR_Singleton* Context)
{
	int16 Word;
	Package.ReadEntry(Word);
	DecompressFromWord(Word);
}

bool FTGOR_SlotIndex::operator==(const FTGOR_ContainerIndex& Other) const
{
	return FTGOR_ContainerIndex::operator==(Other);
}

bool FTGOR_SlotIndex::operator==(const FTGOR_SlotIndex& Other) const
{
	return(Container == Other.Container && Slot == Other.Slot && Valid == Other.Valid);
}

bool FTGOR_SlotIndex::operator!=(const FTGOR_SlotIndex& Other) const
{
	return(Container != Other.Container || Slot != Other.Slot || Valid != Other.Valid);
}

bool FTGOR_SlotIndex::operator<(const FTGOR_SlotIndex& Other) const
{
	if (Container == Other.Container)
	{
		return Slot < Other.Slot;
	}
	return Container < Other.Container;
}

FString FTGOR_SlotIndex::Print() const
{
	return FTGOR_ContainerIndex::Print() + ":" + FString::FromInt(Slot);
}

int16 FTGOR_SlotIndex::CompressIntoWord() const
{
	const int16 Pre = (int16)CompressIntoByte() & 0xFF;
	const int16 Slo = (((int16)Slot) & 0xFF) << 8;
	return(Pre | Slo);
}

void FTGOR_SlotIndex::DecompressFromWord(int16 Word)
{
	DecompressFromByte(Word & 0xFF);
	Slot = (Word >> 8) & 0xFF;
}

/*
bool FTGOR_SlotIndex::NetSerialize(FArchive& Ar, class UPackageMap* Map, bool& bOutSuccess)
{
	int16 Word = CompressIntoWord();
	Ar << Word;
	DecompressFromWord(Word);
	return(bOutSuccess = true);
}
*/


FTGOR_ItemIndex::FTGOR_ItemIndex()
	: FTGOR_SlotIndex(), Item(0)
{
}

FTGOR_ItemIndex::FTGOR_ItemIndex(int16 Word)
	: FTGOR_ItemIndex()
{
	DecompressFromWord(Word);
}

FTGOR_ItemIndex::FTGOR_ItemIndex(uint8 Container, uint8 Slot, uint8 Item)
	: FTGOR_SlotIndex(Container, Slot), Item(Item)
{
}

FTGOR_ItemIndex::FTGOR_ItemIndex(const FTGOR_SlotIndex& Slot, uint8 Item)
	: FTGOR_SlotIndex(Slot), Item(Item)
{
}

FTGOR_ItemIndex::FTGOR_ItemIndex(const FTGOR_ContainerIndex& Container, uint8 Slot, uint8 Item)
	: FTGOR_SlotIndex(Container, Slot), Item(Item)
{
}
FTGOR_ItemIndex::FTGOR_ItemIndex(const FTGOR_ItemIndex& Item)
	: FTGOR_SlotIndex(Item), Item(Item.Item)
{
}

void FTGOR_ItemIndex::Write(FTGOR_GroupWritePackage& Package, UTGOR_Singleton* Context) const
{
	FTGOR_SlotIndex::Write(Package, Context);
	Package.WriteEntry("Item", Item);
}

void FTGOR_ItemIndex::Read(FTGOR_GroupReadPackage& Package, UTGOR_Singleton* Context)
{
	FTGOR_SlotIndex::Read(Package, Context);
	Package.ReadEntry("Item", Item);
}

void FTGOR_ItemIndex::Send(FTGOR_NetworkWritePackage& Package, UTGOR_Singleton* Context) const
{
	const int32 Integer = CompressIntoInteger();
	Package.WriteEntry(Integer);
}

void FTGOR_ItemIndex::Recv(FTGOR_NetworkReadPackage& Package, UTGOR_Singleton* Context)
{
	int32 Integer;
	Package.ReadEntry(Integer);
	DecompressFromInteger(Integer);
}

bool FTGOR_ItemIndex::operator==(const FTGOR_ContainerIndex& Other) const
{
	return FTGOR_ContainerIndex::operator==(Other);
}

bool FTGOR_ItemIndex::operator==(const FTGOR_SlotIndex& Other) const
{
	return FTGOR_SlotIndex::operator==(Other);
}

bool FTGOR_ItemIndex::operator==(const FTGOR_ItemIndex& Other) const
{
	return(Container == Other.Container && Slot == Other.Slot && Item == Other.Item && Valid == Other.Valid);
}

bool FTGOR_ItemIndex::operator!=(const FTGOR_ItemIndex& Other) const
{
	return(Container != Other.Container || Slot != Other.Slot || Item != Other.Item || Valid != Other.Valid);
}

bool FTGOR_ItemIndex::operator<(const FTGOR_ItemIndex& Other) const
{
	if (Container == Other.Container)
	{
		if (Slot == Other.Slot)
		{
			return Item < Other.Item;
		}
		return Slot < Other.Slot;
	}
	return Container < Other.Container;
}

FString FTGOR_ItemIndex::Print() const
{
	return FTGOR_SlotIndex::Print() + ":" + FString::FromInt(Item);
}

int32 FTGOR_ItemIndex::CompressIntoInteger() const
{
	const int32 Pre = (int32)CompressIntoWord() & 0xFFFF;
	const int32 Itm = (((int32)Item) & 0xFF) << 16;
	return(Pre | Itm);
}

void FTGOR_ItemIndex::DecompressFromInteger(int32 Integer)
{
	DecompressFromWord(Integer & 0xFFFF);
	Slot = (Integer >> 16) & 0xFF;
}

/*
bool FTGOR_ItemIndex::NetSerialize(FArchive& Ar, class UPackageMap* Map, bool& bOutSuccess)
{
	int32 Integer = CompressIntoInteger();
	Ar << Integer;
	DecompressFromInteger(Integer);
	return(bOutSuccess = true);
}
*/