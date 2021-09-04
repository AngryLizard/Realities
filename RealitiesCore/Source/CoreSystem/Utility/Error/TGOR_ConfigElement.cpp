// The Gateway of Realities: Planes of Existence. By Salireths.


#include "TGOR_ConfigElement.h"


void UTGOR_ConfigElement::parse(FString String)
{
	// Reset element
	Pointer = 0;
	Parse = String;
	Elements.Reset();
	// Parse until end of String has been reached or an error occured
	for (;;) 
	{
		// Parse key and content
		FString Key = parseName();
		FString Content = parseContent();
		if (Key == L"") return;
		// Create and add new Element
		Elements.Add(Element({ Key, Content })); 
	}
}

FString UTGOR_ConfigElement::makeString()
{
	// Converts this Element into a string for saving
	FString String = L"(";
	// Iterate through all elements and append them to String
	for (int i = 0; i < Elements.Num(); i++)
		String.Append(writeElement(&Elements[i], i < Elements.Num() - 1));
	return(String);
}

void UTGOR_ConfigElement::set(FString Key, FString Content)
{
	int32 Index = find(Key);
	// Add new element with this key if not found
	if (Index == -1) Elements.Add({ Key, Content });
	else Elements[Index] = {Key, Content};
}

FString UTGOR_ConfigElement::get(FString Key)
{
	int32 Index = find(Key);
	// Return empty string if not found
	if (Index == -1) return(L"");
	else return(Elements[Index].Content);
}

UTGOR_ConfigElement* UTGOR_ConfigElement::getSubElement(FString Key)
{
	// Get string representation of subelement
	FString String = get(Key);
	// Create and parse to subelement
	UTGOR_ConfigElement* Element = NewObject<UTGOR_ConfigElement>(this, UTGOR_ConfigElement::StaticClass());
	Element->parse(String);
	return(Element);
}

void UTGOR_ConfigElement::setSubElement(FString Key, UTGOR_ConfigElement* Element)
{
	// Generate string from subelement
	FString String = Element->makeString();
	// Set subelement
	set(Key, String);
}


FString UTGOR_ConfigElement::parseContent()
{
	bool IgnoreSpaces = true;
	int32 Parentheses = 1;
	FString Content = L"";
	// Parse until end of string reached or return called
	while (Pointer < Parse.Len())
	{
		TCHAR c = Parse[Pointer++]; // Parse next digit
		if (c == '(') { if (++Parentheses > 1) Content.AppendChar(c); } else // Start of an array
		if (c == ')') { if (--Parentheses > 0) Content.AppendChar(c); else return(Content); } else // End of an array (return if not end of array)
		if (c == '"') { IgnoreSpaces = !IgnoreSpaces; Content.AppendChar(c); } else // Start/End of a string in quotes (accepts spaces)
		if (Parentheses <= 1 && c == ',') return(Content); else // Return if end of content substring
		if (!IgnoreSpaces || c != ' ') Content.AppendChar(c); // Append character to string only if not a space when Ignore Spaces is set
	}
	return(L"");
}

FString UTGOR_ConfigElement::parseName()
{
	FString Key = L"";
	// Parse until end of string reached or return called
	while (Pointer < Parse.Len())
	{
		TCHAR c = Parse[Pointer++]; // Parse next digit
		if (c == '=') return(Key); else // Return if end of key definition reached
		if (c != ' ' && c != '(') Key.AppendChar(c); // Append character to string only if not a space or parantesis
	}
	return(L"");
}

FString UTGOR_ConfigElement::writeElement(Element* e, bool c)
{
	// Write key
	FString String = e->Key;
	String.AppendChar('=');
	// Write content
	String.Append(e->Content);
	String.AppendChar(c ? ',' : ')');
	return(String);
}


int32 UTGOR_ConfigElement::find(FString Key)
{
	for (int i = 0; i < Elements.Num(); i++)
		if (Elements[i].Key == Key)
			return(i);
	return(-1);
}