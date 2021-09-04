// The Gateway of Realities: Planes of Existence.

#pragma once

#include "../TGOR_UserInstance.h"

#include "CoreSystem/Content/TGOR_CoreContent.h"
#include "TGOR_Command.generated.h"

class UTGOR_CoreContent;
class ATGOR_OnlineController;

/**
*
*/
USTRUCT(BlueprintType)
struct PLAYERSYSTEM_API FTGOR_CommandNumericArgument
{
	GENERATED_USTRUCT_BODY()
		FTGOR_CommandNumericArgument();

	////////////////////////////////////////////////////////////////////////////////////////////////////
	
	/** Can only input whole numbers */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "!TGOR Commands")
		bool IsInteger;

	/** Min float value */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "!TGOR Commands", meta = (EditCondition = "!IsInteger"))
		float Min;

	/** Max float value */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "!TGOR Commands", meta = (EditCondition = "!IsInteger"))
		float Max;

	/** Possible indexes (allows all if empty) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "!TGOR Commands", meta = (EditCondition = "IsInteger"))
		TArray<int32> Index;
};

/**
*
*/
USTRUCT(BlueprintType)
struct PLAYERSYSTEM_API FTGOR_CommandStringArgument
{
	GENERATED_USTRUCT_BODY()
		FTGOR_CommandStringArgument();

	////////////////////////////////////////////////////////////////////////////////////////////////////
	
	/** Can only input resourceable names */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "!TGOR Commands")
		bool IsContent;

	/** Possible string if type (allows all strings if empty) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "!TGOR Commands", meta = (EditCondition = "!IsContent"))
		TArray<FString> Enum;

	/** Possible content class */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "!TGOR Commands", meta = (EditCondition = "IsContent"))
		TSubclassOf<UTGOR_CoreContent> Content;

	/** Whether abstract content classes can be selected */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "!TGOR Commands", meta = (EditCondition = "IsContent"))
		bool AllowAbstract;

	/** Whether no content classes can be selected */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "!TGOR Commands", meta = (EditCondition = "IsContent"))
		bool AllowNone;
};

/**
* 
*/
USTRUCT(BlueprintType)
struct PLAYERSYSTEM_API FTGOR_CommandArgument
{
	GENERATED_USTRUCT_BODY()
		FTGOR_CommandArgument();

	////////////////////////////////////////////////////////////////////////////////////////////////////

	/** Argument name */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "!TGOR Commands")
		FString Name;

	/** Argument type */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "!TGOR Commands")
		bool IsNumeric;

	/** Default value (if too few arguments were passed) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "!TGOR Commands")
		FString Default;

	/** Numeric arguments */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "!TGOR Commands", meta = (EditCondition = "IsNumeric"))
		FTGOR_CommandNumericArgument Numeric;

	/** String arguments */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "!TGOR Commands", meta = (EditCondition = "!IsNumeric"))
		FTGOR_CommandStringArgument String;
};

/**
*
*/
USTRUCT(BlueprintType)
struct PLAYERSYSTEM_API FTGOR_CommandArg
{
	GENERATED_USTRUCT_BODY()
		FTGOR_CommandArg();

	////////////////////////////////////////////////////////////////////////////////////////////////////

	/** Float value */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "!TGOR Commands")
		float Value;

	/** Integer value */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "!TGOR Commands")
		int32 Index;

	/** String value */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "!TGOR Commands")
		FString String;

	/** Content value */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "!TGOR Commands")
		UTGOR_CoreContent* Content;
};

/**
 * 
 */
UCLASS(Blueprintable)
class PLAYERSYSTEM_API UTGOR_Command : public UTGOR_CoreContent
{
	GENERATED_BODY()
	
public:
	UTGOR_Command();

	////////////////////////////////////////////////////////////////////////////////////////////////////
protected:

	/** Minimum priviledge */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "!TGOR Commands")
		ETGOR_Privileges Privilege;

	/** Command arguments */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "!TGOR Commands")
		TArray<FTGOR_CommandArgument> Arguments;

	/** Manual text to be displayed on help */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "!TGOR Chat")
		FText HelpText;

	////////////////////////////////////////////////////////////////////////////////////////////////////
public:

	/** Find whether a user can execute this command */
	UFUNCTION(BlueprintPure, Category = "!TGOR Commands", Meta = (Keywords = "C++"))
		bool CanExecuteCommand(const FTGOR_UserInstance& UserInstance) const;

	/** Parses an integer */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Commands", Meta = (Keywords = "C++"))
		bool ParseInt(FTGOR_CommandArg& CommandArg, FString& Error, const FTGOR_CommandArgument& Argument, const FString& Arg);

	/** Parses a float */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Commands", Meta = (Keywords = "C++"))
		bool ParseFloat(FTGOR_CommandArg& CommandArg, FString& Error, const FTGOR_CommandArgument& Argument, const FString& Arg);

	/** Parses a string */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Commands", Meta = (Keywords = "C++"))
		bool ParseString(FTGOR_CommandArg& CommandArg, FString& Error, const FTGOR_CommandArgument& Argument, const FString& Arg);

	/** Parses a resourceable */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Commands", Meta = (Keywords = "C++"))
		bool ParseContent(FTGOR_CommandArg& CommandArg, FString& Error, const FTGOR_CommandArgument& Argument, const FString& Arg);

	/** Parses an argument */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Commands", Meta = (Keywords = "C++"))
		bool ParseArgument(FTGOR_CommandArg& CommandArg, FString& Error, const FTGOR_CommandArgument& Argument, const FString& Arg);

	////////////////////////////////////////////////////////////////////////////////////////////////////

	/** Call command */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Commands", Meta = (Keywords = "C++"))
		bool Call(FString& Error, ATGOR_Pawn* Pawn, ATGOR_OnlineController* Controller, const FTGOR_UserInstance& UserInstance, const FString& Args);

	/** Execute command, return true if successful */
	UFUNCTION(BlueprintImplementableEvent, Category = "!TGOR Commands", Meta = (Keywords = "C++"))
		bool Command(FString& Error, ATGOR_Pawn* Pawn, ATGOR_OnlineController* Controller, const FTGOR_UserInstance& UserInstance, const TMap<FString, FTGOR_CommandArg>& Args);

	////////////////////////////////////////////////////////////////////////////////////////////////////


	/** Parses an integer */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Commands", Meta = (Keywords = "C++"))
		void AutoCompleteInt(TArray<FString>& Out, const FTGOR_CommandArgument& Argument, const FString& Arg);

	/** Parses a float */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Commands", Meta = (Keywords = "C++"))
		void AutoCompleteFloat(TArray<FString>& Out, const FTGOR_CommandArgument& Argument, const FString& Arg);

	/** Parses a string */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Commands", Meta = (Keywords = "C++"))
		void AutoCompleteString(TArray<FString>& Out, const FTGOR_CommandArgument& Argument, const FString& Arg);

	/** Parses a resourceable */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Commands", Meta = (Keywords = "C++"))
		void AutoCompleteContent(TArray<FString>& Out, const FTGOR_CommandArgument& Argument, const FString& Arg);

	/** Parses an argument */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Commands", Meta = (Keywords = "C++"))
		void AutoCompleteArgument(TArray<FString>& Out, const FTGOR_CommandArgument& Argument, const FString& Arg);

	/** Parses arguments and returns valid arguments as well as possible last arguments */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Commands", Meta = (Keywords = "C++"))
		TArray<FString> AutoComplete(const FString& Args, FString& Prefix);

	////////////////////////////////////////////////////////////////////////////////////////////////////


	/** Spawns an item by index on a controllable */
	/*UFUNCTION(BlueprintCallable, Category = "!TGOR Commands", Meta = (ExpandEnumAsExecs = "Branches", Keywords = "C++"))
		void SpawnItem(ATGOR_Pawn* Pawn, int32 Index, float Quantity, ETGOR_FetchEnumeration& Branches);*/

	////////////////////////////////////////////////////////////////////////////////////////////////////

	/** Returns float argument if found */
	UFUNCTION(BlueprintPure, Category = "!TGOR Commands", Meta = (Keywords = "C++"))
		float FetchFloat(const TMap<FString, FTGOR_CommandArg>& Args, const FString& Name);

	/** Returns string argument if found */
	UFUNCTION(BlueprintPure, Category = "!TGOR Commands", Meta = (Keywords = "C++"))
		FString FetchString(const TMap<FString, FTGOR_CommandArg>& Args, const FString& Name);

	/** Returns integer argument if found */
	UFUNCTION(BlueprintPure, Category = "!TGOR Commands", Meta = (Keywords = "C++"))
		int32 FetchIndex(const TMap<FString, FTGOR_CommandArg>& Args, const FString& Name);

	/** Returns content argument if found */
	UFUNCTION(BlueprintPure, Category = "!TGOR Commands", Meta = (Keywords = "C++"))
		UTGOR_CoreContent* FetchContent(const TMap<FString, FTGOR_CommandArg>& Args, const FString& Name);
};
