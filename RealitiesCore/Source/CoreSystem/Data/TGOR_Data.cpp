// The Gateway of Realities: Planes of Existence.

#include "TGOR_Data.h"
#include "CoreSystem/TGOR_Singleton.h"

#include "CoreSystem/Gameplay/TGOR_GameInstance.h"
#include "CoreSystem/Gameplay/TGOR_GameState.h"
#include "CoreSystem/Storage/TGOR_Package.h"


////////////////////////////////////////////////////////////////////////////////////////////////////


UTGOR_Data::UTGOR_Data()
	: Folder("Data")
{
}

void UTGOR_Data::SaveToFile()
{
	SINGLETON_CHK;
	TScriptInterface<ITGOR_SaveInterface> Interface = this;
	File.Write(Singleton.Get(), Interface);

	File.WriteBinaryFile();
}

void UTGOR_Data::LoadFromFile()
{
	if (File.ReadBinaryFile())
	{
		SINGLETON_CHK;
		TScriptInterface<ITGOR_SaveInterface> Interface = this;
		File.Read(Singleton.Get(), Interface);
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