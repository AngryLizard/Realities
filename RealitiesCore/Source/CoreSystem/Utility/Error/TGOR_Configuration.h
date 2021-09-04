// The Gateway of Realities: Planes of Existence. By Salireths.

#pragma once

#include "UObject/Object.h"
#include "TGOR_ConfigElement.h"
#include "TGOR_Configuration.generated.h"

UENUM(BlueprintType)
enum class ETGOR_ConfigType : uint8
{
	EditorPerProjectIni,
	CompatIni,
	LightmassIni,
	ScalabilityIni,
	InputIni,
	GameIni,
	GameUserSettingsIni
};


/**
 * 
 */
UCLASS(Blueprintable)
class CORESYSTEM_API UTGOR_Configuration : public UObject
{
	GENERATED_BODY()
public:

	/** Load config (has to be in a supported path) from file (filename without path and .ini, i.e. "../Config/Example.ini" => Example) */
	UFUNCTION(BlueprintCallable, Category = "Configuration", Meta = (ExpandEnumAsExecs = "Branches", Keywords = "C++"))
	FString loadConfig(FString Filename, FString Section, ETGOR_ComputeEnumeration& Branches);

	/** Load default config */
	UFUNCTION(BlueprintCallable, Category = "Configuration", Meta = (ExpandEnumAsExecs = "Branches", Keywords = "C++"))
	FString loadDefaultConfig(ETGOR_ConfigType Type, FString Section, ETGOR_ComputeEnumeration& Branches);

	/** Set String at given key */
	UFUNCTION(BlueprintCallable, Category = "Configuration", Meta = (Keywords = "C++"))
	void setString(FString Key, FString String);

	/** Get String at given key */
	UFUNCTION(BlueprintCallable, Category = "Configuration", Meta = (Keywords = "C++"))
	FString getString(FString Key);

	/** Set Array at given key */
	UFUNCTION(BlueprintCallable, Category = "Configuration", Meta = (Keywords = "C++"))
	void setArray(FString Key, TArray<UTGOR_ConfigElement*> Elements);

	/** Get Array at given key */
	UFUNCTION(BlueprintCallable, Category = "Configuration", Meta = (Keywords = "C++"))
	TArray<UTGOR_ConfigElement*> getArray(FString Key);

	/** Get element of subarray. Identify element with SearchKey and SearchContent */
	UFUNCTION(BlueprintCallable, Category = "Configuration", Meta = (ExpandEnumAsExecs = "Branches", Keywords = "C++"))
	FString getSubElement(FString Key, FString SearchKey, FString SearchContent, FString ContentKey, ETGOR_ComputeEnumeration& Branches);

	/** Set element of subarray. Identify element with SearchKey and SearchContent */
	UFUNCTION(BlueprintCallable, Category = "Configuration", Meta = (ExpandEnumAsExecs = "Branches", Keywords = "C++"))
	void setSubElement(FString Key, FString SearchKey, FString SearchContent, FString ContentKey, FString Content, ETGOR_ComputeEnumeration& Branches);

	/** Write changes to ini file */
	UFUNCTION(BlueprintCallable, Category = "Configuration", Meta = (Keywords = "C++"))
	void flush();

	/** Print all keys to console (for debugging) */
	UFUNCTION(BlueprintCallable, Category = "Configuration", Meta = (Keywords = "C++"))
	void printKeys();

private:
	FConfigFile* ConfigFile;
	FString SectionName;
	FString File;

	TArray<UTGOR_ConfigElement*> Cache;
	FString CacheKey;
};
