// The Gateway of Realities: Planes of Existence.

#pragma once

#include "CoreMinimal.h"
#include "Templates/Casts.h"
#include "Base/System/TGOR_Buffer.h"
#include "Base/Instances/Creature/TGOR_CreatureInstance.h"

#include "Utility/Error/TGOR_Error.h"

#include "Object.h"
#include "TGOR_BufferManager.generated.h"


/**
* TGOR_BufferManager allows condensing instances into buffer (byte arrays)
*/
UCLASS()
class REALITIES_API UTGOR_BufferManager : public UObject
{
	GENERATED_BODY()
	
public:

	////////////////////////////////////////////////////////////////////////////////////////////////////

	UFUNCTION(BlueprintPure, Category = "TGOR System", Meta = (Keywords = "C++"))
		static void FillBufferFromAppearance(UTGOR_Singleton* Singleton, UPARAM(ref) FTGOR_CreatureAppearanceInstance& Appearance, FTGOR_Buffer& Buffer);
	
	UFUNCTION(BlueprintPure, Category = "TGOR System", Meta = (Keywords = "C++"))
		static void FillAppearanceFromBuffer(UTGOR_Singleton* Singleton, FTGOR_CreatureAppearanceInstance& Appearance, const FTGOR_Buffer& Buffer);

	UFUNCTION(BlueprintPure, Category = "TGOR System", Meta = (Keywords = "C++"))
		static void FillBufferFromSetup(UTGOR_Singleton* Singleton, UPARAM(ref) FTGOR_CreatureSetupInstance& Appearance, FTGOR_Buffer& Buffer);

	UFUNCTION(BlueprintPure, Category = "TGOR System", Meta = (Keywords = "C++"))
		static void FillSetupFromBuffer(UTGOR_Singleton* Singleton, FTGOR_CreatureSetupInstance& Appearance, const FTGOR_Buffer& Buffer);

	////////////////////////////////////////////////////////////////////////////////////////////////////

	/*UFUNCTION(BlueprintPure, Category = "TGOR Buffer", Meta = (Keywords = "C++"))
		static void FillBufferFromInventory(UTGOR_Singleton* Singleton, UPARAM(ref) FTGOR_InventoryInstance& Inventory, FTGOR_Buffer& Buffer);
	
	UFUNCTION(BlueprintPure, Category = "TGOR Buffer", Meta = (Keywords = "C++"))
		static void FillInventoryFromBuffer(UTGOR_Singleton* Singleton, FTGOR_InventoryInstance& Inventory, const FTGOR_Buffer& Buffer);

	
	UFUNCTION(BlueprintPure, Category = "TGOR Buffer", Meta = (Keywords = "C++"))
		static void FillBufferFromInstance(UTGOR_Singleton* Singleton, UPARAM(ref) FTGOR_ItemInstance_OLD& Instance, FTGOR_Buffer& Buffer);
	
	UFUNCTION(BlueprintPure, Category = "TGOR Buffer", Meta = (Keywords = "C++"))
		static void FillInstanceFromBuffer(UTGOR_Singleton* Singleton, FTGOR_ItemInstance_OLD& Instance, const FTGOR_Buffer& Buffer);*/

	////////////////////////////////////////////////////////////////////////////////////////////////////
	
	template<typename T>
	static void FillBufferFromIO(UTGOR_Singleton* Singleton, const T& t, FTGOR_Buffer& Buffer)
	{
		if (!IsValid(Singleton))
		{
			ERROR("No singleton provided", Error);
		}

		/*
		// Write to instance pack
		T::FilePack InstancePack;
		t.Write(Singleton, InstancePack);

		// Retreive buffer
		FTGOR_Buffer::FilePack BufferPack;
		CTGOR_Buffer& Pack = BufferPack.Pack<1>();
		BufferPack.Pack<0>().Set((uint8)Buffer.Type);

		// Write to pack
		Pack.Set(InstancePack.Size());
		if (!InstancePack.Write(Pack))
		{
			ERROR(FString("Writing instance failed for ") + T::StaticStruct()->GetName(), Error);
		}

		// Fill buffer from pack
		if (!Buffer.Read(BufferPack))
		{
			ERROR(FString("Reading buffer failed for ") + T::StaticStruct()->GetName(), Error);
		}
		*/
	}

	template<typename T>
	static void FillIOFromBuffer(UTGOR_Singleton* Singleton, T& t, const FTGOR_Buffer& Buffer)
	{
		if (!IsValid(Singleton))
		{
			ERROR(FString("No singleton provided for ") + T::StaticStruct()->GetName(), Error);
		}

		/*
		// Retreive buffer
		FTGOR_Buffer::FilePack BufferPack;
		Buffer.Write(BufferPack);
		CTGOR_Buffer& Pack = BufferPack.Pack<1>();

		// Read from pack
		Pack.reset();
		T::FilePack InstancePack;
		if (!InstancePack.Read(Pack))
		{
			ERROR(FString("Reading instance failed for ") + T::StaticStruct()->GetName(), Error);
		}

		// Write into instance
		if (!t.Read(Singleton, InstancePack))
		{
			ERROR(FString("Reading buffer failed for ") + T::StaticStruct()->GetName(), Error);
		}
		*/
	}


	template<typename T>
	static void FillBufferFromPackage(const T& t, FTGOR_Buffer& Buffer)
	{
		/*
		// Retreive buffer
		FTGOR_Buffer::FilePack BufferPack;
		CTGOR_Buffer& Pack = BufferPack.Pack<1>();
		BufferPack.Pack<0>().Set((uint8)Buffer.Type);

		// Fill buffer from pack
		if (!Buffer.Read(BufferPack))
		{
			ERROR(FString("Reading buffer failed for pack"), Error);
		}
		*/
	}

	template<typename T>
	static void FillPackageFromBuffer(T& t, const FTGOR_Buffer& Buffer)
	{
		/*
		// Retreive buffer
		FTGOR_Buffer::FilePack BufferPack;
		Buffer.Write(BufferPack);
		CTGOR_Buffer& Pack = BufferPack.Pack<1>();

		// Read from pack
		Pack.reset();
		if (!t.Read(Pack))
		{
			ERROR(FString("Reading pack failed for pack"), Error);
		}
		*/
	}
};
