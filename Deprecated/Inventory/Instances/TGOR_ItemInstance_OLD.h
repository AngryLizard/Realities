// The Gateway of Realities: Planes of Existence. By Salireths.

#pragma once

#include "Utility/Storage/TGOR_FileHandlePointer.h"
#include "Utility/Error/TGOR_Error.h"

#include "Engine/UserDefinedStruct.h"
#include "TGOR_ItemInstance_OLD.generated.h"

class UTGOR_Item;
/**
 * TGOR_ItemInstance_OLD stores general data
 */
USTRUCT(BlueprintType)
struct REALITIES_API FTGOR_ItemInstance_OLD
{
	GENERATED_USTRUCT_BODY()

	FTGOR_ItemInstance_OLD();

	/* Prefix operator that automatically sets Update to true */
	FTGOR_ItemInstance_OLD& operator!();

	using FilePack = CTGOR_FileType<CTGOR_String, CTGOR_Number, CTGOR_Number, CTGOR_Number, CTGOR_Buffer>;

	bool Write(UTGOR_Singleton* Singleton, FilePack& Pack) const;
	bool Read(UTGOR_Singleton* Singleton, const FilePack& Pack);

	////////////////////////////////////////////////////////////////////////////////////////////////////

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TGOR|Instance")
		UTGOR_Item* Content;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TGOR|Instance")
		float Quantity;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TGOR|Instance")
		bool Update;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TGOR|Instance")
		int32 Flags;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TGOR|Instance")
		TArray<uint8> Data;

	bool NetSerialize(FArchive& Ar, class UPackageMap* Map, bool& bOutSuccess);
};

template<>
struct TStructOpsTypeTraits<FTGOR_ItemInstance_OLD> : public TStructOpsTypeTraitsBase2<FTGOR_ItemInstance_OLD>
{
	enum
	{
		WithNetSerializer = true
	};
};