// The Gateway of Realities: Planes of Existence. By Salireths.

#pragma once

#include "UObject/Object.h"
#include "TGOR_ConfigElement.generated.h"

/**
 * 
 */
UCLASS()
class REALITIES_API UTGOR_ConfigElement : public UObject
{
	GENERATED_BODY()
	
	struct Element
	{
		FString Key;
		FString Content;
	};

public:
	/** Parse string into this element */
	UFUNCTION(BlueprintCallable, Category = "ConfigElement", Meta = (Keywords = "C++"))
	void parse(FString String);

	/** Create string representation of this element */
	UFUNCTION(BlueprintCallable, Category = "ConfigElement", Meta = (Keywords = "C++"))
	FString makeString();

	/** Set Key to certain value (not a hashtable, call sparingly on big arrays) */
	UFUNCTION(BlueprintCallable, Category = "ConfigElement", Meta = (Keywords = "C++"))
	void set(FString Key, FString Content);

	/** Get value at certain key (not a hashtable, call sparingly on big arrays) */
	UFUNCTION(BlueprintCallable, Category = "ConfigElement", Meta = (Keywords = "C++"))
	FString get(FString Key);

	/** Get Subelement if element is an array */
	UFUNCTION(BlueprintCallable, Category = "ConfigElement", Meta = (Keywords = "C++"))
	UTGOR_ConfigElement* getSubElement(FString Key);

	/** Set Subelement as an array */
	UFUNCTION(BlueprintCallable, Category = "ConfigElement", Meta = (Keywords = "C++"))
	void setSubElement(FString Key, UTGOR_ConfigElement* Element);

private:
	TArray<Element> Elements;
	FString Parse;
	int32 Pointer;

	int32 find(FString Key);
	FString writeElement(Element* e, bool c);
	FString parseContent();
	FString parseName();
};
