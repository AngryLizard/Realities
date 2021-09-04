// The Gateway of Realities: Planes of Existence.


#include "TGOR_FileWriteable.h"
#include "GenericPlatform/GenericPlatformFile.h"
#include "HAL/PlatformFilemanager.h"
#include "Misc/Paths.h"

UTGOR_FileWriteable::UTGOR_FileWriteable()
	: Super()
{
	PrintFileNotFoundErrors = true;
}


void UTGOR_FileWriteable::WriteBinaryFile(FString Directory, FString Filename)
{
	/*

	// Set names
	_filename = Sanitize(Filename);
	SecureDirectory(Directory);
	// Create file
	IPlatformFile& PlatformFile = FPlatformFileManager::Get().GetPlatformFile();
	IFileHandle* FileHandle = PlatformFile.OpenWrite(*GetCurrentDir());

	if (FileHandle)
	{
		// Write to file
		_fileHandlePointer = new CTGOR_FileHandlePointer(FileHandle);

		if (!Write(*_fileHandlePointer))
		{
			EPRNT(FString("Writing file ") + Filename + " failed or incomplete")
		}
		
		if (!_fileHandlePointer->IsValid())
		{
			EPRNT(FString("Writing file ") + Filename + " killed")
		}
		// Cleanup
		delete(_fileHandlePointer);
		_fileHandlePointer = nullptr;
		delete(FileHandle);
	}
	else
	{
		if (PrintFileNotFoundErrors)
		{
			EPRNT(FString("Could not open file ") + Filename + " to write")
		}
	}
	*/
}

void UTGOR_FileWriteable::ReadBinaryFile(FString Directory, FString Filename)
{
	/*
	// Set names
	SecureFile(Directory, Filename);
	// Read file
	IPlatformFile& PlatformFile = FPlatformFileManager::Get().GetPlatformFile();
	IFileHandle* FileHandle = PlatformFile.OpenRead(*GetCurrentDir());

	if (FileHandle)
	{
		// Read from file
		_fileHandlePointer = new CTGOR_FileHandlePointer(FileHandle);
		_fileHandlePointer->SetEnd(0);

		if (!Read(*_fileHandlePointer))
		{
			EPRNT(FString("Reading file ") + Filename + " failed or incomplete")
		}
		
		if(!_fileHandlePointer->IsValid())
		{
			EPRNT(FString("Reading file ") + Filename + " killed")
		}
		// Cleanup
		delete(_fileHandlePointer);
		_fileHandlePointer = nullptr;
		delete(FileHandle);
	}
	else
	{
		if (PrintFileNotFoundErrors)
		{
			EPRNT(FString("Could not open file ") + Filename + " to read")
		}
	}
	*/
}


void UTGOR_FileWriteable::SecureDirectory(FString Directory)
{
	// Set names
	_directory = Directory;
	// Get actual path
	FString DirectoryDir = FPaths::ProjectDir() + "/" + _directory;
	IPlatformFile& PlatformFile = FPlatformFileManager::Get().GetPlatformFile();
	// Create directory if not exists
	if (!PlatformFile.DirectoryExists(*DirectoryDir))
	{
		PlatformFile.CreateDirectory(*DirectoryDir);
		if (!PlatformFile.DirectoryExists(*DirectoryDir))
		{
			ERROR("Directory couldn't be created.", Error);
		}
	}
}

void UTGOR_FileWriteable::SecureFile(FString Directory, FString Filename)
{
	// Set names
	_filename = Sanitize(Filename);
	SecureDirectory(Directory);
	// Get actual path
	FString FileDir = GetCurrentDir();
	IPlatformFile& PlatformFile = FPlatformFileManager::Get().GetPlatformFile();
	// Display error in case file doesn't exist
	if (!PlatformFile.FileExists(*FileDir))
	{
		if (PrintFileNotFoundErrors)
		{
			ERROR("File couldn't be opened.", Error);
		}
	}
}


FString UTGOR_FileWriteable::GetCurrentDir()
{
	FString GameDir = FPaths::ProjectDir();
	return(GameDir + "/" + _directory + "/" + _filename + ".tgor");
}


FString UTGOR_FileWriteable::Sanitize(FString Filename)
{
	// Remove right most 5 digits if there is a .tgor
	if (Filename.Find(L".tgor") > 0)
	{ return(Filename.LeftChop(5)); }
	return(Filename);
}


void UTGOR_FileWriteable::PageNext(int64 Size)
{
	/*
	check(_fileHandlePointer != nullptr)
	// Make sure buffer is at end of last page
	int64 End = _fileHandlePointer->End();
	_fileHandlePointer->JumpTo(End);
	End += Size + sizeof(int64);
	// Increase buffer size and write page
	_fileHandlePointer->SetEnd(End);
	_fileHandlePointer->WriteBytes(&End, sizeof(int64));
	*/
}


void UTGOR_FileWriteable::NextPage()
{
	/*
	check(_fileHandlePointer != nullptr)
	// Make sure buffer is at end of last page and page can be written
	int64 End = _fileHandlePointer->End();
	_fileHandlePointer->JumpTo(End);
	_fileHandlePointer->SetEnd(End + sizeof(int64));
	// Increase buffer size and write page
	_fileHandlePointer->ReadBytes(&End, sizeof(int64));
	_fileHandlePointer->SetEnd(End);
	*/
}
