// The Gateway of Realities: Planes of Existence.

#pragma once

#include "Components/ActorComponent.h"
#include "TGOR_LogComponent.generated.h"

#define ADD_COMPONENT_LOG(T, I, A) UTGOR_LogComponent::SeekLog(GetOwner(), #T, I, A)
#define ADD_ACTOR_LOG(T, I, A) UTGOR_LogComponent::SeekLog(this, #T, I, A)

/**
* TGOR_Log stores general information for a log entry
*/
USTRUCT(BlueprintType)
struct FTGOR_Log
{
	GENERATED_USTRUCT_BODY()

public:
	FTGOR_Log();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TGOR Log")
		FString Type;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TGOR Log")
		int32 Index;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TGOR Log")
		int32 Arg;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FLogDelegate, const FTGOR_Log&, Entry);


/**
* TGOR_LogRow stores all log information of one tick
*/
USTRUCT(BlueprintType)
struct FTGOR_LogRow
{
	GENERATED_USTRUCT_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TGOR Log")
		TArray<FTGOR_Log> Log;
};


/**
* TGOR_LogComponent logs information and keeps the data for a certain amount of time
*/
UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class REALITIES_API UTGOR_LogComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UTGOR_LogComponent();
	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	
	////////////////////////////////////////////////////////////////////////////////////////////////////
protected:

	/** number of ticks until log gets cleared */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "TGOR Log")
		int32 LogPersistence;

	UPROPERTY(BlueprintAssignable, Category = "TGOR Log")
		FLogDelegate OnLog;

	////////////////////////////////////////////////////////////////////////////////////////////////////
public:

	/** Clears log */
	UFUNCTION(BlueprintCallable, Category = "TGOR Log", Meta = (Keywords = "C++"))
		void ClearLog();

	/** Adds a log */
	UFUNCTION(BlueprintCallable, Category = "TGOR Log", Meta = (Keywords = "C++"))
		void AddLog(FString Type, int32 Index = -1, int32 Arg = -1);

	/** Returns true if a log exists */
	UFUNCTION(BlueprintCallable, Category = "TGOR Log", Meta = (Keywords = "C++"))
		bool HasLog(FString Type, int32 Index = -1, int32 Arg = -1);

	/** Returns true if a log exists */
	UFUNCTION(BlueprintCallable, Category = "TGOR Log", Meta = (Keywords = "C++"))
		bool HasIndex(FString Type, int32 Index = -1);

	/** Returns true if a log exists */
	UFUNCTION(BlueprintCallable, Category = "TGOR Log", Meta = (Keywords = "C++"))
		bool HasArg(FString Type, int32 Arg = -1);

	/** Adds a log if actor has a log component */
	UFUNCTION(BlueprintCallable, Category = "TGOR Log", Meta = (Keywords = "C++"))
		static void SeekLog(AActor* Actor, FString Type, int32 Index = -1, int32 Arg = -1);

	////////////////////////////////////////////////////////////////////////////////////////////////////

private:
	UPROPERTY()
		TArray<FTGOR_LogRow> Log;

	int32 _logPosition;
	
};