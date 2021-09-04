// The Gateway of Realities: Planes of Existence.

#include "TGOR_StorageLibrary.h"

#include "RealitiesUGC/Mod/TGOR_ContentManager.h"

#include "CoreSystem/TGOR_Singleton.h"
#include "CoreSystem/Storage/TGOR_Package.h"
#include "CoreSystem/Content/TGOR_CoreContent.h"


void UTGOR_StorageLibrary::WriteInteger(FTGOR_GroupWritePackage& Package, FString Identifier, int32 Value) { Package.WriteEntry(Identifier, Value); }
bool UTGOR_StorageLibrary::ReadInteger(FTGOR_GroupReadPackage& Package, FString Identifier, int32& Value) { return Package.ReadEntry(Identifier, Value); }

void UTGOR_StorageLibrary::WriteByte(FTGOR_GroupWritePackage& Package, FString Identifier, uint8 Value) { Package.WriteEntry(Identifier, Value); }
bool UTGOR_StorageLibrary::ReadByte(FTGOR_GroupReadPackage& Package, FString Identifier, uint8& Value) { return Package.ReadEntry(Identifier, Value); }

void UTGOR_StorageLibrary::WriteFloat(FTGOR_GroupWritePackage& Package, FString Identifier, float Value) { Package.WriteEntry(Identifier, Value); }
bool UTGOR_StorageLibrary::ReadFloat(FTGOR_GroupReadPackage& Package, FString Identifier, float& Value) { return Package.ReadEntry(Identifier, Value); }

void UTGOR_StorageLibrary::WriteString(FTGOR_GroupWritePackage& Package, FString Identifier, FString Value) { Package.WriteEntry(Identifier, Value); }
bool UTGOR_StorageLibrary::ReadString(FTGOR_GroupReadPackage& Package, FString Identifier, FString& Value) { return Package.ReadEntry(Identifier, Value); }

void UTGOR_StorageLibrary::WriteName(FTGOR_GroupWritePackage& Package, FString Identifier, FName Value) { Package.WriteEntry(Identifier, Value); }
bool UTGOR_StorageLibrary::ReadName(FTGOR_GroupReadPackage& Package, FString Identifier, FName& Value) { return Package.ReadEntry(Identifier, Value); }

void UTGOR_StorageLibrary::WriteVector(FTGOR_GroupWritePackage& Package, FString Identifier, FVector Value) { Package.WriteEntry(Identifier, Value); }
bool UTGOR_StorageLibrary::ReadVector(FTGOR_GroupReadPackage& Package, FString Identifier, FVector& Value) { return Package.ReadEntry(Identifier, Value); }

void UTGOR_StorageLibrary::WriteRotation(FTGOR_GroupWritePackage& Package, FString Identifier, FRotator Value) { Package.WriteEntry(Identifier, Value); }
bool UTGOR_StorageLibrary::ReadRotation(FTGOR_GroupReadPackage& Package, FString Identifier, FRotator& Value) { return Package.ReadEntry(Identifier, Value); }

void UTGOR_StorageLibrary::WriteContent(FTGOR_GroupWritePackage& Package, FString Identifier, UTGOR_Content* Value) { Package.WriteEntry(Identifier, Value); }
bool UTGOR_StorageLibrary::ReadContent(FTGOR_GroupReadPackage& Package, FString Identifier, UTGOR_Content*& Value) { return Package.ReadEntry(Identifier, Value); }

void UTGOR_StorageLibrary::WriteObject(FTGOR_GroupWritePackage& Package, FString Identifier, TScriptInterface<ITGOR_SaveInterface> Value) { Package.WriteEntry(Identifier, Value); }
bool UTGOR_StorageLibrary::ReadObject(FTGOR_GroupReadPackage& Package, FString Identifier, TScriptInterface<ITGOR_SaveInterface> Value) { return Package.ReadEntry(Identifier, Value); }