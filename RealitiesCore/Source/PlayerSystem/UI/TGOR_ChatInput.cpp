// The Gateway of Realities: Planes of Existence.


#include "TGOR_ChatInput.h"

#include "CoreSystem/TGOR_Singleton.h"
#include "RealitiesUGC/Mod/TGOR_ContentManager.h"
#include "../Content/TGOR_Command.h"
#include "../Content/TGOR_Channel.h"
#include "PlayerSystem/Gameplay/TGOR_OnlineController.h"
#include "Internationalization/Regex.h"

UTGOR_ChatInput::UTGOR_ChatInput()
	: Super()
{
	HistoryPointer = 0;
	SystemColor = FLinearColor(1.0f, 1.0f, 0.0f, 1.0f);
	
	CurrentPrefix = L"/";
}

FString UTGOR_ChatInput::CreateUserString(const FTGOR_UserProperties& User)
{
	FString Serial = FString::FromInt(User.Serial);
	if (User.Serial < 100) Serial = FString("0") + Serial;
	if (User.Serial < 10) Serial = FString("0") + Serial;
	return(User.Name + "#" + Serial);
}

bool UTGOR_ChatInput::CompareUserString(const FString& String, const FTGOR_UserProperties& User)
{
	// Check whether the user is a subset of given string and string is a given substring of user with serial
	if (String.StartsWith(User.Name, ESearchCase::IgnoreCase))
	{
		const FString UserString = CreateUserString(User);
		if (UserString.StartsWith(String, ESearchCase::IgnoreCase))
		{
			return(true);
		}
	}
	return(false);
}

FString UTGOR_ChatInput::CreateCommandString(UTGOR_Command* Command)
{
	return(Command->GetFirstAlias());
}

FString UTGOR_ChatInput::CreateChannelString(UTGOR_Channel* Channel)
{
	return(Channel->GetFirstAlias());
}

FLinearColor UTGOR_ChatInput::CreateMessageColor(const FTGOR_ChatMessageInstance& Message)
{
	switch (Message.Domain)
	{
		case ETGOR_ChatDomainEnumeration::System:
		{
			// Get color for system calls
			return(SystemColor);
		}
		case ETGOR_ChatDomainEnumeration::Public:
		{
			if (IsValid(Message.Channel))
			{
				return(Message.Channel->GetChannelColor());
			}
			break;
		}
		case ETGOR_ChatDomainEnumeration::Private:
		{
			// Get online controller
			ATGOR_OnlineController* Controller = Cast<ATGOR_OnlineController>(GetOwningPlayer());
			if (IsValid(Controller))
			{
				// Find user with serial
				for (const FTGOR_UserInfo& Info : Controller->GetOnlineUsers())
				{
					if (Info.Properties.Serial == Message.UserSerial)
					{
						return(Info.Properties.Color);
					}
				}
			}
			break;
		}
	}
	return(FLinearColor::Black);
}

void UTGOR_ChatInput::ChangeDomainFromMessage(const FTGOR_ChatMessageInstance& Message)
{
	const FLinearColor Color = CreateMessageColor(Message);
	switch (Message.Domain)
	{
		case ETGOR_ChatDomainEnumeration::System:
		{
			if (IsValid(Message.Command))
			{
				CurrentMessage.Command = Message.Command;
				CurrentMessage.Domain = ETGOR_ChatDomainEnumeration::System;
				ChangeDomain(Message.Domain, Color, CreateCommandString(Message.Command));
				ChangeContent(Message.Message);
			}
			break;
		}
		case ETGOR_ChatDomainEnumeration::Public:
		{
			if (IsValid(Message.Channel))
			{
				CurrentMessage.Channel = Message.Channel;
				CurrentMessage.Domain = ETGOR_ChatDomainEnumeration::Public;
				ChangeDomain(Message.Domain, Color, CreateChannelString(Message.Channel));
				ChangeContent(Message.Message);
			}
			break;
		}
		case ETGOR_ChatDomainEnumeration::Private:
		{
			// Get online controller
			ATGOR_OnlineController* Controller = Cast<ATGOR_OnlineController>(GetOwningPlayer());
			if (IsValid(Controller))
			{
				// Find user with serial
				for (const FTGOR_UserInfo& Info : Controller->GetOnlineUsers())
				{
					if (Info.Properties.Serial == Message.UserSerial)
					{
						CurrentMessage.UserSerial = Message.UserSerial;
						CurrentMessage.Domain = ETGOR_ChatDomainEnumeration::Private;
						ChangeDomain(Message.Domain, Color, CreateUserString(Info.Properties));
						ChangeContent(Message.Message);
						break;
					}
				}
			}
			break;
		}
	}
}

void UTGOR_ChatInput::SwitchDomain(ETGOR_ChatDomainEnumeration Domain)
{
	CurrentMessage.Domain = Domain;
	ChangeDomainFromMessage(CurrentMessage);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void UTGOR_ChatInput::ParseChatMessage(FString& Domain, FString& Name, FString& Content, const FString& Input, ETGOR_ChatParseEnumeration& Branches)
{
	Branches = ETGOR_ChatParseEnumeration::None;

	// Save performance on empty string
	if (Input.IsEmpty())
	{
		Content = L"";
		Branches = ETGOR_ChatParseEnumeration::IsEmpty;
		return;
	}

	// Save performance if no command is being entered
	if (!Input.StartsWith(L"/"))
	{
		Content = Input;
		Branches = ETGOR_ChatParseEnumeration::PlainText;
		return;
	}

	// Parse for current state to display autocomplete if desired
	ETGOR_FetchEnumeration State;
	ParseChatContent(Domain, Name, Content, Input, State);
	if (State == ETGOR_FetchEnumeration::Found)
	{
		Branches = ETGOR_ChatParseEnumeration::Content;
		return;
	}

	ParseChatReceiver(Domain, Name, Input, State);
	if (State == ETGOR_FetchEnumeration::Found)
	{
		Branches = ETGOR_ChatParseEnumeration::Receiver;
		return;
	}

	ParseChatDomain(Domain, Input, State);
	if (State == ETGOR_FetchEnumeration::Found)
	{
		Branches = ETGOR_ChatParseEnumeration::Domain;
		return;
	}
}


void UTGOR_ChatInput::ParseChatDomain(FString& Domain, const FString& Input, ETGOR_FetchEnumeration& Branches)
{
	Branches = ETGOR_FetchEnumeration::Empty;
	const FRegexPattern Pattern("\\/(.*)");
	FRegexMatcher Matcher(Pattern, Input);

	if (Matcher.FindNext())
	{
		Branches = ETGOR_FetchEnumeration::Found;
		Domain = Matcher.GetCaptureGroup(1);
	}
}

void UTGOR_ChatInput::ParseChatReceiver(FString& Domain, FString& Name, const FString& Input, ETGOR_FetchEnumeration& Branches)
{
	Branches = ETGOR_FetchEnumeration::Empty;
	const FRegexPattern Pattern("\\/(.*)\\s(.*)");
	FRegexMatcher Matcher(Pattern, Input);

	if (Matcher.FindNext())
	{
		Branches = ETGOR_FetchEnumeration::Found;
		Domain = Matcher.GetCaptureGroup(1);
		Name = Matcher.GetCaptureGroup(2);
	}
}

void UTGOR_ChatInput::ParseChatContent(FString& Domain, FString& Name, FString& Content, const FString& Input, ETGOR_FetchEnumeration& Branches)
{
	Branches = ETGOR_FetchEnumeration::Empty;
	const FRegexPattern Pattern("\\/(.*)\\s(.*)\\s(.*)");
	FRegexMatcher Matcher(Pattern, Input);

	if (Matcher.FindNext())
	{
		Branches = ETGOR_FetchEnumeration::Found;
		Domain = Matcher.GetCaptureGroup(1);
		Name = Matcher.GetCaptureGroup(2);
		Content = Matcher.GetCaptureGroup(3);
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////


ETGOR_ChatDomainEnumeration UTGOR_ChatInput::InterpretChatDomain(const FString& Domain, ETGOR_FetchEnumeration& Branches)
{
	Branches = ETGOR_FetchEnumeration::Found;

	for (const FString& String : PrivateDomain)
	{
		if (Domain.Equals(String, ESearchCase::IgnoreCase))
		{
			return(ETGOR_ChatDomainEnumeration::Private);
		}
	}

	for (const FString& String : PublicDomain)
	{
		if (Domain.Equals(String, ESearchCase::IgnoreCase))
		{
			return(ETGOR_ChatDomainEnumeration::Public);
		}
	}

	for (const FString& String : SystemDomain)
	{
		if (Domain.Equals(String, ESearchCase::IgnoreCase))
		{
			return(ETGOR_ChatDomainEnumeration::System);
		}
	}

	Branches = ETGOR_FetchEnumeration::Empty;
	return(ETGOR_ChatDomainEnumeration::System);
}


UTGOR_Command* UTGOR_ChatInput::InterpretChatCommand(const FString& Command, ETGOR_FetchEnumeration& Branches)
{
	Branches = ETGOR_FetchEnumeration::Found;
	SINGLETON_RETCHK(nullptr);
	UTGOR_ContentManager* ContentManager = Singleton->GetContentManager();

	TArray<UTGOR_Command*> Commands = ContentManager->GetTListFromType<UTGOR_Command>();
	for (UTGOR_Command* Content : Commands)
	{
		if (!Content->IsAbstract())
		{
			for (const FString& String : Content->GetAliases())
			{
				if (String.Equals(Command, ESearchCase::IgnoreCase))
				{
					return(Content);
				}
			}
		}
	}

	Branches = ETGOR_FetchEnumeration::Empty;
	return(nullptr);
}


TArray<FString> UTGOR_ChatInput::AutoCompleteChatCommand(const FString& Command)
{
	TArray<FString> Out;
	SINGLETON_RETCHK(Out);
	UTGOR_ContentManager* ContentManager = Singleton->GetContentManager();

	TArray<UTGOR_Command*> Commands = ContentManager->GetTListFromType<UTGOR_Command>();
	for (UTGOR_Command* Content : Commands)
	{
		if (!Content->IsAbstract())
		{
			// Only add one alias if no prefix
			if (Command.IsEmpty())
			{
				Out.Add(Content->GetFirstAlias());
			}
			else
			{
				for (const FString& String : Content->GetAliases())
				{
					if (String.StartsWith(Command, ESearchCase::IgnoreCase))
					{
						Out.Add(String);
					}
				}
			}
		}
	}

	return(Out);
}


UTGOR_Channel* UTGOR_ChatInput::InterpretChatChannel(const FString& Channel, ETGOR_FetchEnumeration& Branches)
{
	Branches = ETGOR_FetchEnumeration::Found;
	SINGLETON_RETCHK(nullptr);
	UTGOR_ContentManager* ContentManager = Singleton->GetContentManager();

	TArray<UTGOR_Channel*> Channels = ContentManager->GetTListFromType<UTGOR_Channel>();
	for (UTGOR_Channel* Content : Channels)
	{
		if (!Content->IsAbstract())
		{
			for (const FString& String : Content->GetAliases())
			{
				if (String.Equals(Channel, ESearchCase::IgnoreCase))
				{
					return(Content);
				}
			}
		}
	}

	Branches = ETGOR_FetchEnumeration::Empty;
	return(nullptr);
}

TArray<FString> UTGOR_ChatInput::AutoCompleteChatChannel(const FString& Channel)
{
	TArray<FString> Out;
	SINGLETON_RETCHK(Out);
	UTGOR_ContentManager* ContentManager = Singleton->GetContentManager();

	TArray<UTGOR_Channel*> Channels = ContentManager->GetTListFromType<UTGOR_Channel>();
	for (UTGOR_Channel* Content : Channels)
	{
		if (!Content->IsAbstract())
		{
			// Only add one alias if no prefix
			if (Channel.IsEmpty())
			{
				Out.Add(Content->GetFirstAlias());
			}
			else
			{
				for (const FString& String : Content->GetAliases())
				{
					if (String.StartsWith(Channel, ESearchCase::IgnoreCase))
					{
						Out.Add(String);
					}
				}
			}
		}
	}

	return(Out);
}


FTGOR_UserProperties UTGOR_ChatInput::InterpretChatUser(const FString& User, ETGOR_FetchEnumeration& Branches)
{
	Branches = ETGOR_FetchEnumeration::Found;
	FTGOR_UserProperties Properties;

	// Get online controller
	ATGOR_OnlineController* Controller = Cast<ATGOR_OnlineController>(GetOwningPlayer());
	if (IsValid(Controller))
	{
		// Find user with name
		for (const FTGOR_UserInfo& Info : Controller->GetOnlineUsers())
		{
			if (CompareUserString(User, Info.Properties))
			{
				Properties = Info.Properties;
				return(Properties);
			}
		}
	}

	Branches = ETGOR_FetchEnumeration::Empty;
	return(Properties);
}


TArray<FString> UTGOR_ChatInput::AutoCompleteChatUser(const FString& User)
{
	TArray<FString> Properties;
	
	// Get online controller
	ATGOR_OnlineController* Controller = Cast<ATGOR_OnlineController>(GetOwningPlayer());
	if (IsValid(Controller))
	{
		// Find user with name
		for (const FTGOR_UserInfo& Info : Controller->GetOnlineUsers())
		{
			const FString UserString = CreateUserString(Info.Properties);
			if (User.IsEmpty() || UserString.StartsWith(User, ESearchCase::IgnoreCase))
			{
				Properties.Add(UserString);
			}
		}
	}

	return(Properties);
}



TArray<FString> UTGOR_ChatInput::AutoCompleteAny(const FString& String, int32 EntryLimit)
{
	TArray<FString> Out;

	// Add domains
	AppendStartingWith(Out, PrivateDomain, String);
	AppendStartingWith(Out, PublicDomain, String);
	AppendStartingWith(Out, SystemDomain, String);

	// Add commands
	TArray<FString> Commands = AutoCompleteChatCommand(String);
	AppendUnique(Out, Commands, EntryLimit);

	// Add channels
	TArray<FString> Channels = AutoCompleteChatChannel(String);
	AppendUnique(Out, Channels, EntryLimit);
	
	// Add users
	TArray<FString> Users = AutoCompleteChatUser(String);
	AppendUnique(Out, Users, EntryLimit);
	return(Out);
}


////////////////////////////////////////////////////////////////////////////////////////////////////


void UTGOR_ChatInput::AppendStartingWith(TArray<FString>& Out, const TArray<FString>& In, const FString& Prefix)
{
	// Add all with right prefix
	for (const FString& String : In)
	{
		if (Prefix.IsEmpty() || String.StartsWith(Prefix, ESearchCase::IgnoreCase))
		{
			Out.AddUnique(String);
		}
	}
}

void UTGOR_ChatInput::AppendUnique(TArray<FString>& Out, const TArray<FString>& In, int32 EntryLimit)
{
	// Add until entry limit is reached
	int32 Num = Out.Num();
	for (const FString& String : In)
	{
		if (++Num >= EntryLimit)
		{
			return;
		}
		Out.AddUnique(String);
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////


int32 UTGOR_ChatInput::GetMaxContentLength()
{
	ATGOR_OnlineController* Controller = Cast<ATGOR_OnlineController>(GetOwningPlayer());
	if (IsValid(Controller))
	{
		return Controller->GetMaxContentLength();
	}
	return(0);
}

void UTGOR_ChatInput::SendChatMessage(const FString& Content)
{
	if (!Content.IsEmpty() || CurrentMessage.Domain == ETGOR_ChatDomainEnumeration::System)
	{
		ATGOR_OnlineController* Controller = Cast<ATGOR_OnlineController>(GetOwningPlayer());
		if (IsValid(Controller))
		{
			// Build message (Sender is set by server)
			CurrentMessage.Message = Content;
			CurrentMessage.Message = Content.Left(Controller->GetMaxContentLength());

			// Send message
			Controller->SendChatMessage(CurrentMessage);

			// Add to history, set pointer at the very end (one above the valid range)
			History.Add(CurrentMessage);
			HistoryPointer = History.Num();
			ChangeContent("");
		}
		else
		{
			ERROR("Cannot get owning player OnlineController to send message with", Error);
		}
	}
}


void UTGOR_ChatInput::GoUpHistory()
{
	// Stop at last history entry
	HistoryPointer = FMath::Max(HistoryPointer - 1, 0);

	if (History.IsValidIndex(HistoryPointer))
	{
		ChangeDomainFromMessage(History[HistoryPointer]);
	}
	else
	{
		CurrentMessage.Message = L"";
		ChangeDomainFromMessage(CurrentMessage);
	}
}


void UTGOR_ChatInput::GoDownHistory()
{
	// Treat "newest" entry (one above array range) as fresh input
	HistoryPointer = FMath::Min(HistoryPointer + 1, History.Num());

	if (History.IsValidIndex(HistoryPointer))
	{
		ChangeDomainFromMessage(History[HistoryPointer]);
	}
	else
	{
		CurrentMessage.Message = L"";
		ChangeDomainFromMessage(CurrentMessage);
	}

}


void UTGOR_ChatInput::ApplyHistory()
{
	const int32 Num = History.Num();
	if (History.IsValidIndex(HistoryPointer))
	{
		CurrentMessage = History[HistoryPointer];
	}

	CurrentMessage.Message = L"";
	ChangeDomainFromMessage(CurrentMessage);
}


void UTGOR_ChatInput::FindState(const FString& Receiver, const FString& Content)
{
	CurrentMessage.Message = Content;
	ChangeContent(Content);

	// Get channel and use a defined color mapping to privileges
	ETGOR_FetchEnumeration State;
	CurrentMessage.Command = InterpretChatCommand(Receiver, State);
	if (IsValid(CurrentMessage.Command))
	{
		CurrentPrefix = L"";
		CurrentMessage.Domain = ETGOR_ChatDomainEnumeration::System;
	}
	else
	{
		CurrentPrefix = L"/";
		// Get channel and use its color
		CurrentMessage.Channel = InterpretChatChannel(Receiver, State);
		if (IsValid(CurrentMessage.Channel))
		{
			CurrentMessage.Domain = ETGOR_ChatDomainEnumeration::Public;
		}
		else
		{
			// Get player and use their color
			FTGOR_UserProperties Properties = InterpretChatUser(Receiver, State);
			if (State == ETGOR_FetchEnumeration::Found)
			{
				CurrentMessage.UserSerial = Properties.Serial;
				CurrentMessage.Domain = ETGOR_ChatDomainEnumeration::Private;
			}
		}
	}
	ChangeDomainFromMessage(CurrentMessage);
}


void UTGOR_ChatInput::ChangeState(ETGOR_ChatDomainEnumeration Domain, const FString& Receiver, const FString& Content)
{
	CurrentMessage.Domain = Domain;
	CurrentMessage.Message = Content;
	ChangeContent(Content);

	ETGOR_FetchEnumeration State;
	switch (CurrentMessage.Domain)
	{
		case ETGOR_ChatDomainEnumeration::System:
		{
			// Get channel and use a defined color mapping to privileges
			CurrentMessage.Command = InterpretChatCommand(Receiver, State);
			CurrentPrefix = L"";
			break;
		}
		case ETGOR_ChatDomainEnumeration::Public:
		{
			// Get channel and use its color
			CurrentMessage.Channel = InterpretChatChannel(Receiver, State);
			CurrentPrefix = L"/";
			break;
		}
		case ETGOR_ChatDomainEnumeration::Private :
		{
			// Get player and use his color
			FTGOR_UserProperties Properties = InterpretChatUser(Receiver, State);
			CurrentMessage.UserSerial = Properties.Serial;
			CurrentPrefix = L"/";
			break;
		}
	}
	ChangeDomainFromMessage(CurrentMessage);
}

void UTGOR_ChatInput::ResetState()
{
	CurrentMessage.Command = nullptr;
	CurrentMessage.Channel = nullptr;
	CurrentMessage.UserSerial = -1;
	CurrentPrefix = L"/";
}
