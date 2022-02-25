// The Gateway of Realities: Planes of Existence.


#include "TGOR_Command.h"

#include "Realities/Base/TGOR_Singleton.h"
#include "Realities/Base/Content/TGOR_ContentManager.h"
#include "Realities/Base/Content/TGOR_Content.h"
#include "Realities/Base/Controller/TGOR_OnlineController.h"
#include "Realities/Components/Inventory/TGOR_ContainerComponent.h"
#include "Realities/Actors/Pawns/TGOR_Pawn.h"
#include "Serialization/JsonSerializerMacros.h"
#include "Internationalization/Regex.h"

#include "Realities/Mod/Actions/TGOR_Action.h"
#include "Realities/Mod/Spawner/Items/TGOR_Item.h"
#include "Realities/Mod/Channels/TGOR_Channel.h"

#define LOCTEXT_NAMESPACE "ChatCommands"

FTGOR_CommandNumericArgument::FTGOR_CommandNumericArgument()
{
	IsInteger = true;
	Min = 0.0f;
	Max = 1.0f;
}

FTGOR_CommandStringArgument::FTGOR_CommandStringArgument()
{
	IsContent = false;
	AllowAbstract = false;
	AllowNone = false;
}

FTGOR_CommandArgument::FTGOR_CommandArgument()
{
	IsNumeric = true;
	Name = "";
	Default = "";
}

FTGOR_CommandArg::FTGOR_CommandArg()
:	Value(0.0f),
	Index(0),
	String(""),
	Content(nullptr)
{
}

UTGOR_Command::UTGOR_Command()
	: Super()
{
	Privilege = ETGOR_Privileges::User;
	HelpText = LOCTEXT("ChatCommands_NoHelp", "No Manual has been written for this command.");
}

////////////////////////////////////////////////////////////////////////////////////////////////////

bool UTGOR_Command::CanExecuteCommand(const FTGOR_UserInstance& UserInstance) const
{
	return Privilege <= UserInstance.Privileges;
}


bool UTGOR_Command::ParseInt(FTGOR_CommandArg& CommandArg, FString& Error, const FTGOR_CommandArgument& Argument, const FString& Arg)
{
	if (Arg.IsNumeric())
	{
		int32 Int = FCString::Atoi(*Arg);
		int32 Indexes = Argument.Numeric.Index.Num();
		if (Indexes == 0)
		{
			// Pass through integer if not restricted
			CommandArg.Index = Int;
			return(true);
		}
		else
		{
			// See if index exists
			for (const int32 Index : Argument.Numeric.Index)
			{
				if (Index == Int)
				{
					CommandArg.String = FString::FromInt(Index);
					CommandArg.Index = Index;
					CommandArg.Value = (float)Index;
					CommandArg.Content = nullptr;
					return(true);
				}
			}

			Error = FString("Invalid argument '") + Arg + "' for " + Argument.Name + ".";
			return(false);
		}
	}

	Error = Argument.Name + " expects a number.";
	return(false);
}

bool UTGOR_Command::ParseFloat(FTGOR_CommandArg& CommandArg, FString& Error, const FTGOR_CommandArgument& Argument, const FString& Arg)
{
	if (Arg.IsNumeric())
	{
		float Float = FCString::Atof(*Arg);
		// Make sure float is in range
		if (Argument.Numeric.Min <= Float && Float <= Argument.Numeric.Max)
		{
			CommandArg.String = FString::SanitizeFloat(Float);
			CommandArg.Index = FMath::FloorToInt(Float);
			CommandArg.Value = Float;
			CommandArg.Content = nullptr;
			return(true);
		}

		Error = Argument.Name + " only allows arguments in between " + FString::SanitizeFloat(Argument.Numeric.Min) + " and " + FString::SanitizeFloat(Argument.Numeric.Max) + ".";
		return(false);
	}

	Error = Argument.Name + " expects a number.";
	return(false);
}

bool UTGOR_Command::ParseString(FTGOR_CommandArg& CommandArg, FString& Error, const FTGOR_CommandArgument& Argument, const FString& Arg)
{
	int32 Strings = Argument.String.Enum.Num();
	if (Strings == 0)
	{
		CommandArg.String = Arg;
		return(true);
	}
	else
	{
		// See if enum exists
		for (int32 Index = 0; Index < Argument.String.Enum.Num(); Index++)
		{
			const FString& String = Argument.String.Enum[Index];
			if (String.Equals(Arg, ESearchCase::IgnoreCase))
			{
				CommandArg.String = String;
				CommandArg.Index = Index;
				CommandArg.Value = (float)Index;
				CommandArg.Content = nullptr;
				return(true);
			}
		}

		Error = FString("Invalid argument '") + Arg + "' for " + Argument.Name + ".";
	}
	return(false);
}

bool UTGOR_Command::ParseContent(FTGOR_CommandArg& CommandArg, FString& Error, const FTGOR_CommandArgument& Argument, const FString& Arg)
{
	SINGLETON_RETCHK(false);
	UTGOR_ContentManager* ContentManager = Singleton->GetContentManager();

	TArray<UTGOR_Content*> List = ContentManager->GetTListFromType<UTGOR_Content>(Argument.String.Content);

	// See if resourceable exists
	for (UTGOR_Content* Content : List)
	{
		if (Argument.String.AllowAbstract || !Content->IsAbstract())
		{
			for (const FString& String : Content->GetAliases())
			{
				if (String.Equals(Arg, ESearchCase::IgnoreCase))
				{
					CommandArg.String = String;
					CommandArg.Index = ContentManager->GetTIndex<UTGOR_Content>(Content);
					CommandArg.Value = (float)CommandArg.Index;
					CommandArg.Content = Content;
					return(true);
				}
			}
		}
	}

	if (Argument.String.AllowNone && (Arg.IsEmpty() || FString("None").Equals(Arg, ESearchCase::IgnoreCase)))
	{
		CommandArg.String = "";
		CommandArg.Index = -1;
		CommandArg.Value = 0.0f;
		CommandArg.Content = nullptr;
	}

	Error = Arg + " couldn't be found for " + Argument.Name + ".";
	return(false);
}

bool UTGOR_Command::ParseArgument(FTGOR_CommandArg& CommandArg, FString& Error, const FTGOR_CommandArgument& Argument, const FString& Arg)
{
	if (Argument.IsNumeric)
	{
		if (Argument.Numeric.IsInteger)
		{
			if (!ParseInt(CommandArg, Error, Argument, Arg))
			{
				return(false);
			}
		}
		else
		{
			if (!ParseFloat(CommandArg, Error, Argument, Arg))
			{
				return(false);
			}
		}
	}
	else
	{
		if (Argument.String.IsContent)
		{
			if (!ParseContent(CommandArg, Error, Argument, Arg))
			{
				return(false);
			}
		}
		else
		{
			if (!ParseString(CommandArg, Error, Argument, Arg))
			{
				return(false);
			}
		}
	}
	return(true);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

bool UTGOR_Command::Call(FString& Error, ATGOR_Pawn* Pawn, ATGOR_OnlineController* Controller, const FTGOR_UserInstance& UserInstance, const FString& Args)
{
	// Doesn't support string literals
	//int32 Size = Args.ParseIntoArray(Parsed, L" ", true);

	// Split into arguments
	FJsonSerializableArray Parsed;
	FString Current = Args;
	while (!Current.IsEmpty())
	{
		// Look for string delimiters
		char Del = Current[0];
		if (Del == '\"' || Del == '\'')
		{
			Current = Current.RightChop(1);
		}
		else
		{
			Del = ' ';
		}

		// Split string at next delimiter
		int32 Index = 0;
		if (Current.FindChar(Del, Index))
		{
			Parsed.Emplace(Current.Left(Index));
			Current = Current.RightChop(Index + 1);

			// Make sure first charachter isn't a whitespace
			if (Current[0] == ' ')
			{
				Current = Current.RightChop(1);
			}
		}
		else
		{
			Parsed.Emplace(Current);
			Current = "";
		}
	}

	// Process all arguments
	TMap<FString, FTGOR_CommandArg> CommandArgs;
	int32 Num = Arguments.Num();
	if (Num >= Parsed.Num())
	{
		for (int i = 0; i < Num; i++)
		{
			FTGOR_CommandArg CommandArg;
			const FTGOR_CommandArgument& Argument = Arguments[i];

			// Use default arguments if too few were provided
			FString Arg = Argument.Default;
			if (i < Parsed.Num())
			{
				Arg = Parsed[i];
			}

			if (!ParseArgument(CommandArg, Error, Argument, Arg))
			{
				return(false);
			}
			
			CommandArgs.Add(Argument.Name, CommandArg);
		}

		return(Command(Error, Pawn, Controller, UserInstance, CommandArgs));
	}
	return(false);
}

////////////////////////////////////////////////////////////////////////////////////////////////////


void UTGOR_Command::AutoCompleteInt(TArray<FString>& Out, const FTGOR_CommandArgument& Argument, const FString& Arg)
{
	if (Arg.IsNumeric() || Arg.IsEmpty())
	{
		int32 Int = FCString::Atoi(*Arg);
		int32 Indexes = Argument.Numeric.Index.Num();
		if (Indexes == 0)
		{
			Out.Add(FString::FromInt(Int));
			Out.Add(FString::FromInt(0));
			Out.Add(FString::FromInt(1));
		}
		else
		{
			// See if index exists
			for (const int32 Index : Argument.Numeric.Index)
			{
				if (Arg.IsEmpty() || Index == Int)
				{
					Out.Add(FString::FromInt(Index));
				}
			}
			return;
		}
	}
}

void UTGOR_Command::AutoCompleteFloat(TArray<FString>& Out, const FTGOR_CommandArgument& Argument, const FString& Arg)
{
	if (Arg.IsNumeric())
	{
		float Float = FCString::Atof(*Arg);
		// Make sure float is in range
		Float = FMath::Clamp(Float, Argument.Numeric.Min, Argument.Numeric.Max);
		Out.Add(FString::SanitizeFloat(Float));
	}
	Out.Add(FString::SanitizeFloat(Argument.Numeric.Min));
	Out.Add(FString::SanitizeFloat(Argument.Numeric.Max));
}

void UTGOR_Command::AutoCompleteString(TArray<FString>& Out, const FTGOR_CommandArgument& Argument, const FString& Arg)
{
	int32 Strings = Argument.String.Enum.Num();
	if (Strings == 0)
	{
		Out.Add(Arg);
	}
	else
	{
		// See if enum exists
		for (const FString& String : Argument.String.Enum)
		{
			if (Arg.IsEmpty() || String.StartsWith(Arg, ESearchCase::IgnoreCase))
			{
				Out.Add(String);
			}
		}
	}
}

void UTGOR_Command::AutoCompleteContent(TArray<FString>& Out, const FTGOR_CommandArgument& Argument, const FString& Arg)
{
	SINGLETON_CHK;
	UTGOR_ContentManager* ContentManager = Singleton->GetContentManager();
	TArray<UTGOR_Content*> List = ContentManager->GetTListFromType<UTGOR_Content>(Argument.String.Content);

	// See if resourceable exists
	for (UTGOR_Content* Content : List)
	{
		if (Argument.String.AllowAbstract || !Content->IsAbstract())
		{
			for (const FString& String : Content->GetAliases())
			{
				if (Arg.IsEmpty() || String.StartsWith(Arg, ESearchCase::IgnoreCase))
				{
					Out.Add(String);
				}
			}
		}
	}

	if (Argument.String.AllowNone && (Arg.IsEmpty() || FString("None").StartsWith(Arg, ESearchCase::IgnoreCase)))
	{
		Out.Add("None");
	}
}

void UTGOR_Command::AutoCompleteArgument(TArray<FString>& Out, const FTGOR_CommandArgument& Argument, const FString& Arg)
{
	if (Argument.IsNumeric)
	{
		if (Argument.Numeric.IsInteger)
		{
			AutoCompleteInt(Out, Argument, Arg);
		}
		else
		{
			AutoCompleteFloat(Out, Argument, Arg);
		}
	}
	else
	{
		if (Argument.String.IsContent)
		{
			AutoCompleteContent(Out, Argument, Arg);
		}
		else
		{
			AutoCompleteString(Out, Argument, Arg);
		}
	}
}


TArray<FString> UTGOR_Command::AutoComplete(const FString& Args, FString& Prefix)
{
	TArray<FString> Out;
	Prefix = "";

	// Get all arguments
	FJsonSerializableArray Parsed;
	int32 Size = Args.ParseIntoArray(Parsed, L" ", false);

	// Get num of arguments
	TMap<FString, FTGOR_CommandArg> CommandArgs;
	int32 Num = Arguments.Num();
	int32 Min = FMath::Min(Num, Size);

	// Process all arguments but the last one
	for (int i = 0; i < Min - 1; i++)
	{
		FString Error;
		FTGOR_CommandArg CommandArg;
		const FTGOR_CommandArgument& Argument = Arguments[i];
		const FString Arg = Parsed[i];

		if (!ParseArgument(CommandArg, Error, Argument, Arg))
		{
			AutoCompleteArgument(Out, Argument, L"");
			return(Out);
		}

		Prefix += Arg + " ";
	}

	// If all previous arguments were correct, autocomplete last
	if (Min > 0)
	{
		const int32 Last = Min - 1;
		const FTGOR_CommandArgument& Argument = Arguments[Last];
		const FString Arg = Parsed[Last];
		AutoCompleteArgument(Out, Argument, Arg);
	}
	else if(Num > 0)
	{
		const FTGOR_CommandArgument& Argument = Arguments[0];
		AutoCompleteArgument(Out, Argument, L"");
	}
	else
	{
		Out.Add("");
	}
	
	return(Out);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

/*void UTGOR_Command::SpawnItem(ATGOR_Pawn* Pawn, int32 Index, float Quantity, ETGOR_FetchEnumeration& Branches)
{
	Branches = ETGOR_FetchEnumeration::Empty;

	// Make sure quantity is valid
	if (Quantity < SMALL_NUMBER)
	{
		return;
	}

	SINGLETON_CHK

	// Get item class to check if valid
	UTGOR_Item* Item = Singleton->ContentManager->GetTFromIndex<UTGOR_Item>(Index);
	if(IsValid(Item))
	{
		// Get any container class to spawn item in
		UTGOR_ContainerComponent* Container = Cast<UTGOR_ContainerComponent>(Pawn->GetComponentByClass(UTGOR_ContainerComponent::StaticClass()));
		if (IsValid(Container))
		{
			// Actually spawn item
			FTGOR_ItemInstance_OLD Instance;
			Instance.Content = Item;
			Instance.Quantity = Quantity;

			FVector Location = Pawn->GetActorLocation();
			Container->DropItem(Instance, Location);
		}
	}
}*/

////////////////////////////////////////////////////////////////////////////////////////////////////

float UTGOR_Command::FetchFloat(const TMap<FString, FTGOR_CommandArg>& Args, const FString& Name)
{
	const FTGOR_CommandArg* Arg = Args.Find(Name);
	if (Arg)
	{
		return(Arg->Value);
	}
	return(-1.0f);
}

FString UTGOR_Command::FetchString(const TMap<FString, FTGOR_CommandArg>& Args, const FString& Name)
{
	const FTGOR_CommandArg* Arg = Args.Find(Name);
	if (Arg)
	{
		return(Arg->String);
	}
	return("");
}

int32 UTGOR_Command::FetchIndex(const TMap<FString, FTGOR_CommandArg>& Args, const FString& Name)
{
	const FTGOR_CommandArg* Arg = Args.Find(Name);
	if (Arg)
	{
		return(Arg->Index);
	}
	return(-1);
}

UTGOR_Content* UTGOR_Command::FetchContent(const TMap<FString, FTGOR_CommandArg>& Args, const FString& Name)
{
	const FTGOR_CommandArg* Arg = Args.Find(Name);
	if (Arg)
	{
		return(Arg->Content);
	}
	return(nullptr);
}


#undef LOCTEXT_NAMESPACE