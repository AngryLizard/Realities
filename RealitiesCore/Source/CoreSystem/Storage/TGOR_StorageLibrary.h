// The Gateway of Realities: Planes of Existence.

#pragma once

#include "CoreSystem/Storage/TGOR_PackageGroup.h"

#include "Kismet/BlueprintFunctionLibrary.h"
#include "TGOR_StorageLibrary.generated.h"

/**
 * 
 */
UCLASS()
class CORESYSTEM_API UTGOR_StorageLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
public:

	/** */
	UFUNCTION(BlueprintCallable, Category = "!TGOR EditorUtilities", Meta = (Keywords = "C++"))
		static void WriteInteger(UPARAM(ref) FTGOR_GroupWritePackage& Package, FString Identifier, int32 Value);

	UFUNCTION(BlueprintCallable, Category = "!TGOR EditorUtilities", Meta = (Keywords = "C++"))
		static bool ReadInteger(UPARAM(ref) FTGOR_GroupReadPackage& Package, FString Identifier, int32& Value);


	/** */
	UFUNCTION(BlueprintCallable, Category = "!TGOR EditorUtilities", Meta = (Keywords = "C++"))
		static void WriteByte(UPARAM(ref) FTGOR_GroupWritePackage& Package, FString Identifier, uint8 Value);

	UFUNCTION(BlueprintCallable, Category = "!TGOR EditorUtilities", Meta = (Keywords = "C++"))
		static bool ReadByte(UPARAM(ref) FTGOR_GroupReadPackage& Package, FString Identifier, uint8& Value);


	/** */
	UFUNCTION(BlueprintCallable, Category = "!TGOR EditorUtilities", Meta = (Keywords = "C++"))
		static void WriteFloat(UPARAM(ref) FTGOR_GroupWritePackage& Package, FString Identifier, float Value);

	UFUNCTION(BlueprintCallable, Category = "!TGOR EditorUtilities", Meta = (Keywords = "C++"))
		static bool ReadFloat(UPARAM(ref) FTGOR_GroupReadPackage& Package, FString Identifier, float& Value);


	/** */
	UFUNCTION(BlueprintCallable, Category = "!TGOR EditorUtilities", Meta = (Keywords = "C++"))
		static void WriteString(UPARAM(ref) FTGOR_GroupWritePackage& Package, FString Identifier, FString Value);

	UFUNCTION(BlueprintCallable, Category = "!TGOR EditorUtilities", Meta = (Keywords = "C++"))
		static bool ReadString(UPARAM(ref) FTGOR_GroupReadPackage& Package, FString Identifier, FString& Value);


	/** */
	UFUNCTION(BlueprintCallable, Category = "!TGOR EditorUtilities", Meta = (Keywords = "C++"))
		static void WriteName(UPARAM(ref) FTGOR_GroupWritePackage& Package, FString Identifier, FName Value);

	UFUNCTION(BlueprintCallable, Category = "!TGOR EditorUtilities", Meta = (Keywords = "C++"))
		static bool ReadName(UPARAM(ref) FTGOR_GroupReadPackage& Package, FString Identifier, FName& Value);


	/** */
	UFUNCTION(BlueprintCallable, Category = "!TGOR EditorUtilities", Meta = (Keywords = "C++"))
		static void WriteVector(UPARAM(ref) FTGOR_GroupWritePackage& Package, FString Identifier, FVector Value);

	UFUNCTION(BlueprintCallable, Category = "!TGOR EditorUtilities", Meta = (Keywords = "C++"))
		static bool ReadVector(UPARAM(ref) FTGOR_GroupReadPackage& Package, FString Identifier, FVector& Value);


	/** */
	UFUNCTION(BlueprintCallable, Category = "!TGOR EditorUtilities", Meta = (Keywords = "C++"))
		static void WriteRotation(UPARAM(ref) FTGOR_GroupWritePackage& Package, FString Identifier, FRotator Value);

	UFUNCTION(BlueprintCallable, Category = "!TGOR EditorUtilities", Meta = (Keywords = "C++"))
		static bool ReadRotation(UPARAM(ref) FTGOR_GroupReadPackage& Package, FString Identifier, FRotator& Value);


	/** */
	UFUNCTION(BlueprintCallable, Category = "!TGOR EditorUtilities", Meta = (Keywords = "C++"))
		static void WriteContent(UPARAM(ref) FTGOR_GroupWritePackage& Package, FString Identifier, UTGOR_Content* Value);

	UFUNCTION(BlueprintCallable, Category = "!TGOR EditorUtilities", Meta = (Keywords = "C++"))
		static bool ReadContent(UPARAM(ref) FTGOR_GroupReadPackage& Package, FString Identifier, UTGOR_Content*& Value);


	/** */
	UFUNCTION(BlueprintCallable, Category = "!TGOR EditorUtilities", Meta = (Keywords = "C++"))
		static void WriteObject(UPARAM(ref) FTGOR_GroupWritePackage& Package, FString Identifier, TScriptInterface<ITGOR_SaveInterface> Value);

	UFUNCTION(BlueprintCallable, Category = "!TGOR EditorUtilities", Meta = (Keywords = "C++"))
		static bool ReadObject(UPARAM(ref) FTGOR_GroupReadPackage& Package, FString Identifier, TScriptInterface<ITGOR_SaveInterface> Value);

};
