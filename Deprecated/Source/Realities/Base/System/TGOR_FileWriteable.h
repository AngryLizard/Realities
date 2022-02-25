// The Gateway of Realities: Planes of Existence.

#pragma once

#include "Realities/Utility/Storage/TGOR_Serialisation.h"
#include "Realities/Utility/Error/TGOR_Error.h"

#include "UObject/NoExportTypes.h"
#include "TGOR_FileWriteable.generated.h"

/**
* TGOR_FileWriteable provides a baseclass for classes that can be written to file
*/
UCLASS()
class REALITIES_API UTGOR_FileWriteable : public UObject
{
	GENERATED_BODY()

public:
	UTGOR_FileWriteable();

	////////////////////////////////////////////////////////////////////////////////////////////////////
protected:

	/** Bool deciding whether or not file errors will get printed */
	UPROPERTY(BlueprintReadWrite, Category = "TGOR System")
	bool PrintFileNotFoundErrors;

	////////////////////////////////////////////////////////////////////////////////////////////////////
public:

	/** Writes this object to a file */
	UFUNCTION(BlueprintCallable, Category = "TGOR System", Meta = (Keywords = "C++"))
		void WriteBinaryFile(FString Directory, FString Filename);

	/** Reads this object from a file */
	UFUNCTION(BlueprintCallable, Category = "TGOR System", Meta = (Keywords = "C++"))
		void ReadBinaryFile(FString Directory, FString Filename);

	////////////////////////////////////////////////////////////////////////////////////////////////////

	/** Makes sure directory exists. */
	UFUNCTION(BlueprintCallable, Category = "TGOR System", Meta = (Keywords = "C++"))
		void SecureDirectory(FString Directory);

	/** Makes sure file exists. */
	UFUNCTION(BlueprintCallable, Category = "TGOR System", Meta = (Keywords = "C++"))
		void SecureFile(FString Directory, FString Filename);

	/** Returns currently open directory */
	UFUNCTION(BlueprintCallable, Category = "TGOR System", Meta = (Keywords = "C++"))
		FString GetCurrentDir();

	/** Returns filename without .tgor */
	UFUNCTION(BlueprintCallable, Category = "TGOR System", Meta = (Keywords = "C++"))
		FString Sanitize(FString Filename);

	////////////////////////////////////////////////////////////////////////////////////////////////////

	void PageNext(int64 Size);
	void NextPage();
	
	virtual bool Write() { return(false); };
	virtual bool Read() { return(false); };
	
private:

	FString _directory;
	FString _filename;
};
