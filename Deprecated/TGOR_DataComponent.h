// The Gateway of Realities: Planes of Existence.

#pragma once

#include "Realities/Base/TGOR_Object.h"
#include "Realities/Components/TGOR_Component.h"
#include "TGOR_DataComponent.generated.h"

class UTGOR_EquipableAction;

USTRUCT()
struct REALITIES_API FTGOR_EventParamContext
{
	GENERATED_USTRUCT_BODY()

	/** Returns the serialization data, must always be overridden */
	virtual UScriptStruct* GetScriptStruct() const
	{
		return FTGOR_EventParamContext::StaticStruct();
	}

	/** Returns a debug string representation */
	virtual FString ToString() const { return "None"; };
};

USTRUCT()
struct REALITIES_API FTGOR_TestEventParamContext : public FTGOR_EventParamContext
{
	GENERATED_USTRUCT_BODY()

	/** Returns the serialization data, must always be overridden */
	virtual UScriptStruct* GetScriptStruct() const override
	{
		return FTGOR_TestEventParamContext::StaticStruct();
	}

	/** Returns a debug string representation */
	virtual FString ToString() const override { return "Test"; };

	UPROPERTY()
		int32 TestIndex;
};

USTRUCT()
struct REALITIES_API FTGOR_TestWithSerialEventParamContext : public FTGOR_EventParamContext
{
	GENERATED_USTRUCT_BODY()

	/** Returns the serialization data, must always be overridden */
	virtual UScriptStruct* GetScriptStruct() const override
	{
		return FTGOR_TestWithSerialEventParamContext::StaticStruct();
	}

	/** Returns a debug string representation */
	virtual FString ToString() const override { return "Test"; };

	UPROPERTY()
		float TestFloat;

	bool NetSerialize(FArchive& Ar, class UPackageMap* Map, bool& bOutSuccess)
	{
		Ar << TestFloat;
		return bOutSuccess = true;
	}
};

template<>
struct TStructOpsTypeTraits<FTGOR_TestWithSerialEventParamContext> : public TStructOpsTypeTraitsBase2<FTGOR_TestWithSerialEventParamContext>
{
	enum
	{
		WithNetSerializer = true,
	};
};


USTRUCT(BlueprintType)
struct REALITIES_API FTGOR_EventParamContextHandle
{
	GENERATED_USTRUCT_BODY()

	FTGOR_EventParamContextHandle() : Data(nullptr) {}
	virtual ~FTGOR_EventParamContextHandle() {}

	/** Constructs from an existing context, should be allocated by new */
	explicit FTGOR_EventParamContextHandle(FTGOR_EventParamContext* DataPtr)
	{
		Data = TSharedPtr<FTGOR_EventParamContext>(DataPtr);
	}

	void operator=(FTGOR_EventParamContext* DataPtr)
	{
		Data = TSharedPtr<FTGOR_EventParamContext>(DataPtr);
	}

	FTGOR_EventParamContextHandle(FTGOR_EventParamContextHandle&& Other) : Data(MoveTemp(Other.Data)) { }
	FTGOR_EventParamContextHandle(const FTGOR_EventParamContextHandle& Other) : Data(Other.Data) { }

	FTGOR_EventParamContextHandle& operator=(FTGOR_EventParamContextHandle&& Other) { Data = MoveTemp(Other.Data); return *this; }
	FTGOR_EventParamContextHandle& operator=(const FTGOR_EventParamContextHandle& Other) { Data = Other.Data; return *this; }

	void Clear();
	bool IsValid() const;

	template<typename T>
	static FTGOR_EventParamContextHandle Create()
	{
		return FTGOR_EventParamContextHandle(new T());
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
	bool operator==(FTGOR_EventParamContextHandle const& Other) const;
	bool operator!=(FTGOR_EventParamContextHandle const& Other) const;

	/** Custom serializer, handles polymorphism of context */
	bool NetSerialize(FArchive& Ar, class UPackageMap* Map, bool& bOutSuccess);

private:
	TSharedPtr<FTGOR_EventParamContext> Data;
};

template<>
struct TStructOpsTypeTraits<FTGOR_EventParamContextHandle> : public TStructOpsTypeTraitsBase2<FTGOR_EventParamContextHandle>
{
	enum
	{
		WithCopy = true, // Necessary so that TSharedPtr<FGameplayEffectContext> Data is copied around
		WithNetSerializer = true,
		WithIdenticalViaEquality = true,
	};
};





UCLASS(BlueprintType, Blueprintable)
class REALITIES_API UTGOR_TestRepData : public UTGOR_Object
{
	GENERATED_BODY()

public:
	UTGOR_TestRepData();

	void GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const override;

	bool IsSupportedForNetworking() const override;
	bool CallRemoteFunction(UFunction* Function, void* Parms, FOutParmRec* OutParms, FFrame* Stack) override;
	int32 GetFunctionCallspace(UFunction* Function, FFrame* Stack) override;


	UFUNCTION(Reliable, Server, WithValidation, BlueprintCallable, Category = "!TGOR Action|Internal", Meta = (Keywords = "C++"))
		void ServerCall(int32 Identifier);
	virtual void ServerCall_Implementation(int32 Identifier);
	virtual bool ServerCall_Validate(int32 Identifier);

	UFUNCTION(BlueprintCallable, Reliable, NetMulticast, BlueprintCallable, Category = "!TGOR Action|Internal", Meta = (Keywords = "C++"))
		void ClientCall(int32 Identifier);
	virtual void ClientCall_Implementation(int32 Identifier);

	UFUNCTION(BlueprintCallable, Reliable, NetMulticast, BlueprintCallable, Category = "!TGOR Action|Internal", Meta = (Keywords = "C++"))
		void ClientParamCall(FTGOR_EventParamContextHandle Param);
	virtual void ClientParamCall_Implementation(FTGOR_EventParamContextHandle Param);


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated, Category = "!TGOR System")
		int32 Thing;

};

UCLASS(BlueprintType)
class REALITIES_API UTGOR_PrimaryTestRepData : public UTGOR_TestRepData
{
	GENERATED_BODY()

public:
	UTGOR_PrimaryTestRepData();

	void GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated, Category = "!TGOR System")
		int32 Stuff;

};

UCLASS(BlueprintType)
class REALITIES_API UTGOR_SecondaryTestRepData : public UTGOR_TestRepData
{
	GENERATED_BODY()

public:
	UTGOR_SecondaryTestRepData();

	void GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated, Category = "!TGOR System")
		int32 Barks;

};

/**
* TGOR_DataComponent
*/
UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class REALITIES_API UTGOR_DataComponent : public UTGOR_Component
{
	GENERATED_BODY()

public:	
	UTGOR_DataComponent();
	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	void GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const override;

	virtual void GetSubobjectsWithStableNamesForNetworking(TArray<UObject*>& Objs) override;
	bool ReplicateSubobjects(class UActorChannel* Channel, class FOutBunch* Bunch, FReplicationFlags* RepFlags) override;

	/** Creates test function params */
	UFUNCTION(BlueprintPure, Category = "!TGOR Storage", Meta = (Keywords = "C++"))
		FTGOR_EventParamContextHandle CreateParams(float Param) const;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated, Category = "!TGOR System")
		UTGOR_TestRepData*	SubobjectData;

	UFUNCTION(BlueprintCallable, Reliable, NetMulticast, BlueprintCallable, Category = "!TGOR Action|Internal", Meta = (Keywords = "C++"))
		void ClientParamCall(UTGOR_TestRepData* Param);
	virtual void ClientParamCall_Implementation(UTGOR_TestRepData* Param);
};