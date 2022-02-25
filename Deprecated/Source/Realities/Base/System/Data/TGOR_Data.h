// The Gateway of Realities: Planes of Existence.

#pragma once

#include "Realities/Utility/Datastructures/TGOR_ContentTable.h"
#include "Realities/Utility/TGOR_CustomEnumerations.h"
#include "Realities/Utility/Storage/TGOR_File.h"

#include "Realities/Interfaces/TGOR_SaveInterface.h"
#include "Realities/Base/TGOR_Object.h"
#include "TGOR_Data.generated.h"

#define DATA_INSTANCE_TABLE(T, N) \
HASH_TABLE(TSharedPtr<FTGOR_##T##Instance>, N)	T##Table = HASH_TABLE(TSharedPtr<FTGOR_##T##Instance>, N)(TSharedPtr<FTGOR_##T##Instance>()); \
static FTGOR_##T##Instance _default##T##Instance;

#define ENFORCE(C) if (!(C)) { return(false); }

#define DATA_INSTANCE_DEFAULT(S, N) \
FTGOR_##N##Instance S::_default##N##Instance = FTGOR_##N##Instance();

class UTGOR_Singleton;

template<typename T>
using InstanceLambda = std::function<bool(T& Instance, int32 Key)>;

template<typename T>
using ConstInstanceLambda = std::function<bool(const T& Instance, int32 Key)>;

/**
* TGOR_Data allows storing resource tables
*/
UCLASS(BlueprintType)
class REALITIES_API UTGOR_Data : public UTGOR_Object, public ITGOR_SaveInterface
{
	GENERATED_BODY()

public:
	UTGOR_Data();

	////////////////////////////////////////////////////////////////////////////////////////////////////


	/** Saves data of this world to files */
	UFUNCTION(BlueprintCallable, Category = "TGOR Dimension", Meta = (Keywords = "C++"))
		void SaveToFile();

	/** Loads data of this world from files */
	UFUNCTION(BlueprintCallable, Category = "TGOR Dimension", Meta = (Keywords = "C++"))
		void LoadFromFile();

	/** Get current filename */
	UFUNCTION(BlueprintPure, Category = "TGOR Dimension", Meta = (Keywords = "C++"))
		FString GetFilename() const;

	/** Set current filename */
	UFUNCTION(BlueprintCallable, Category = "TGOR Dimension", Meta = (Keywords = "C++"))
		void SetFilename(const FString& Name);

protected:

	UPROPERTY()
		FTGOR_File File;

	UPROPERTY(BlueprintReadOnly, Category = "TGOR Dimension")
		FString Folder;

protected:

	template<typename T, typename H>
	T& GetInstance(H& Table, T& DefaultInstance, int32 Key, ETGOR_FetchEnumeration& Branches)
	{
		Branches = ETGOR_FetchEnumeration::Empty;
		int32 Index = Table.find(Key);
		if (Index == -1) return(DefaultInstance);

		Branches = ETGOR_FetchEnumeration::Found;
		TSharedPtr<T> Ref = Table.getByIndex(Index);

		if (!Ref.IsValid()) return(DefaultInstance);
		return(*Ref);
	}


	template<typename T, typename H>
	TWeakPtr<T> GetInstanceReference(H& Table, int32 Key)
	{
		return(TWeakPtr<T>(Table.get(Key)));
	}


	template<typename T, typename H>
	int32 StoreInstance(H& Table, const int ManualInterval, const int Bits, const T& Instance)
	{
		const uint32 Max = ((2 << Bits) - 1);

		int32 Key;
		do Key = FMath::RandRange(ManualInterval, Max);
		while (Table.find(Key) != -1);

		TSharedPtr<T> Ref = TSharedPtr<T>(new T(Instance));

		int32 Index = Table.insert(Key, Ref);
		if (Index == -1) return(-1);
		return(Key);
	}


	template<typename T, typename H>
	int32 StoreInstanceAt(H& Table, int32 Key, const int Bits, const T& Instance)
	{
		const uint32 Max = ((2 << Bits) - 1);

		if (Key < 0 || Key > Max) return(-1);
		TSharedPtr<T> Ref = Table.get(Key);
		if (!Ref.IsValid())
		{
			Ref = TSharedPtr<T>(new T(Instance));
			int32 Index = Table.insert(Key, Ref);
			if (Index == -1) return(-1);
		}
		else *Ref = Instance;
		return(Key);
	}


	template<typename T, typename H>
	void RemoveInstance(H& Table, int32 Key)
	{
		if (Key == -1) return;
		TWeakPtr<T> Shared = Table.remove(Key);
		Shared.Reset();
	}


	////////////////////////////////////////////////////////////////////////////////////////////////////

	template<typename T, typename H>
	TArray<T> ListInstanceTable(H& Table)
	{
		TArray<T> Array;
		ForEachInstance<T>(Table,
			[&Array](T& Instance, int32 Key)
		{
			Array.Add(Instance);
			return(true);
		}
		);
		return(Array);
	}

	template<typename T, typename H>
	bool ForEachInstance(H& Table, InstanceLambda<T> Func)
	{
		return Table.forEach([Func](const int32& Key, TSharedPtr<T>& Ref, int32 Index) -> bool
		{
			return(Func(*Ref.Get(), Key));
		});
	}

	template<typename T, typename H>
	bool ForEachInstance(const H& Table, ConstInstanceLambda<T> Func) const
	{
		return Table.forEachConst([Func](const int32& Key, const TSharedPtr<T>& Ref, int32 Index) -> bool
		{
			return(Func(*Ref.Get(), Key));
		});
	}

	////////////////////////////////////////////////////////////////////////////////////////////////////

	template<typename T, typename H>
	void ClearInstanceTable(H& Table)
	{
		Table.clear();
	}

	/*
	template<typename T, typename H>
	bool WriteInstance(H& Table, CTGOR_FileHandlePointer& FileHandlePointer, FString Type)
	{
		TArray<T::FilePack*> Array;
		
		Table.forEachConst([&Array, Type](const int32& Key, const TSharedPtr<T>& Ref, int32 Index) -> bool
		{
			T& Instance = *Ref.Get();

			T::FilePack* Pack = new T::FilePack();
			if (Instance.Write(*Pack, Key))
			{
				Array.Add(Pack);
			}

			return(true);
		});

		CTGOR_Number Number;
		Number.Set(Array.Num());
		PageNext(Number.Size());
		if (!Number.Write(FileHandlePointer)) return(false);

		for (int i = 0; i < Array.Num(); i++)
		{
			T::FilePack* Pack = Array[i];
			PageNext(Pack->Size());
			if (!Pack->Write(FileHandlePointer))
			{
				delete(Pack);
				return(false);
			}
			delete(Pack);
		}

		return(true);
	}


	template<typename T, typename H>
	bool ReadInstance(H& Table, CTGOR_FileHandlePointer& FileHandlePointer, FString Type)
	{
		NextPage();
		CTGOR_Number Number;
		if (!Number.Read(FileHandlePointer)) { return(false); }
		if (!Number.HasType(CTGOR_Number::SIGNED)) { return(false); }
		int32 Num = Number.Get().i;

		for (int i = 0; i < Num; i++)
		{
			NextPage();
			T::FilePack Pack;
			if (Pack.Read(FileHandlePointer))
			{
				int32 Key;
				T Instance;
				if (Instance.Read(Pack, Key))
				{
					if (Key >= 0 || Key <= INT16_MAX)
					{
						TSharedPtr<T> Ref = Table.get(Key);
						if (!Ref.IsValid())
						{
							Ref = TSharedPtr<T>(new T(Instance));
							int32 Index = Table.insert(Key, Ref);
						}
						else
						{
							*Ref = Instance;
						}
					}
				}
			}
		}
		return(true);
	}

	////////////////////////////////////////////////////////////////////////////////////////////////////
public:

	bool WriteString(CTGOR_FileHandlePointer& FileHandlePointer, FString String)
	{
		// Write string to pack
		CTGOR_String Pack;
		Pack.Set(*String);

		// Write pack to file
		PageNext(Pack.Size());
		if (!Pack.Write(FileHandlePointer)) { return(false); }

		return(true);
	}

	bool ReadString(CTGOR_FileHandlePointer& FileHandlePointer, FString& String)
	{
		// Read pack from file
		NextPage();
		CTGOR_String Pack;
		if (!Pack.Read(FileHandlePointer)) { return(false); }

		// Read pack to string
		String = FString(Pack.Get());

		return(true);
	}

	bool WriteInteger(CTGOR_FileHandlePointer& FileHandlePointer, int32 Integer)
	{
		// Write number to pack
		CTGOR_Number Pack;
		Pack.Set(Integer);

		// Write pack to file
		PageNext(Pack.Size());
		if (!Pack.Write(FileHandlePointer)) { return(false); }

		return(true);
	}

	bool ReadInteger(CTGOR_FileHandlePointer& FileHandlePointer, int32& Integer)
	{
		// Read pack from file
		NextPage();
		CTGOR_Number Pack;
		if (!Pack.Read(FileHandlePointer)) { return(false); }
		if (!Pack.HasType(CTGOR_Number::SIGNED)) { return(false); }

		// Read pack to number
		Integer = Pack.Get().i;

		return(true);
	}

	template<typename T>
	bool WriteInstance(CTGOR_FileHandlePointer& FileHandlePointer, const T& Instance)
	{
		// Write instance to pack
		T::FilePack Pack;
		if (!Instance.Write(Pack)) { return(false); }

		// Write pack to file
		PageNext(Pack.Size());
		if (!Pack.Write(FileHandlePointer)) { return(false); }

		return(true);
	}

	template<typename T>
	bool ReadInstance(CTGOR_FileHandlePointer& FileHandlePointer, T& Instance)
	{
		// Read instance from file
		NextPage();
		T::FilePack Pack;
		if (!Pack.Read(FileHandlePointer)) { return(false); }
		
		// Read Instance from pack
		if (!Instance.Read(Pack)) { return(false); }

		return(true);
	}

	template<typename T>
	bool WriteInstance(CTGOR_FileHandlePointer& FileHandlePointer, UTGOR_Singleton* Singleton, const T& Instance)
	{
		// Write instance to pack
		T::FilePack Pack;
		if (!Instance.Write(Singleton, Pack)) { return(false); }

		// Write pack to file
		PageNext(Pack.Size());
		if (!Pack.Write(FileHandlePointer)) { return(false); }

		return(true);
	}

	template<typename T>
	bool ReadInstance(CTGOR_FileHandlePointer& FileHandlePointer, UTGOR_Singleton* Singleton, T& Instance)
	{
		// Read instance from file
		NextPage();
		T::FilePack Pack;
		if (!Pack.Read(FileHandlePointer)) { return(false); }

		// Read Instance from pack
		if (!Instance.Read(Singleton, Pack)) { return(false); }

		return(true);
	}
	*/
};
