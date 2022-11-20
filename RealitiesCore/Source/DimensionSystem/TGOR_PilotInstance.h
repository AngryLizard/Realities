// The Gateway of Realities: Planes of Existence.

#pragma once

#include "CoreMinimal.h"
#include "Coresystem/Storage/TGOR_Serialisation.h"
#include "RealitiesUtility/Structures/TGOR_Index.h"
#include "RealitiesUtility/Structures/TGOR_Time.h"

#include "CoreSystem/Storage/TGOR_Serialisation.h"
#include "CoreSystem/Storage/TGOR_PackageNetwork.h"
#include "CoreSystem/Storage/TGOR_PackageGroup.h"

#include "TGOR_MobilityInstance.h"

#include "Engine/UserDefinedStruct.h"
#include "TGOR_PilotInstance.generated.h"

class UTGOR_MobilityComponent;

/**
* FTGOR_PilotState
*/
USTRUCT(BlueprintType)
struct FTGOR_PilotState
{
	GENERATED_USTRUCT_BODY()
		SERIALISE_INIT_HEADER();

	FTGOR_PilotState();
	void Send(FTGOR_NetworkWritePackage& Package, UTGOR_Singleton* Context) const;
	void Recv(FTGOR_NetworkReadPackage& Package, UTGOR_Singleton* Context);

	/** Currently active slot */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		int32 ActiveSlot;
};

template<>
struct TStructOpsTypeTraits<FTGOR_PilotState> : public TStructOpsTypeTraitsBase2<FTGOR_PilotState>
{
	enum
	{
		WithNetSerializer = true
	};
};


USTRUCT()
struct DIMENSIONSYSTEM_API FTGOR_PilotParamsBase
{
	GENERATED_USTRUCT_BODY()

	/** Returns the serialization data, must always be overridden */
	virtual UScriptStruct* GetScriptStruct() const { return FTGOR_PilotParamsBase::StaticStruct(); }

	/** Returns a debug string representation */
	virtual FString ToString() const { return "Base"; };
};


USTRUCT(BlueprintType)
struct DIMENSIONSYSTEM_API FTGOR_PilotUpdate
{
	GENERATED_USTRUCT_BODY()

	FTGOR_PilotUpdate() { }

	FTGOR_PilotUpdate(FTGOR_PilotUpdate&& Other) : State(Other.State), Data(MoveTemp(Other.Data)) { }
	FTGOR_PilotUpdate(const FTGOR_PilotUpdate& Other) : State(Other.State), Data(Other.Data) { }

	FTGOR_PilotUpdate& operator=(FTGOR_PilotUpdate&& Other) { State = Other.State; Data = MoveTemp(Other.Data); return *this; }
	FTGOR_PilotUpdate& operator=(const FTGOR_PilotUpdate& Other) { State = Other.State; Data = Other.Data; return *this; }


	/** Pilot state (active slot and timestamp) */
	FTGOR_PilotState State;

	/** Raw storage of target data, do not modify this directly */
	TArray<TSharedPtr<FTGOR_PilotParamsBase>, TInlineAllocator<1> >	Data;

	template<typename T, typename ... Args>
	void Add(Args ... args)
	{
		Data.Emplace(TSharedPtr<FTGOR_PilotParamsBase>(new T(args...)));
	}

	template<typename T>
	T* Get()
	{
		for (const TSharedPtr<FTGOR_PilotParamsBase>& Handle : Data)
		{
			if (Handle.IsValid() && Handle->GetScriptStruct() == T::StaticStruct())
			{
				return static_cast<T*>(Handle.Get());
			}
		}
		return nullptr;
	}

	template<typename T>
	const T* Get() const
	{
		for (const TSharedPtr<FTGOR_PilotParamsBase>& Handle : Data)
		{
			if (Handle.IsValid() && Handle->GetScriptStruct() == T::StaticStruct())
			{
				return static_cast<T*>(Handle.Get());
			}
		}
		return nullptr;
	}

	/** Serialize for networking, handles polymorphism */
	bool NetSerialize(FArchive& Ar, class UPackageMap* Map, bool& bOutSuccess);

	bool operator==(const FTGOR_PilotUpdate& Other) const
	{
		// Both invalid structs or both valid and Pointer compare (???) // deep comparison equality
		if (Data.Num() != Other.Data.Num())
		{
			return false;
		}
		for (int32 i = 0; i < Data.Num(); ++i)
		{
			if (Data[i].IsValid() != Other.Data[i].IsValid())
			{
				return false;
			}
			if (Data[i].Get() != Other.Data[i].Get())
			{
				return false;
			}
		}
		return true;
	}

	bool operator!=(const FTGOR_PilotUpdate& Other) const
	{
		return !(FTGOR_PilotUpdate::operator==(Other));
	}
};

template<>
struct TStructOpsTypeTraits<FTGOR_PilotUpdate> : public TStructOpsTypeTraitsBase2<FTGOR_PilotUpdate>
{
	enum
	{
		WithCopy = true, // Necessary so that TSharedPtr<FGameplayEffectContext> Data is copied around
		WithNetSerializer = true,
		WithIdenticalViaEquality = true,
	};
};


/**
*
*/
USTRUCT(BlueprintType)
struct DIMENSIONSYSTEM_API FTGOR_VisualContact
{
	GENERATED_USTRUCT_BODY()

		FTGOR_VisualContact();

	////////////////////////////////////////////////////////////////////////////////////////////////////

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FVector Location;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FVector Normal;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		bool Blocking;
};

/**
*
*/
USTRUCT(BlueprintType)
struct DIMENSIONSYSTEM_API FTGOR_MovementParent
{
	GENERATED_USTRUCT_BODY()

		FTGOR_MovementParent();

	////////////////////////////////////////////////////////////////////////////////////////////////////

	/** Parent mobility */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		UTGOR_MobilityComponent* Mobility;

	/** Parent index */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		int32 Index;
};

