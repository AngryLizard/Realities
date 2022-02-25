#include "TGOR_FloatStorage.h"

UTGOR_FloatStorage::UTGOR_FloatStorage()
	: Super(),
	ItemRef(nullptr)
{
}

float UTGOR_FloatStorage::GetFloat(int32 Index)
{
	if (IsIndexValid(Index))
	{
		return Values[Index];
	}

	return 0.0f;
}

int32 UTGOR_FloatStorage::AddFloat(float Value)
{
	if (GetCurrentSize() + 1 > GetMaxSize()) return INDEX_NONE;

	return Values.Add(Value);
}

int32 UTGOR_FloatStorage::ReplaceFloat(float Value, int32 Index)
{
	if (IsIndexValid(Index))
	{
		if (RemoveAt(Index))
		{
			int32 NewIndex = Values.Insert(Value, Index);
			return NewIndex != INDEX_NONE;
		}
	}

	return false;
}

void UTGOR_FloatStorage::Clear()
{
	Values.Empty();
}

bool UTGOR_FloatStorage::RemoveAt(int32 Index)
{
	if (IsIndexValid(Index))
	{
		Values.RemoveAt(Index);
		return true;
	}

	return false;
}

void UTGOR_FloatStorage::Assign(const UTGOR_Storage* Other)
{
	auto OtherInventory = Cast<UTGOR_FloatStorage>(Other);
	if (IsValid(OtherInventory))
	{
		this->Values = OtherInventory->Values;
	}
}

int32 UTGOR_FloatStorage::GetMaxSize() const
{
	//return ItemRef->GetMaxStorageSize();
	return 0; // TODO: Discontinued, can make a custom storage for this
}

int32 UTGOR_FloatStorage::GetCurrentSize() const
{
	return Values.Num();
}

bool UTGOR_FloatStorage::IsIndexValid(int32 Index) const
{
	return Index >= 0 && Index < GetCurrentSize() && Values.IsValidIndex(Index);
}

UTGOR_Item* UTGOR_FloatStorage::GetItem() const
{
	return ItemRef;
}

bool UTGOR_FloatStorage::Equals(const UTGOR_Storage* Other) const
{
	if (!IsValid(Other)) return false;

	if (IsValid(ItemRef) && !ItemRef->Equals(Other->GetItem())) return false;

	auto OtherInventory = Cast<UTGOR_FloatStorage>(Other);
	for (float ThisValue : Values)
	{
		for (float OtherValue : OtherInventory->Values)
		{
			if (ThisValue != OtherValue) return false;
		}
	}

	return true;
}

void UTGOR_FloatStorage::SetItem(UTGOR_Item* NewItem)
{
	if (IsValid(NewItem) && NewItem->IsA(UTGOR_StorageItem::StaticClass()))
	{
		ItemRef = Cast<UTGOR_StorageItem>(NewItem);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Can't set item. Item '%s' inherits not of 'UTGOR_StorageItem'"), *NewItem->GetDefaultName())
	}
}