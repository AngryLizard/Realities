// The Gateway of Realities: Planes of Existence.

#pragma once

#include "RealitiesUtility/Structures/TGOR_Time.h"

#include "Engine/UserDefinedStruct.h"
#include "TGOR_EventInstance.generated.h"

class UTGOR_Content;


USTRUCT()
struct ACTIONSYSTEM_API FTGOR_EventParamsBase
{
	GENERATED_USTRUCT_BODY()

	/** Returns the serialization data, must always be overridden */
	virtual UScriptStruct* GetScriptStruct() const { return FTGOR_EventParamsBase::StaticStruct(); }

	/** Returns a debug string representation */
	virtual FString ToString() const { return "Base"; };
};

USTRUCT()
struct ACTIONSYSTEM_API FTGOR_EventParamsIndex : public FTGOR_EventParamsBase
{
	GENERATED_USTRUCT_BODY()
	FTGOR_EventParamsIndex() : Index(INDEX_NONE) {}
	FTGOR_EventParamsIndex(int32 Index) : Index(Index) {}

	virtual UScriptStruct* GetScriptStruct() const override { return FTGOR_EventParamsIndex::StaticStruct(); }
	virtual FString ToString() const override { return "Index"; };

	UPROPERTY()
		int32 Index;
};

USTRUCT()
struct ACTIONSYSTEM_API FTGOR_EventParamsValue : public FTGOR_EventParamsBase
{
	GENERATED_USTRUCT_BODY()
	FTGOR_EventParamsValue() : Value(0.0f) {}
	FTGOR_EventParamsValue(float Value) : Value(Value) {}

	virtual UScriptStruct* GetScriptStruct() const override { return FTGOR_EventParamsValue::StaticStruct(); }
	virtual FString ToString() const override { return "Value"; };

	UPROPERTY()
		float Value;
};

USTRUCT()
struct ACTIONSYSTEM_API FTGOR_EventParamsContent : public FTGOR_EventParamsBase
{
	GENERATED_USTRUCT_BODY()
	FTGOR_EventParamsContent() : Content(nullptr) {}
	FTGOR_EventParamsContent(UTGOR_Content* Content) : Content(Content) {}

	virtual UScriptStruct* GetScriptStruct() const override { return FTGOR_EventParamsContent::StaticStruct(); }
	virtual FString ToString() const override { return "Content"; };

	UPROPERTY()
		UTGOR_Content* Content;
};


USTRUCT(BlueprintType)
struct ACTIONSYSTEM_API FTGOR_EventInstance
{
	GENERATED_USTRUCT_BODY()

		FTGOR_EventInstance() : Data(nullptr) {}
	virtual ~FTGOR_EventInstance() {}

	/** Constructs from an existing context, should be allocated by new */
	explicit FTGOR_EventInstance(FTGOR_EventParamsBase* DataPtr)
	{
		Data = TSharedPtr<FTGOR_EventParamsBase>(DataPtr);
	}

	void operator=(FTGOR_EventParamsBase* DataPtr)
	{
		Data = TSharedPtr<FTGOR_EventParamsBase>(DataPtr);
	}

	FTGOR_EventInstance(FTGOR_EventInstance&& Other) : Data(MoveTemp(Other.Data)) { }
	FTGOR_EventInstance(const FTGOR_EventInstance& Other) : Data(Other.Data) { }

	FTGOR_EventInstance& operator=(FTGOR_EventInstance&& Other) { Data = MoveTemp(Other.Data); return *this; }
	FTGOR_EventInstance& operator=(const FTGOR_EventInstance& Other) { Data = Other.Data; return *this; }

	void Clear();
	bool IsValid() const;

	template<typename T, typename R>
	static FTGOR_EventInstance Create(R t)
	{
		return FTGOR_EventInstance(new T(t));
	}

	template<typename T>
	static FTGOR_EventInstance Create()
	{
		return FTGOR_EventInstance(new T());
	}

	template<typename T>
	T* Get()
	{
		if (IsValid())
		{
			UScriptStruct* Desired = T::StaticStruct();
			UScriptStruct* Current = Data->GetScriptStruct();

			if (Desired == Current)
			{
				return static_cast<T*>(Data.Get());
			}
		}
		return nullptr;
	}

	template<typename T>
	const T* Get() const
	{
		if (IsValid() && Data->GetScriptStruct() == T::StaticStruct())
		{
			return static_cast<const T*>(Data.Get());
		}
		return nullptr;
	}

	FString ToString() const;
	bool operator==(FTGOR_EventInstance const& Other) const;
	bool operator!=(FTGOR_EventInstance const& Other) const;

	/** Custom serializer, handles polymorphism of context */
	bool NetSerialize(FArchive& Ar, class UPackageMap* Map, bool& bOutSuccess);

private:
	TSharedPtr<FTGOR_EventParamsBase> Data;
};

template<>
struct TStructOpsTypeTraits<FTGOR_EventInstance> : public TStructOpsTypeTraitsBase2<FTGOR_EventInstance>
{
	enum
	{
		WithCopy = true, // Necessary so that TSharedPtr<FGameplayEffectContext> Data is copied around
		WithNetSerializer = true,
		WithIdenticalViaEquality = true,
	};
};
