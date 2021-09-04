// The Gateway of Realities: Planes of Existence.

#pragma once

#include "CoreMinimal.h"
#include "CoreSystem/Storage/TGOR_PackageMinimal.h"

#include "UObject/NoExportTypes.h"

#include "TGOR_File.generated.h"

USTRUCT(BlueprintType)
struct CORESYSTEM_API FTGOR_File
{
	GENERATED_USTRUCT_BODY()
private:
	CTGOR_Database Database;

	FString Dir;
	FString Name;

public:
	FTGOR_File();
	void SetFilename(const FString& Directory, const FString& Filename);
	
	////////////////////////////////////////////////////////////////////////////////////////////////////
public:

	template<typename T>
	void Write(UTGOR_Context* Context, const T& In)
	{
		Database.ExpandHistory(Context->GetVersion());
		Database.InitialWrite(Context, In);
	}

	template<typename T>
	bool Read(UTGOR_Context* Context, T& Out)
	{
		return Database.InitialRead(Context, Out);
	}

	/** Read/Write from to this file */
	bool WriteBinaryFile();
	bool ReadBinaryFile();

	////////////////////////////////////////////////////////////////////////////////////////////////////

	/** Makes sure directory exists, returns false if not available */
	bool SecureDirectory();

	/** Makes sure file exists, returns false if not available */
	bool SecureFile();

	/** Returns currently open directory */
	FString GetCurrentDir() const;

	/** Returns currently associated filename */
	FString GetCurrentFilename() const;

	/** Removes .tgor from filename */
	void Sanitize();
};