// The Gateway of Realities: Planes of Existence.

#pragma once

#include "CoreMinimal.h"
#include "Realities/Utility/TGOR_CustomEnumerations.h"
#include "Realities/Utility/Storage/TGOR_PackageGroup.h"
#include "Realities/Utility/Storage/TGOR_PackageNetwork.h"
#include "Realities/Utility/Storage/TGOR_Serialisation.h"
#include "Components/ActorComponent.h"

#include "Engine/UserDefinedStruct.h"
#include "TGOR_IndexInstance.generated.h"


/**
* TGOR_ContainerIndex stores the index of a container
*/
USTRUCT(BlueprintType)
struct REALITIES_API FTGOR_ContainerIndex
{
	GENERATED_USTRUCT_BODY()
		SERIALISE_INIT_HEADER;
		FTGOR_ContainerIndex();
	FTGOR_ContainerIndex(uint8 Container);
	FTGOR_ContainerIndex(const FTGOR_ContainerIndex& Slot);

	void Write(FTGOR_GroupWritePackage& Package, UTGOR_Singleton* Context) const;
	void Read(FTGOR_GroupReadPackage& Package, UTGOR_Singleton* Context);
	void Send(FTGOR_NetworkWritePackage& Package, UTGOR_Singleton* Context) const;
	void Recv(FTGOR_NetworkReadPackage& Package, UTGOR_Singleton* Context);


	bool operator==(const FTGOR_ContainerIndex& Other) const;
	bool operator!=(const FTGOR_ContainerIndex& Other) const;
	bool operator<(const FTGOR_ContainerIndex& Other) const;
	bool IsValid() const;
	FString Print() const;

	uint8 CompressIntoByte() const;
	void DecompressFromByte(uint8 Byte);

	/** Containers are limited to max 8 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		uint8 Container;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		bool Valid;

	static const FTGOR_ContainerIndex ZeroContainer;
};

template<>
struct TStructOpsTypeTraits<FTGOR_ContainerIndex> : public TStructOpsTypeTraitsBase2<FTGOR_ContainerIndex>
{
	enum
	{
		WithNetSerializer = true
	};
};

static uint32 GetTypeHash(const FTGOR_ContainerIndex& Key)
{
	if (!Key.IsValid())	return(-1);
	return Key.Container;
}

/**
* TGOR_SlotIndex stores the index of a slot
*/
USTRUCT(BlueprintType)
struct REALITIES_API FTGOR_SlotIndex : public FTGOR_ContainerIndex
{
	GENERATED_USTRUCT_BODY()
		SERIALISE_INIT_HEADER;
		FTGOR_SlotIndex();
	FTGOR_SlotIndex(uint8 Byte);
	FTGOR_SlotIndex(uint8 Container, uint8 Slot);
	FTGOR_SlotIndex(const FTGOR_ContainerIndex& Container, uint8 Slot);
	FTGOR_SlotIndex(const FTGOR_SlotIndex& Slot);

	void Write(FTGOR_GroupWritePackage& Package, UTGOR_Singleton* Context) const;
	void Read(FTGOR_GroupReadPackage& Package, UTGOR_Singleton* Context);
	void Send(FTGOR_NetworkWritePackage& Package, UTGOR_Singleton* Context) const;
	void Recv(FTGOR_NetworkReadPackage& Package, UTGOR_Singleton* Context);

	bool operator==(const FTGOR_ContainerIndex& Other) const;
	bool operator==(const FTGOR_SlotIndex& Other) const;
	bool operator!=(const FTGOR_SlotIndex& Other) const;
	bool operator<(const FTGOR_SlotIndex& Other) const;
	FString Print() const;

	int16 CompressIntoWord() const;
	void DecompressFromWord(int16 Word);

	/** Slots are limited to max 32 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		uint8 Slot;


	static const FTGOR_SlotIndex ZeroSlot;
};

template<>
struct TStructOpsTypeTraits<FTGOR_SlotIndex> : public TStructOpsTypeTraitsBase2<FTGOR_SlotIndex>
{
	enum
	{
		WithNetSerializer = true
	};
};

static uint32 GetTypeHash(const FTGOR_SlotIndex& Key)
{
	if (!Key.IsValid())	return(-1);
	return Key.CompressIntoByte();
}

/**
* TGOR_ItemIndex stores the index of an item
*/
USTRUCT(BlueprintType)
struct REALITIES_API FTGOR_ItemIndex : public FTGOR_SlotIndex
{
	GENERATED_USTRUCT_BODY()
		SERIALISE_INIT_HEADER;
		FTGOR_ItemIndex();
	FTGOR_ItemIndex(int16 Word);
	FTGOR_ItemIndex(uint8 Container, uint8 Slot, uint8 Item);
	FTGOR_ItemIndex(const FTGOR_SlotIndex& Slot, uint8 Item);
	FTGOR_ItemIndex(const FTGOR_ContainerIndex& Container, uint8 Slot, uint8 Item);
	FTGOR_ItemIndex(const FTGOR_ItemIndex& Item);

	void Write(FTGOR_GroupWritePackage& Package, UTGOR_Singleton* Context) const;
	void Read(FTGOR_GroupReadPackage& Package, UTGOR_Singleton* Context);
	void Send(FTGOR_NetworkWritePackage& Package, UTGOR_Singleton* Context) const;
	void Recv(FTGOR_NetworkReadPackage& Package, UTGOR_Singleton* Context);

	bool operator==(const FTGOR_ContainerIndex& Other) const;
	bool operator==(const FTGOR_SlotIndex& Other) const;
	bool operator==(const FTGOR_ItemIndex& Other) const;
	bool operator!=(const FTGOR_ItemIndex& Other) const;
	bool operator<(const FTGOR_ItemIndex& Other) const;
	FString Print() const;

	int32 CompressIntoInteger() const;
	void DecompressFromInteger(int32 Integer);

	/** Slots are limited to max 256 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		uint8 Item;

	static const FTGOR_ItemIndex ZeroItem;
};

template<>
struct TStructOpsTypeTraits<FTGOR_ItemIndex> : public TStructOpsTypeTraitsBase2<FTGOR_ItemIndex>
{
	enum
	{
		WithNetSerializer = true
	};
};

static uint32 GetTypeHash(const FTGOR_ItemIndex& Key)
{
	if (!Key.IsValid())	return(-1);
	return Key.CompressIntoWord();
}
