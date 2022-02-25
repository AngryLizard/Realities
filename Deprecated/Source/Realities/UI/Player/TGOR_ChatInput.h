// The Gateway of Realities: Planes of Existence.

#pragma once

#include "CoreMinimal.h"
#include "Realities/Base/Controller/TGOR_OnlineController.h"
#include "Realities/Base/Instances/Player/TGOR_UserInstance.h"

#include "Realities/UI/TGOR_Widget.h"
#include "TGOR_ChatInput.generated.h"

class UTGOR_Command;
class UTGOR_Channel;


UENUM(BlueprintType)
enum class ETGOR_ChatParseEnumeration : uint8
{
	None,
	IsEmpty,
	PlainText,
	Domain,
	Receiver,
	Content
};


/**
 * 
 */
UCLASS()
class REALITIES_API UTGOR_ChatInput : public UTGOR_Widget
{
	GENERATED_BODY()
	
public:
	UTGOR_ChatInput();

	/** Changes chat domain */
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "TGOR Chat", Meta = (Keywords = "C++"))
		void ChangeDomain(ETGOR_ChatDomainEnumeration Domain, const FLinearColor& Colour, const FString& Subdomain);

	/** Changes chat content */
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "TGOR Chat", Meta = (Keywords = "C++"))
		void ChangeContent(const FString& Content);
	
	////////////////////////////////////////////////////////////////////////////////////////////////////

	/** Create name string from a given user Discord style bob#0001*/
	UFUNCTION(BlueprintPure, Category = "TGOR Chat", Meta = (Keywords = "C++"))
		FString CreateUserString(const FTGOR_UserProperties& User);

	/** Checks whether a string matches a user */
	UFUNCTION(BlueprintPure, Category = "TGOR Chat", Meta = (Keywords = "C++"))
		bool CompareUserString(const FString& String, const FTGOR_UserProperties& User);


	/** Creates receiver string from given command */
	UFUNCTION(BlueprintPure, Category = "TGOR Chat", Meta = (Keywords = "C++"))
		FString CreateCommandString(UTGOR_Command* Command);

	/** Creates receiver string from given channel */
	UFUNCTION(BlueprintPure, Category = "TGOR Chat", Meta = (Keywords = "C++"))
		FString CreateChannelString(UTGOR_Channel* Channel);

	/** Creates message colour from a given message */
	UFUNCTION(BlueprintPure, Category = "TGOR Chat", Meta = (Keywords = "C++"))
		FLinearColor CreateMessageColour(const FTGOR_ChatMessageInstance& Message);

	/** Changes chat domain given a message */
	UFUNCTION(BlueprintCallable, Category = "TGOR Chat", Meta = (Keywords = "C++"))
		void ChangeDomainFromMessage(const FTGOR_ChatMessageInstance& Message);

	/** Changes chat domain given a domain */
	UFUNCTION(BlueprintCallable, Category = "TGOR Chat", Meta = (Keywords = "C++"))
		void SwitchDomain(ETGOR_ChatDomainEnumeration Domain);

	////////////////////////////////////////////////////////////////////////////////////////////////////

	/** Parse message for Domain, name and content */
	UFUNCTION(BlueprintCallable, Category = "TGOR Chat", Meta = (ExpandEnumAsExecs = "Branches", Keywords = "C++"))
		void ParseChatMessage(FString& Domain, FString& Name, FString& Content, const FString& Input, ETGOR_ChatParseEnumeration& Branches);

	/** Parse message for Receiver */
	UFUNCTION(BlueprintCallable, Category = "TGOR Chat", Meta = (ExpandEnumAsExecs = "Branches", Keywords = "C++"))
		void ParseChatDomain(FString& Domain, const FString& Input, ETGOR_FetchEnumeration& Branches);

	/** Parse message for Receiver */
	UFUNCTION(BlueprintCallable, Category = "TGOR Chat", Meta = (ExpandEnumAsExecs = "Branches", Keywords = "C++"))
		void ParseChatReceiver(FString& Domain, FString& Name, const FString& Input, ETGOR_FetchEnumeration& Branches);

	/** Parse message for content */
	UFUNCTION(BlueprintCallable, Category = "TGOR Chat", Meta = (ExpandEnumAsExecs = "Branches", Keywords = "C++"))
		void ParseChatContent(FString& Domain, FString& Name, FString& Content, const FString& Input, ETGOR_FetchEnumeration& Branches);
	
	////////////////////////////////////////////////////////////////////////////////////////////////////

	/** Parse string for Domain enum */
	UFUNCTION(BlueprintCallable, Category = "TGOR Chat", Meta = (ExpandEnumAsExecs = "Branches", Keywords = "C++"))
		ETGOR_ChatDomainEnumeration InterpretChatDomain(const FString& Domain, ETGOR_FetchEnumeration& Branches);


	/** Parse string for command resourceable */
	UFUNCTION(BlueprintCallable, Category = "TGOR Chat", Meta = (ExpandEnumAsExecs = "Branches", Keywords = "C++"))
		UTGOR_Command* InterpretChatCommand(const FString& Command, ETGOR_FetchEnumeration& Branches);

	/** Parse string for command resourceables with given prefix */
	UFUNCTION(BlueprintCallable, Category = "TGOR Chat", Meta = (Keywords = "C++"))
		TArray<FString> AutoCompleteChatCommand(const FString& Command);


	/** Parse string for channel resourceable */
	UFUNCTION(BlueprintCallable, Category = "TGOR Chat", Meta = (ExpandEnumAsExecs = "Branches", Keywords = "C++"))
		UTGOR_Channel* InterpretChatChannel(const FString& Channel, ETGOR_FetchEnumeration& Branches);

	/** Parse string for channel resourceables with given prefix */
	UFUNCTION(BlueprintCallable, Category = "TGOR Chat", Meta = (Keywords = "C++"))
		TArray<FString> AutoCompleteChatChannel(const FString& Channel);
	

	/** Parse string for player */
	UFUNCTION(BlueprintCallable, Category = "TGOR Chat", Meta = (ExpandEnumAsExecs = "Branches", Keywords = "C++"))
		FTGOR_UserProperties InterpretChatUser(const FString& User, ETGOR_FetchEnumeration& Branches);

	/** Parse string for players with given prefix */
	UFUNCTION(BlueprintCallable, Category = "TGOR Chat", Meta = (Keywords = "C++"))
		TArray<FString> AutoCompleteChatUser(const FString& User);
	

	/** Parse string for any subdomain with given prefix, limits number of entries no more list gets added */
	UFUNCTION(BlueprintCallable, Category = "TGOR Chat", Meta = (Keywords = "C++"))
		TArray<FString> AutoCompleteAny(const FString& String, int32 EntryLimit);

	////////////////////////////////////////////////////////////////////////////////////////////////////

	/** Uniquely add array of strings to another where string starts with a prefix */
	UFUNCTION(BlueprintCallable, Category = "TGOR Chat", Meta = (Keywords = "C++"))
		static void AppendStartingWith(TArray<FString>& Out, const TArray<FString>& In, const FString& Prefix);

	/** Uniquely add array of strings to another */
	UFUNCTION(BlueprintCallable, Category = "TGOR Chat", Meta = (Keywords = "C++"))
		static void AppendUnique(TArray<FString>& Out, const TArray<FString>& In, int32 EntryLimit);

	////////////////////////////////////////////////////////////////////////////////////////////////////

	/** Retreives max allowed characters */
	UFUNCTION(BlueprintPure, Category = "TGOR Chat", Meta = (Keywords = "C++"))
		int32 GetMaxContentLength();
	
	/** Send message from current state */
	UFUNCTION(BlueprintCallable, Category = "TGOR Chat", Meta = (Keywords = "C++"))
		void SendChatMessage(const FString& Content);


	/** Goes back in chat history */
	UFUNCTION(BlueprintCallable, Category = "TGOR Chat", Meta = (Keywords = "C++"))
		void GoUpHistory();

	/** Goes forward in chat history */
	UFUNCTION(BlueprintCallable, Category = "TGOR Chat", Meta = (Keywords = "C++"))
		void GoDownHistory();

	/** Applies current selection (sets active) */
	UFUNCTION(BlueprintCallable, Category = "TGOR Chat", Meta = (Keywords = "C++"))
		void ApplyHistory();


	/** Finds and sets current chat state from any domain */
	UFUNCTION(BlueprintCallable, Category = "TGOR Chat", Meta = (Keywords = "C++"))
		void FindState(const FString& Receiver, const FString& Content);

	/** Sets current chat state */
	UFUNCTION(BlueprintCallable, Category = "TGOR Chat", Meta = (Keywords = "C++"))
		void ChangeState(ETGOR_ChatDomainEnumeration Domain, const FString& Receiver, const FString& Content);


	/** Resets chat state to default */
	UFUNCTION(BlueprintCallable, Category = "TGOR Chat", Meta = (Keywords = "C++"))
		void ResetState();


	////////////////////////////////////////////////////////////////////////////////////////////////////
protected:

	/** History of earlier commands */
	UPROPERTY(BlueprintReadOnly, Category = "TGOR Chat")
		TArray<FTGOR_ChatMessageInstance> History;

	/** Current position in history */
	UPROPERTY(BlueprintReadOnly, Category = "TGOR Chat")
		int32 HistoryPointer;

	////////////////////////////////////////////////////////////////////////////////////////////////////
	
	/** Currently active message data*/
	UPROPERTY(BlueprintReadOnly, Category = "TGOR Chat")
		FTGOR_ChatMessageInstance CurrentMessage;

	/** Currently active valid prefix text */
	UPROPERTY(BlueprintReadWrite, Category = "TGOR Chat")
		FString CurrentPrefix;


	/** Private domain names */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "TGOR Chat")
		TArray<FString> PrivateDomain;

	/** Public domain names */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "TGOR Chat")
		TArray<FString> PublicDomain;

	/** System domain names */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "TGOR Chat")
		TArray<FString> SystemDomain;

	/** System message colours */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "TGOR Chat", Meta = (Keywords = "C++"))
		FLinearColor SystemColour;

};
