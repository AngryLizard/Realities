// The Gateway of Realities: Planes of Existence.


#include "TGOR_File.h"
#include "GenericPlatform/GenericPlatformFile.h"
#include "HAL/PlatformFilemanager.h"


FTGOR_File::FTGOR_File()
{
}

void FTGOR_File::SetFilename(const FString& Directory, const FString& Filename)
{
	Dir = Directory;
	Name = Filename;
}

bool FTGOR_File::WriteBinaryFile()
{
	// Set names
	Sanitize();
	if (SecureDirectory())
	{
		// Create file
		IPlatformFile& PlatformFile = FPlatformFileManager::Get().GetPlatformFile();
		IFileHandle* FileHandle = PlatformFile.OpenWrite(*GetCurrentDir());
		if (FileHandle)
		{
			Database.Write(FileHandle);
			FileHandle->Flush(true);
			delete(FileHandle);
			return true;
		}
	}
	return false;
}

bool FTGOR_File::ReadBinaryFile()
{
	if (SecureFile())
	{
		// Read file
		IPlatformFile& PlatformFile = FPlatformFileManager::Get().GetPlatformFile();
		IFileHandle* FileHandle = PlatformFile.OpenRead(*GetCurrentDir());
		if (FileHandle)
		{
			Database.Read(FileHandle);
			delete(FileHandle);
			return true;
		}
	}
	return false;
}

bool FTGOR_File::SecureDirectory()
{
	// Get actual path
	IPlatformFile& PlatformFile = FPlatformFileManager::Get().GetPlatformFile();

	// Create directory if not exists
	if (!PlatformFile.DirectoryExists(*Dir))
	{
		PlatformFile.CreateDirectoryTree(*Dir);
		return PlatformFile.DirectoryExists(*Dir);
	}
	return true;
}

bool FTGOR_File::SecureFile()
{
	// Set names
	Sanitize();
	SecureDirectory();

	// Get actual path
	FString FileDir = GetCurrentDir();
	IPlatformFile& PlatformFile = FPlatformFileManager::Get().GetPlatformFile();

	// Display error in case file doesn't exist
	if (!PlatformFile.FileExists(*FileDir))
	{
		return false;
	}
	return true;
}

FString FTGOR_File::GetCurrentDir() const
{
	return(Dir + "/" + GetCurrentFilename() + ".tgor");
}

FString FTGOR_File::GetCurrentFilename() const
{
	return(Name);
}

void FTGOR_File::Sanitize()
{
	// Remove right most 5 digits if there is a .tgor
	if (Name.Find(L".tgor") > 0)
	{
		Name = Name.LeftChop(5);
	}
}
