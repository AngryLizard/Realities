// The Gateway of Realities: Planes of Existence.

#include "TGOR_Data.h"
#include "Realities/Base/TGOR_Singleton.h"

#include "Realities/Base/TGOR_GameInstance.h"
#include "Realities/Utility/Storage/TGOR_Package.h"

UTGOR_Data::UTGOR_Data()
	: Folder("Data")
{
}

void UTGOR_Data::SaveToFile()
{
	SINGLETON_CHK;
	TScriptInterface<ITGOR_SaveInterface> Interface = this;
	File.Write(Singleton, Interface);

	File.WriteBinaryFile();
}

void UTGOR_Data::LoadFromFile()
{
	if (File.ReadBinaryFile())
	{
		SINGLETON_CHK;
		TScriptInterface<ITGOR_SaveInterface> Interface = this;
		File.Read(Singleton, Interface);
	}
}

FString UTGOR_Data::GetFilename() const
{
	return File.GetCurrentFilename();
}

void UTGOR_Data::SetFilename(const FString& Name)
{
	const FString Path = UTGOR_GameInstance::GetSaveDirectory(this) / Folder;
	File.SetFilename(Path, Name);
}