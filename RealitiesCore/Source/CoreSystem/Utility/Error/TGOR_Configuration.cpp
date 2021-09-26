// The Gateway of Realities: Planes of Existence. By Salireths.


#include "TGOR_Configuration.h"
#include "CoreSystem/Utility/TGOR_CoreEnumerations.h"
#include "CoreMinimal.h"
#include "Misc/ConfigCacheIni.h"

FString UTGOR_Configuration::loadConfig(FString Filename, FString Section, ETGOR_ComputeEnumeration& Branches)
{
	Branches = ETGOR_ComputeEnumeration::Failed;
	FString Name = Filename + ".ini";
	FString ConfigsPath;
	// Search default paths for file
	if ((ConfigFile = GConfig->Find(ConfigsPath = FPaths::EngineConfigDir() + Name)) == nullptr)
	if ((ConfigFile = GConfig->Find(ConfigsPath = FPaths::SourceConfigDir() + Name)) == nullptr)
	if ((ConfigFile = GConfig->Find(ConfigsPath = FPaths::GeneratedConfigDir() + Name)) == nullptr)
	if ((ConfigFile = GConfig->Find(ConfigsPath = FPaths::ProjectConfigDir() + Name)) == nullptr)
	{ return(L"File not found."); }
	// Check if section exists
	if (!ConfigFile->Contains(Section)) { return(ConfigsPath + L" Section not found."); }
	Branches = ETGOR_ComputeEnumeration::Success;
	SectionName = Section;
	return(File = ConfigsPath);
}

FString UTGOR_Configuration::loadDefaultConfig(ETGOR_ConfigType Type, FString Section, ETGOR_ComputeEnumeration& Branches)
{
	Branches = ETGOR_ComputeEnumeration::Failed;
	// Assign default config file path
	FString ConfigsPath;
	switch (Type)
	{
		case ETGOR_ConfigType::EditorPerProjectIni :	ConfigsPath = GEditorPerProjectIni; break;
		case ETGOR_ConfigType::CompatIni :				ConfigsPath = GCompatIni; break;
		case ETGOR_ConfigType::LightmassIni :			ConfigsPath = GLightmassIni; break;
		case ETGOR_ConfigType::ScalabilityIni :			ConfigsPath = GScalabilityIni; break;
		case ETGOR_ConfigType::InputIni :				ConfigsPath = GInputIni; break;
		case ETGOR_ConfigType::GameIni :				ConfigsPath = GGameIni; break;
		case ETGOR_ConfigType::GameUserSettingsIni :	ConfigsPath = GGameUserSettingsIni; break;
		default: return(L"Default not found.");
	}
	// Make sure file exists
	if ((ConfigFile = GConfig->Find(ConfigsPath)) == nullptr)
	{ return(L"File not found."); }
	// Check if section exists
	if (!ConfigFile->Contains(Section)) { return(ConfigsPath + L" Section not found."); }
	Branches = ETGOR_ComputeEnumeration::Success;
	SectionName = Section;
	return(File = ConfigsPath);
}

void UTGOR_Configuration::setString(FString Key, FString String)
{
	// Use default implementation
	GConfig->SetString(*SectionName, *Key, *String, File);
}

FString UTGOR_Configuration::getString(FString Key)
{
	FString Value;
	// Use default implementation
	GConfig->GetString(*SectionName, *Key, Value, File);
	return(Value);
}

void UTGOR_Configuration::setArray(FString Key, TArray<UTGOR_ConfigElement*> Elements)
{
	// Convert Elements to plaintext
	TArray<FString> Array;
	for (int i = 0; i < Elements.Num(); i++)
		Array.Add(Elements[i]->makeString());
	// Write into config
	GConfig->SetArray(*SectionName, *Key, Array, File);
}

TArray<UTGOR_ConfigElement*> UTGOR_Configuration::getArray(FString Key)
{
	// Check if already cached
	if (Key == CacheKey) return(Cache);
	CacheKey = Key;
	// Empty cache
	Cache.Reset();
	TArray<FString> Array;
	GConfig->GetArray(*SectionName, *Key, Array, File);
	// Parse plaintext into elements
	for (int i = 0; i < Array.Num(); i++)
	{
		UTGOR_ConfigElement* Element = NewObject<UTGOR_ConfigElement>(this, UTGOR_ConfigElement::StaticClass());
		Element->parse(Array[i]);
		Cache.Add(Element);
	}
	return(Cache);
}

FString UTGOR_Configuration::getSubElement(FString Key, FString SearchKey, FString SearchContent, FString ContentKey, ETGOR_ComputeEnumeration& Branches)
{
	// Retrieve list of elements
	Branches = ETGOR_ComputeEnumeration::Success;
	TArray<UTGOR_ConfigElement*> Array = getArray(Key);
	// Find entry with matching content
	for (int i = 0; i < Array.Num(); i++)
		if (Array[i]->get(SearchKey) == SearchContent)
			return(Array[i]->get(ContentKey));
	Branches = ETGOR_ComputeEnumeration::Failed;
	return(L"");
}

void UTGOR_Configuration::setSubElement(FString Key, FString SearchKey, FString SearchContent, FString ContentKey, FString Content, ETGOR_ComputeEnumeration& Branches)
{
	// Retrieve list of elements
	Branches = ETGOR_ComputeEnumeration::Success;
	TArray<UTGOR_ConfigElement*> Array = getArray(Key);
	// Find entry with matching content, overwrite and write to config
	for (int i = 0; i < Array.Num(); i++)
		if (Array[i]->get(SearchKey) == SearchContent)
		{
			Array[i]->set(ContentKey, Content);
			setArray(Key, Array);
			return;
		}
	Branches = ETGOR_ComputeEnumeration::Failed;
}

void UTGOR_Configuration::flush()
{
	// Use default implementation
	GConfig->Flush(false, File);
}

void UTGOR_Configuration::printKeys()
{
	TArray<FName> Array;
	// Put all keys into array
	ConfigFile->Find(SectionName)->GetKeys(Array);
	// Display array
	//for (int i = 0; i < Array.Num(); i++)
	//DEBUGTRACESTRING(Array[i].GetPlainNameString());
}