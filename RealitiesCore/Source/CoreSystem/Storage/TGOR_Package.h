// The Gateway of Realities: Planes of Existence.

#pragma once

#include "CoreMinimal.h"
#include "TGOR_PackageMinimal.h"

#include "TGOR_Buffer.h"
#include "TGOR_PackageNetwork.h"
#include "TGOR_PackageGroup.h"
#include "TGOR_PackageArray.h"
#include "TGOR_PackageMap.h"
#include "TGOR_Table.h"

#include "TGOR_SaveInterface.h"
#include "RealitiesUtility/Utility/TGOR_Math.h"
#include "RealitiesUtility/Structures/TGOR_Time.h"
#include "RealitiesUtility/Structures/TGOR_Polar.h"
#include "RealitiesUtility/Structures/TGOR_Index.h"
#include "RealitiesUtility/Structures/TGOR_Normal.h"
#include "RealitiesUtility/Structures/TGOR_Percentage.h"

// Check for whether variable can be stored in memory as is
template<class S, typename = void>
struct is_compressible : std::false_type {};

template<class S>
struct is_compressible<S, typename std::enable_if_t<
	std::is_arithmetic<S>::value || std::is_same<S, FVector2D>::value || std::is_same<S, FLinearColor>::value || std::is_same<S, FColor>::value>> : std::true_type {};

// Compressible variables can be copied to memory
template<typename S>
class CTGOR_Pack<S, typename std::enable_if_t<is_compressible<S>::value>>
{
public:
	static CORESYSTEM_API FString DataType();
	static void DataWrite(UTGOR_Context* Context, CTGOR_Database* Database, uint32 Position, uint32& Size, const S& In)
	{
		Size = sizeof(S);
		Database->Write(Position, (const uint8*)& In, Size);
	}
	static bool DataRead(UTGOR_Context* Context, const CTGOR_Database* Database, uint32 Position, uint32 Size, S& Out)
	{
		Database->Read(Position, (uint8*)& Out, Size);
		return true;
	}
};

// Can compress bools even further
template<>
class CTGOR_Arch<bool>
{
public:
	static void DataWrite(UTGOR_Context* Context, CTGOR_Archive* Archive, const bool& In)
	{
		Archive->WriteBits((const uint8*)&In, 1);
	}
	static bool DataRead(UTGOR_Context* Context, const CTGOR_Archive* Archive, bool& Out)
	{
		Out = false;
		Archive->ReadBits((uint8*)&Out, 1);
		return true;
	}
};

template<typename S>
class CTGOR_Arch<S, typename std::enable_if_t<is_compressible<S>::value && !std::is_same<S, bool>::value>>
{
public:
	static void DataWrite(UTGOR_Context* Context, CTGOR_Archive* Archive, const S& In)
	{
		Archive->Write((const uint8*)&In, sizeof(S));
	}
	static bool DataRead(UTGOR_Context* Context, const CTGOR_Archive* Archive, S& Out)
	{
		Archive->Read((uint8*)&Out, sizeof(S));
		return true;
	}
};


// Strings are handled like buffer
template<>
class CTGOR_Pack<FString>
{
public:
	static FString DataType() { return "String"; }
	static void DataWrite(UTGOR_Context* Context, CTGOR_Database* Database, uint32 Position, uint32& Size, const FString& In)
	{
		Size = (uint32)(In.Len() * sizeof(TCHAR));
		Database->Write(Position, (const uint8*)*In, Size);
	}
	static bool DataRead(UTGOR_Context* Context, const CTGOR_Database* Database, uint32 Position, uint32 Size, FString& Out)
	{
		const uint32 Count = Size / sizeof(TCHAR);
		Out = FString::ChrN(Count, ' ');
		Database->Read(Position, (uint8*)Out.GetCharArray().GetData(), Size);
		return true;
	}
};

template<>
class CTGOR_Arch<FString>
{
public:
	static void DataWrite(UTGOR_Context* Context, CTGOR_Archive* Archive, const FString& In)
	{
		const uint16 Size = (uint16)In.Len();
		CTGOR_Arch<uint16>::DataWrite(Context, Archive, Size);
		Archive->Write((const uint8*)*In, (uint32)(Size * sizeof(TCHAR)));
	}
	static bool DataRead(UTGOR_Context* Context, const CTGOR_Archive* Archive, FString& Out)
	{
		uint16 Size = 0;
		CTGOR_Arch<uint16>::DataRead(Context, Archive, Size);
		Out = FString::ChrN(Size, ' ');
		Archive->Read((uint8*)Out.GetCharArray().GetData(), (uint32)(Size * sizeof(TCHAR)));
		return true;
	}
};


// Quaternions can be stored with 3 floats
template<>
class CTGOR_Pack<FQuat>
{
public:
	static FString DataType() { return "Quat"; }
	static void DataWrite(UTGOR_Context* Context, CTGOR_Database* Database, uint32 Position, uint32& Size, const FQuat& In)
	{
		const FQuat Quat = In.GetNormalized();
		const bool Negative = Quat.W < 0.0;
		CTGOR_Pack<double>::DataWrite(Context, Database, Position+0, Size, Quat.X);
		CTGOR_Pack<double>::DataWrite(Context, Database, Position+4, Size, Quat.Y);
		CTGOR_Pack<double>::DataWrite(Context, Database, Position+8, Size, Quat.Z);
		CTGOR_Pack<bool>::DataWrite(Context, Database, Position+12, Size, Negative);
		Size = 13;
	}
	static bool DataRead(UTGOR_Context* Context, const CTGOR_Database* Database, uint32 Position, uint32 Size, FQuat& Out)
	{
		bool Negative;
		CTGOR_Pack<double>::DataRead(Context, Database, Position + 0, 4, Out.X);
		CTGOR_Pack<double>::DataRead(Context, Database, Position + 4, 4, Out.Y);
		CTGOR_Pack<double>::DataRead(Context, Database, Position + 8, 4, Out.Z);
		CTGOR_Pack<bool>::DataRead(Context, Database, Position + 12, 1, Negative);
		Out.W = FMath::Sqrt(1.0 - Out.X * Out.X - Out.Y * Out.Y - Out.Z * Out.Z);
		if (Negative) Out.W *= -1;
		return true;
	}
};

template<>
class CTGOR_Arch<FQuat>
{
public:
	static void DataWrite(UTGOR_Context* Context, CTGOR_Archive* Archive, const FQuat& In)
	{
		// When normalised, quaternion can be expressed with 3 numbers
		const FQuat Quat = In.GetNormalized();
		const bool Sgn = Quat.W < 0.0;
		uint16 X = FMath::RoundToInt((Quat.X + 1.0) * INT16_MAX) & 0xFFFF;
		uint16 Y = FMath::RoundToInt((Quat.Y + 1.0) * INT16_MAX) & 0xFFFF;
		uint16 Z = FMath::RoundToInt((Quat.Z + 1.0) * INT16_MAX) & 0xFFFF;
		CTGOR_Arch<uint16>::DataWrite(Context, Archive, X);
		CTGOR_Arch<uint16>::DataWrite(Context, Archive, Y);
		CTGOR_Arch<uint16>::DataWrite(Context, Archive, Z);
		CTGOR_Arch<bool>::DataWrite(Context, Archive, Sgn);
	}
	static bool DataRead(UTGOR_Context* Context, const CTGOR_Archive* Archive, FQuat& Out)
	{
		uint16 X, Y, Z;
		bool Sgn;
		CTGOR_Arch<uint16>::DataRead(Context, Archive, X);
		CTGOR_Arch<uint16>::DataRead(Context, Archive, Y);
		CTGOR_Arch<uint16>::DataRead(Context, Archive, Z);
		CTGOR_Arch<bool>::DataRead(Context, Archive, Sgn);
		Out.X = ((double)X) / INT16_MAX - 1.0;
		Out.Y = ((double)Y) / INT16_MAX - 1.0;
		Out.Z = ((double)Z) / INT16_MAX - 1.0;
		Out.W = FMath::Sqrt(FMath::Max(1.0 - Out.X * Out.X - Out.Y * Out.Y - Out.Z * Out.Z, 0.0));
		if (Sgn) Out.W *= -1;
		return true;
	}
};

// Vectors can be quantized
template<>
class CTGOR_Pack<FVector>
{
public:
	static FString DataType() { return "Vector"; }
	static void DataWrite(UTGOR_Context* Context, CTGOR_Database* Database, uint32 Position, uint32& Size, const FVector& In)
	{
		Size = sizeof(FVector);
		Database->Write(Position, (const uint8*)&In, Size);
	}
	static bool DataRead(UTGOR_Context* Context, const CTGOR_Database* Database, uint32 Position, uint32 Size, FVector& Out)
	{
		Database->Read(Position, (uint8*)&Out, Size);
		return true;
	}
};

template<>
class CTGOR_Arch<FVector>
{
public:
	static void DataWrite(UTGOR_Context* Context, CTGOR_Archive* Archive, const FVector& In)
	{
		Archive->WriteVector(In);
	}
	static bool DataRead(UTGOR_Context* Context, const CTGOR_Archive* Archive, FVector& Out)
	{
		Archive->ReadVector(Out);
		return true;
	}
};

// Rotators can be quantized
template<>
class CTGOR_Pack<FRotator>
{
public:
	static FString DataType() { return "Rotator"; }
	static void DataWrite(UTGOR_Context* Context, CTGOR_Database* Database, uint32 Position, uint32& Size, const FRotator& In)
	{
		Size = sizeof(FRotator);
		Database->Write(Position, (const uint8*)&In, Size);
	}
	static bool DataRead(UTGOR_Context* Context, const CTGOR_Database* Database, uint32 Position, uint32 Size, FRotator& Out)
	{
		Database->Read(Position, (uint8*)&Out, Size);
		return true;
	}
};

template<>
class CTGOR_Arch<FRotator>
{
public:
	static void DataWrite(UTGOR_Context* Context, CTGOR_Archive* Archive, const FRotator& In)
	{
		Archive->WriteRotator(In);
	}
	static bool DataRead(UTGOR_Context* Context, const CTGOR_Archive* Archive, FRotator& Out)
	{
		Archive->ReadRotator(Out);
		return true;
	}
};

// Transform stores everything individually
template<>
class CTGOR_Pack<FTransform>
{
public:
	static FString DataType() { return "Transform"; }
	static void DataWrite(UTGOR_Context* Context, CTGOR_Database* Database, uint32 Position, uint32& Size, const FTransform& In)
	{
		const FQuat Rotation = In.GetRotation();
		const FVector Location = In.GetLocation();
		const FVector Scale = In.GetScale3D();

		uint32 TempSize = 0;
		CTGOR_Pack<FQuat>::DataWrite(Context, Database, Position + 0, Size, Rotation);
		CTGOR_Pack<FVector>::DataWrite(Context, Database, Position + 13, Size, Location);
		CTGOR_Pack<FVector>::DataWrite(Context, Database, Position + 25, Size, Scale);
	}
	static bool DataRead(UTGOR_Context* Context, const CTGOR_Database* Database, uint32 Position, uint32 Size, FTransform& Out)
	{
		FQuat Rotation;
		FVector Location;
		FVector Scale;

		CTGOR_Pack<FQuat>::DataRead(Context, Database, Position + 0, 13, Rotation);
		CTGOR_Pack<FVector>::DataRead(Context, Database, Position + 13, 12, Location);
		CTGOR_Pack<FVector>::DataRead(Context, Database, Position + 25, 12, Scale);

		Out.SetRotation(Rotation);
		Out.SetLocation(Location);
		Out.SetScale3D(Scale);
		return true;
	}
};

template<>
class CTGOR_Arch<FTransform>
{
public:
	static void DataWrite(UTGOR_Context* Context, CTGOR_Archive* Archive, const FTransform& In)
	{
		const FQuat Rotation = In.GetRotation();
		const FVector Location = In.GetLocation();
		const FVector Scale = In.GetScale3D();

		uint32 TempSize = 0;
		CTGOR_Arch<FQuat>::DataWrite(Context, Archive, Rotation);
		CTGOR_Arch<FVector>::DataWrite(Context, Archive, Location);
		CTGOR_Arch<FVector>::DataWrite(Context, Archive, Scale);
	}
	static bool DataRead(UTGOR_Context* Context, const CTGOR_Archive* Archive, FTransform& Out)
	{
		FQuat Rotation;
		FVector Location;
		FVector Scale;

		CTGOR_Arch<FQuat>::DataRead(Context, Archive, Rotation);
		CTGOR_Arch<FVector>::DataRead(Context, Archive, Location);
		CTGOR_Arch<FVector>::DataRead(Context, Archive, Scale);

		Out.SetRotation(Rotation);
		Out.SetLocation(Location);
		Out.SetScale3D(Scale);
		return true;
	}
};


// Strings are handled like buffer
template<>
class CTGOR_Pack<FName>
{
public:
	static FString DataType() { return "Name"; }
	static void DataWrite(UTGOR_Context* Context, CTGOR_Database* Database, uint32 Position, uint32& Size, const FName& In)
	{
		const FString String = In.ToString();
		CTGOR_Pack<FString>::DataWrite(Context, Database, Position, Size, String);
	}
	static bool DataRead(UTGOR_Context* Context, const CTGOR_Database* Database, uint32 Position, uint32 Size, FName& Out)
	{
		FString String = "";
		const bool Success = CTGOR_Pack<FString>::DataRead(Context, Database, Position, Size, String);
		Out = *String;
		return Success;
	}
};

template<>
class CTGOR_Arch<FName>
{
public:
	static void DataWrite(UTGOR_Context* Context, CTGOR_Archive* Archive, const FName& In)
	{
		const FString String = In.ToString();
		CTGOR_Arch<FString>::DataWrite(Context, Archive, String);
	}
	static bool DataRead(UTGOR_Context* Context, const CTGOR_Archive* Archive, FName& Out)
	{
		FString String = "";
		const bool Success = CTGOR_Arch<FString>::DataRead(Context, Archive, String);
		Out = *String;
		return Success;
	}
};

// Enum types just cast to byte
template<typename S>
class CTGOR_Pack<S, typename std::enable_if_t<std::is_enum<S>::value>>
{
public:
	static FString DataType() { return "Enum"; }
	static void DataWrite(UTGOR_Context* Context, CTGOR_Database* Database, uint32 Position, uint32& Size, const S& In)
	{
		const uint8 Byte = (uint8)In;
		CTGOR_Pack<uint8>::DataWrite(Context, Database, Position, Size, Byte);
	}
	static bool DataRead(UTGOR_Context* Context, const CTGOR_Database* Database, uint32 Position, uint32 Size, S& Out)
	{
		uint8 Byte = 0;
		CTGOR_Pack<uint8>::DataRead(Context, Database, Position, Size, Byte);
		Out = (S)Byte;
		return true;
	}
};

template<typename S>
class CTGOR_Arch<S, typename std::enable_if_t<std::is_enum<S>::value>>
{
public:
	static void DataWrite(UTGOR_Context* Context, CTGOR_Archive* Archive, const S& In)
	{
		const uint8 Byte = (uint8)In;
		CTGOR_Arch<uint8>::DataWrite(Context, Archive, Byte);
	}
	static bool DataRead(UTGOR_Context* Context, const CTGOR_Archive* Archive, S& Out)
	{
		uint8 Byte = 0;
		CTGOR_Arch<uint8>::DataRead(Context, Archive, Byte);
		Out = (S)Byte;
		return true;
	}
};

// Finding serilisation function parameters
template <class F> struct ArgType;
template <class S, class T> struct ArgType<void(S::*)(T, UTGOR_Context*)> { typedef T type; };
template <class S, class T> struct ArgType<void(S::*)(T, UTGOR_Context*) const> { typedef T type;};

// Check for Read/Write functions
template<class S, typename = void>
struct is_serialisable : std::false_type {};

template<class S>
struct is_serialisable<S, typename std::enable_if_t<
	std::is_same<typename ArgType<decltype(&S::Write)>::type, FTGOR_GroupWritePackage&>::value &&
	std::is_same<typename ArgType<decltype(&S::Read)>::type, FTGOR_GroupReadPackage&>::value>> : std::true_type {};

// Check for Load/Save functions
template<class S, typename = void>
struct is_networkable : std::false_type {};

template<class S>
struct is_networkable<S, typename std::enable_if_t<
	std::is_same<typename ArgType<decltype(&S::Send)>::type, FTGOR_NetworkWritePackage&>::value &&
	std::is_same<typename ArgType<decltype(&S::Recv)>::type, FTGOR_NetworkReadPackage&>::value>> : std::true_type {};


// Check for cache member
template<class S, typename = void>
struct has_cache : std::false_type 
{
	static void ReadCached(FTGOR_GroupWritePackage& Package, const S& In) {}
	static void WriteCached(FTGOR_GroupReadPackage& Package, S& Out) {}
};

template<class S>
struct has_cache<S, typename std::enable_if_t<
	std::is_same<typename decltype(S::LegacyCache), CTGOR_GroupPackageCache>::value>> : std::true_type
{
	static void ReadCached(FTGOR_GroupWritePackage& Package, const S& In)
	{
		Package.Package.ReadCached(In.LegacyCache);
	}
	static void WriteCached(FTGOR_GroupReadPackage& Package, S& Out)
	{
		Package.Package.WriteCached(Out.LegacyCache);
	}
};

// Check for typename declaration
template<class S, typename = void>
struct has_typename : std::false_type 
{
	static FString GetType(FString Default) { return Default; }
};

template<class S>
struct has_typename<S, typename std::enable_if_t<
	std::is_same<typename decltype(S::TYPENAME), const char*>::value>> : std::true_type 
{
	static FString GetType(FString Default) { return Default + S::TYPENAME; }
};

// Recursively serialises serialisable objects that implement Write and Read
template<typename S>
class CTGOR_Pack<S, typename std::enable_if_t<is_serialisable<S>::value>>
{
public:
	static FString DataType() { return has_typename<S>::GetType("Group"); }
	static void DataWrite(UTGOR_Context* Context, CTGOR_Database* Database, uint32 Position, uint32& Size, const S& In)
	{
		FTGOR_GroupWritePackage Package(Database, Context, Position);
		In.Write(Package, Context); // Write content to database
		has_cache<S>::ReadCached(Package, In); // Read cache buffer as last register into database
		Package.Package.WriteRegister(Size); // Write register into database
	}
	static bool DataRead(UTGOR_Context* Context, const CTGOR_Database* Database, uint32 Position, uint32 Size, S& Out)
	{
		FTGOR_GroupReadPackage Package(Database, Context, Position);
		Package.Package.ReadRegister(Size); // Read register from database
		Out.Read(Package, Context); // Read content from database
		has_cache<S>::WriteCached(Package, Out); // Generate cache from database of un-read/failed register entries
		return true;
	}
};

template<typename S>
class CTGOR_Arch<S, typename std::enable_if_t<is_networkable<S>::value>>
{
public:
	static void DataWrite(UTGOR_Context* Context, CTGOR_Archive* Archive, const S& In)
	{
		FTGOR_NetworkWritePackage Package(Archive, Context);
		In.Send(Package, Context); // Write content to database
	}
	static bool DataRead(UTGOR_Context* Context, const CTGOR_Archive* Archive, S& Out)
	{
		FTGOR_NetworkReadPackage Package(Archive, Context);
		Out.Recv(Package, Context); // Read content from database
		return true;
	}
};

template<typename S>
class CTGOR_Pack<S*, typename std::enable_if_t<std::is_base_of<ITGOR_SaveInterface, S>::value>>
{
public:
	static FString DataType() { return has_typename<S>::GetType("Group"); }
	static void DataWrite(UTGOR_Context* Context, CTGOR_Database* Database, uint32 Position, uint32& Size, S* const& In)
	{
		FTGOR_GroupWritePackage Package(Database, Context, Position);
		In->Execute_Write(In, Package, Context); // Write content to database
		has_cache<ITGOR_SaveInterface>::ReadCached(Package, *In); // Read cache buffer as last register into database
		Package.Package.WriteRegister(Size); // Write register into database
	}
	static bool DataRead(UTGOR_Context* Context, const CTGOR_Database* Database, uint32 Position, uint32 Size, S*& Out)
	{
		FTGOR_GroupReadPackage Package(Database, Context, Position);
		Package.Package.ReadRegister(Size); // Read register from database
		Out->Execute_Read(Out, Package, Context); // Read content from database
		has_cache<ITGOR_SaveInterface>::WriteCached(Package, *Out); // Generate cache from database of un-read/failed register entries
		return true;
	}
};

template<typename S>
class CTGOR_Arch<S*, typename std::enable_if_t<std::is_base_of<ITGOR_NetInterface, S>::value>>
{
public:
	static void DataWrite(UTGOR_Context* Context, CTGOR_Archive* Archive, S* const& In)
	{
		FTGOR_NetworkWritePackage Package(Archive, Context);
		In->Execute_Send(In, Package, Context); // Write content to database
	}
	static bool DataRead(UTGOR_Context* Context, const CTGOR_Archive* Archive, S*& Out)
	{
		FTGOR_NetworkReadPackage Package(Archive, Context);
		Out->Execute_Recv(Out, Package, Context); // Read content from database
		return true;
	}
};


// Recursively serialises interface objects that implement Write and Read
template<>
class CTGOR_Pack<TScriptInterface<ITGOR_SaveInterface>>
{
public:
	static FString DataType() { return "Interface"; }
	static void DataWrite(UTGOR_Context* Context, CTGOR_Database* Database, uint32 Position, uint32& Size, const TScriptInterface<ITGOR_SaveInterface>& In)
	{
		FTGOR_GroupWritePackage Package(Database, Context, Position);
		In->Execute_Write(In.GetObject(), Package, Context); // Write content to database
		if (In) has_cache<ITGOR_SaveInterface>::ReadCached(Package, *In); // Read cache buffer as last register into database
		Package.Package.WriteRegister(Size);  // Write register into database
	}
	static bool DataRead(UTGOR_Context* Context, const CTGOR_Database* Database, uint32 Position, uint32 Size, TScriptInterface<ITGOR_SaveInterface>& Out)
	{
		FTGOR_GroupReadPackage Package(Database, Context, Position);
		Package.Package.ReadRegister(Size); // Read register from database
		Out->Execute_Read(Out.GetObject(), Package, Context); // Read content from database
		if (Out) has_cache<ITGOR_SaveInterface>::WriteCached(Package, *Out); // Generate cache from database of un-read/failed register entries
		return true;
	}
};

template<>
class CTGOR_Pack<TScriptInterface<ITGOR_NetInterface>>
{
public:
	static void DataWrite(UTGOR_Context* Context, CTGOR_Archive* Archive, const TScriptInterface<ITGOR_NetInterface>& In)
	{
		FTGOR_NetworkWritePackage Package(Archive, Context);
		In->Execute_Send(In.GetObject(), Package, Context); // Write content to database
	}
	static bool DataRead(UTGOR_Context* Context, const CTGOR_Archive* Archive, TScriptInterface<ITGOR_NetInterface>& Out)
	{
		FTGOR_NetworkReadPackage Package(Archive, Context);
		Out->Execute_Recv(Out.GetObject(), Package, Context); // Read content from database
		return true;
	}
};

class UTGOR_ContentManager;

// Serialises content
template<typename S>
class CTGOR_Pack<S*, typename std::enable_if_t<std::is_base_of<UTGOR_Content, S>::value>>
{
public:
	static FString DataType() { return "Content"; }
	static void DataWrite(UTGOR_Context* Context, CTGOR_Database* Database, uint32 Position, uint32& Size, S* const& In)
	{
		if (IsValid(In))
		{
			// Serialise identifier and mod
			const FString Identifier = In->GetDefaultName();
			const uint32 IdentifierSerial = Database->SerializeIdentifier(Identifier);
			const FString Mod = In->GetModIdentifier();
			const uint32 ModSerial = Database->SerializeIdentifier(Identifier);

			// Write to buffer
			Database->Write(Position, (const uint8*)&IdentifierSerial, sizeof(uint32));
			Database->Write(Position + sizeof(uint32), (const uint8*)&ModSerial, sizeof(uint32));
			Size = 2 * sizeof(uint32);
		}
		else
		{
			Size = 0;
		}
	}
	static bool DataRead(UTGOR_Context* Context, const CTGOR_Database* Database, uint32 Position, uint32 Size, S*& Out)
	{
		if (Size <= 0)
		{
			Out = nullptr;
			return true;
		}
		else if (Size == 2 * sizeof(uint32))
		{
			// Read from buffer
			uint32 IdentifierSerial, ModSerial;
			Database->Read(Position, (uint8*)&IdentifierSerial, sizeof(uint32));
			Database->Read(Position + sizeof(uint32), (uint8*)&ModSerial, sizeof(uint32));

			// Serialise identifier and mod
			const FString Identifier = Database->GetIdentifier(IdentifierSerial);
			const FString Mod = Database->GetIdentifier(ModSerial);

			UTGOR_ContentManager* ContentManager = Context->GetContentManager();
			Out = ContentManager->GetTFromName<S>(Identifier);
			return IsValid(Out);
		}
		return false;
	}
};

template<typename S>
class CTGOR_Arch<S*, typename std::enable_if_t<std::is_base_of<UTGOR_Content, S>::value>>
{
public:
	static void DataWrite(UTGOR_Context* Context, CTGOR_Archive* Archive, S* const& In)
	{
		const bool Valid = IsValid(In);
		CTGOR_Arch<bool>::DataWrite(Context, Archive, Valid);
		if (Valid)
		{
			UTGOR_ContentManager* ContentManager = Context->GetContentManager();
			const int Size = ContentManager->GetTCapacity<S>();
			const uint32 Bits = FMath::Max<uint32>(FMath::CeilLogTwo(Size), 1);

			const int16 Serial = ContentManager->GetTIndex<S>(In);
			//CTGOR_Arch<int16>::DataWrite(Context, Archive, Serial);
			Archive->WriteBits((const uint8*)&Serial, Bits);
		}
	}
	static bool DataRead(UTGOR_Context* Context, const CTGOR_Archive* Archive, S*& Out)
	{
		bool Valid = false;
		CTGOR_Arch<bool>::DataRead(Context, Archive, Valid);
		if (Valid)
		{
			UTGOR_ContentManager* ContentManager = Context->GetContentManager();
			const int Size = ContentManager->GetTCapacity<S>();
			const uint32 Bits = FMath::Max<uint32>(FMath::CeilLogTwo(Size), 1);

			int16 Serial = 0;
			//CTGOR_Arch<int16>::DataRead(Context, Archive, Serial);
			Archive->ReadBits((uint8*)&Serial, Bits);
			Out = ContentManager->GetTFromIndex<S>(Serial);
		}
		else
		{
			Out = nullptr;
		}
		return true;
	}
};


template<typename S>
class CTGOR_Pack<TObjectPtr<S>, typename std::enable_if_t<std::is_base_of<UTGOR_Content, S>::value>>
{
public:
	static FString DataType() { return "Content"; }
	static void DataWrite(UTGOR_Context* Context, CTGOR_Database* Database, uint32 Position, uint32& Size, const TObjectPtr<S>& In)
	{
		if (IsValid(In))
		{
			// Serialise identifier and mod
			const FString Identifier = In->GetDefaultName();
			const uint32 IdentifierSerial = Database->SerializeIdentifier(Identifier);
			const FString Mod = In->GetModIdentifier();
			const uint32 ModSerial = Database->SerializeIdentifier(Identifier);

			// Write to buffer
			Database->Write(Position, (const uint8*)&IdentifierSerial, sizeof(uint32));
			Database->Write(Position + sizeof(uint32), (const uint8*)&ModSerial, sizeof(uint32));
			Size = 2 * sizeof(uint32);
		}
		else
		{
			Size = 0;
		}
	}
	static bool DataRead(UTGOR_Context* Context, const CTGOR_Database* Database, uint32 Position, uint32 Size, TObjectPtr<S>& Out)
	{
		if (Size <= 0)
		{
			Out = nullptr;
			return true;
		}
		else if (Size == 2 * sizeof(uint32))
		{
			// Read from buffer
			uint32 IdentifierSerial, ModSerial;
			Database->Read(Position, (uint8*)&IdentifierSerial, sizeof(uint32));
			Database->Read(Position + sizeof(uint32), (uint8*)&ModSerial, sizeof(uint32));

			// Serialise identifier and mod
			const FString Identifier = Database->GetIdentifier(IdentifierSerial);
			const FString Mod = Database->GetIdentifier(ModSerial);

			UTGOR_ContentManager* ContentManager = Context->GetContentManager();
			Out = ContentManager->GetTFromName<S>(Identifier);
			return IsValid(Out);
		}
		return false;
	}
};

template<typename S>
class CTGOR_Arch<TObjectPtr<S>, typename std::enable_if_t<std::is_base_of<UTGOR_Content, S>::value>>
{
public:
	static void DataWrite(UTGOR_Context* Context, CTGOR_Archive* Archive, const TObjectPtr<S>& In)
	{
		const bool Valid = IsValid(In);
		CTGOR_Arch<bool>::DataWrite(Context, Archive, Valid);
		if (Valid)
		{
			UTGOR_ContentManager* ContentManager = Context->GetContentManager();
			const int Size = ContentManager->GetTCapacity<S>();
			const uint32 Bits = FMath::Max<uint32>(FMath::CeilLogTwo(Size), 1);

			const int16 Serial = ContentManager->GetTIndex<S>(In);
			//CTGOR_Arch<int16>::DataWrite(Context, Archive, Serial);
			Archive->WriteBits((const uint8*)&Serial, Bits);
		}
	}
	static bool DataRead(UTGOR_Context* Context, const CTGOR_Archive* Archive, TObjectPtr<S>& Out)
	{
		bool Valid = false;
		CTGOR_Arch<bool>::DataRead(Context, Archive, Valid);
		if (Valid)
		{
			UTGOR_ContentManager* ContentManager = Context->GetContentManager();
			const int Size = ContentManager->GetTCapacity<S>();
			const uint32 Bits = FMath::Max<uint32>(FMath::CeilLogTwo(Size), 1);

			int16 Serial = 0;
			//CTGOR_Arch<int16>::DataRead(Context, Archive, Serial);
			Archive->ReadBits((uint8*)&Serial, Bits);
			Out = ContentManager->GetTFromIndex<S>(Serial);
		}
		else
		{
			Out = nullptr;
		}
		return true;
	}
};

// Object pointer forward
template<typename S>
class CTGOR_Pack<S*, typename std::enable_if_t<!std::is_base_of<UTGOR_Content, S>::value && !std::is_base_of<ITGOR_SaveInterface, S>::value>>
{
public:
	static FString DataType() { return CTGOR_Pack<S>::DataType() + "Pointer"; }
	static void DataWrite(UTGOR_Context* Context, CTGOR_Database* Database, uint32 Position, uint32& Size, const S* In)
	{
		if (In)
		{
			CTGOR_Pack<S>::DataWrite(Context, Database, Position, Size, *In);
		}
	}
	static bool DataRead(UTGOR_Context* Context, const CTGOR_Database* Database, uint32 Position, uint32 Size, S* Out)
	{
		if (Out)
		{
			return CTGOR_Pack<S>::DataRead(Context, Database, Position, Size, *Out);
		}
		return false;
	}
};

template<typename S>
class CTGOR_Arch<S*, typename std::enable_if_t<!std::is_base_of<UTGOR_Content, S>::value && !std::is_base_of<ITGOR_NetInterface, S>::value>>
{
public:
	static void DataWrite(UTGOR_Context* Context, CTGOR_Archive* Archive, const S* In)
	{
		if (In)
		{
			CTGOR_Arch<S>::DataWrite(Context, Archive, *In);
		}
	}
	static bool DataRead(UTGOR_Context* Context, const CTGOR_Archive* Archive, S* Out)
	{
		if (Out)
		{
			return CTGOR_Arch<S>::DataRead(Context, Archive, *Out);
		}
		return false;
	}
};

template<typename S>
class CTGOR_Pack<TObjectPtr<S>, typename std::enable_if_t<!std::is_base_of<UTGOR_Content, S>::value && !std::is_base_of<ITGOR_SaveInterface, S>::value>>
{
public:
	static FString DataType() { return CTGOR_Pack<S>::DataType() + "Pointer"; }
	static void DataWrite(UTGOR_Context* Context, CTGOR_Database* Database, uint32 Position, uint32& Size, const TObjectPtr<S>& In)
	{
		if (In)
		{
			CTGOR_Pack<S>::DataWrite(Context, Database, Position, Size, *In);
		}
	}
	static bool DataRead(UTGOR_Context* Context, const CTGOR_Database* Database, uint32 Position, uint32 Size, TObjectPtr<S>& Out)
	{
		if (Out)
		{
			return CTGOR_Pack<S>::DataRead(Context, Database, Position, Size, *Out);
		}
		return false;
	}
};

template<typename S>
class CTGOR_Arch<TObjectPtr<S>, typename std::enable_if_t<!std::is_base_of<UTGOR_Content, S>::value && !std::is_base_of<ITGOR_NetInterface, S>::value>>
{
public:
	static void DataWrite(UTGOR_Context* Context, CTGOR_Archive* Archive, const TObjectPtr<S>& In)
	{
		if (In)
		{
			CTGOR_Arch<S>::DataWrite(Context, Archive, *In);
		}
	}
	static bool DataRead(UTGOR_Context* Context, const CTGOR_Archive* Archive, TObjectPtr<S>& Out)
	{
		if (Out)
		{
			return CTGOR_Arch<S>::DataRead(Context, Archive, *Out);
		}
		return false;
	}
};

// Object pointer mapping
// WEAK POINTER ONLY SUPPORTED FOR FILE STORAGE IF OWNER HAS AN IDENTITY
template<typename S>
class CTGOR_Pack<TWeakObjectPtr<S>, typename std::enable_if_t<std::is_base_of<AActor, S>::value>>
{
public:
	static FString DataType() { return "Actor"; }
	static void DataWrite(UTGOR_Context* Context, CTGOR_Database* Database, uint32 Position, uint32& Size, const TWeakObjectPtr<S>& In)
	{
		Size = 0;

		// Fetch
		ETGOR_FetchEnumeration State;
		UTGOR_WorldData* WorldData = Context->GetWorldData();
		FTGOR_DimensionIdentifier Identifier = WorldData->GetIdentifier(In.Get(), State);
		if (State == ETGOR_FetchEnumeration::Found)
		{
			// Write
			CTGOR_Pack<FTGOR_DimensionIdentifier>::DataWrite(Context, Database, Position, Size, Identifier);
		}
	}
	static bool DataRead(UTGOR_Context* Context, const CTGOR_Database* Database, uint32 Position, uint32 Size, TWeakObjectPtr<S>& Out)
	{
		if (Size == 0)
		{
			Out.Reset();
			return true;
		}

		// Read
		FTGOR_DimensionIdentifier Identifier;
		CTGOR_Pack<FTGOR_DimensionIdentifier>::DataRead(Context, Database, Position, Size, Identifier);

		// Apply
		UTGOR_WorldData* WorldData = Context->GetWorldData();
		Out = WorldData->GetIdentityActor<S>(Identifier);
		return Out.IsValid();
	}
};

template<typename S>
class CTGOR_Pack<TWeakObjectPtr<S>, typename std::enable_if_t<std::is_base_of<UActorComponent, S>::value>>
{
public:
	static FString DataType() { return "Component"; }
	static void DataWrite(UTGOR_Context* Context, CTGOR_Database* Database, uint32 Position, uint32& Size, const TWeakObjectPtr<S>& In)
	{
		Size = 0;

		// Fetch
		ETGOR_FetchEnumeration State;
		UTGOR_WorldData* WorldData = Context->GetWorldData();
		FTGOR_ComponentIdentifier Identifier = WorldData->GetComponentIdentifier(In.Get(), State);
		if (State == ETGOR_FetchEnumeration::Found)
		{
			// Write
			CTGOR_Pack<FTGOR_ComponentIdentifier>::DataWrite(Context, Database, Position, Size, Identifier);
		}
	}
	static bool DataRead(UTGOR_Context* Context, const CTGOR_Database* Database, uint32 Position, uint32 Size, TWeakObjectPtr<S>& Out)
	{
		if (Size == 0)
		{
			Out.Reset();
			return true;
		}

		// Read
		FTGOR_ComponentIdentifier Identifier;
		CTGOR_Pack<FTGOR_ComponentIdentifier>::DataRead(Context, Database, Position, Size, Identifier);

		// Apply
		UTGOR_WorldData* WorldData = Context->GetWorldData();
		Out = WorldData->GetIdentityComponent<S>(Identifier);
		return Out.IsValid();
	}
};


template<typename S>
class CTGOR_Arch<TWeakObjectPtr<S>>
{
public:
	static void DataWrite(UTGOR_Context* Context, CTGOR_Archive* Archive, const TWeakObjectPtr<S>& In)
	{
		const bool Valid = In.IsValid();
		CTGOR_Arch<bool>::DataWrite(Context, Archive, Valid);
		if (In.IsValid())
		{
			UObject* Object = In.Get();
			Archive->Map<S>(Object);
		}
	}
	static bool DataRead(UTGOR_Context* Context, const CTGOR_Archive* Archive, TWeakObjectPtr<S>& Out)
	{
		bool Valid = false;
		if (CTGOR_Arch<bool>::DataRead(Context, Archive, Valid))
		{
			if (Valid)
			{
				UObject* Object = nullptr;
				Archive->Map<S>(Object);
				Out = TWeakObjectPtr<S>(Cast<S>(Object));
			}
			else
			{
				Out = nullptr;
			}
			return true;
		}
		return false;
	}
};


// Serialises buffer (can be stored more efficiently than other array types)
template<typename S>
class CTGOR_Pack<TArray<S>, typename std::enable_if_t<is_compressible<S>::value>>
{
public:
	static FString DataType() { return CTGOR_Pack<S>::DataType() + "Buffer"; }
	static void DataWrite(UTGOR_Context* Context, CTGOR_Database* Database, uint32 Position, uint32& Size, const TArray<S>& In)
	{
		Size = sizeof(S) * In.Num();
		if (Size > 0)
		{
			Database->Write(Position, (const uint8*)In.GetData(), Size);
		}
	}
	static bool DataRead(UTGOR_Context* Context, const CTGOR_Database* Database, uint32 Position, uint32 Size, TArray<S>& Out)
	{
		Out.SetNum(Size / sizeof(S));
		if (Size > 0)
		{
			Database->Read(Position, (uint8*)Out.GetData(), Size);
		}
		return true;
	}
};

template<typename S>
class CTGOR_Arch<TArray<S>, typename std::enable_if_t<is_compressible<S>::value>>
{
public:
	static void DataWrite(UTGOR_Context* Context, CTGOR_Archive* Archive, const TArray<S>& In)
	{
		const uint16 Size = In.Num();
		CTGOR_Arch<uint16>::DataWrite(Context, Archive, Size);
		if (Size > 0)
		{
			Archive->Write((const uint8*)In.GetData(), sizeof(S) * Size);
		}
	}
	static bool DataRead(UTGOR_Context* Context, const CTGOR_Archive* Archive, TArray<S>& Out)
	{
		uint16 Size = 0;
		CTGOR_Arch<uint16>::DataRead(Context, Archive, Size);
		Out.SetNum(Size);
		if (Size > 0)
		{
			Archive->Read((uint8*)Out.GetData(), sizeof(S) * Size);
		}
		return true;
	}
};

// Serialises arrays of supported types that aren't compressible
template<typename S>
class CTGOR_Pack<TArray<S>, typename std::enable_if_t<!is_compressible<S>::value>>
{
public:
	static FString DataType() { return CTGOR_Pack<S>::DataType() + "Array"; }
	static void DataWrite(UTGOR_Context* Context, CTGOR_Database* Database, uint32 Position, uint32& Size, const TArray<S>& In)
	{
		FTGOR_ArrayWritePackage Package(Database, Context, Position);
		for (int32 Index = 0; Index < In.Num(); Index++)
		{
			Package.WriteEntry(Index, In[Index]);
		}
		Package.Package.WriteRegister(Size);
	}
	static bool DataRead(UTGOR_Context* Context, const CTGOR_Database* Database, uint32 Position, uint32 Size, TArray<S>& Out)
	{
		FTGOR_ArrayReadPackage Package(Database, Context, Position);
		Package.Package.ReadRegister(Size);
		Out.SetNum(Package.Package.GetRegisterCount());
		for (int32 Index = 0; Index < Out.Num(); Index++)
		{
			Package.ReadEntry(Index, Out[Index]);
		}
		return true;
	}
};

template<typename S>
class CTGOR_Arch<TArray<S>, typename std::enable_if_t<!is_compressible<S>::value>>
{
public:
	static void DataWrite(UTGOR_Context* Context, CTGOR_Archive* Archive, const TArray<S>& In)
	{
		const uint16 Size = In.Num();
		CTGOR_Arch<uint16>::DataWrite(Context, Archive, Size);

		FTGOR_NetworkWritePackage Package(Archive, Context);
		for (const S& s : In)
		{
			Package.WriteEntry(s);
		}
	}
	static bool DataRead(UTGOR_Context* Context, const CTGOR_Archive* Archive, TArray<S>& Out)
	{
		uint16 Size = 0;
		CTGOR_Arch<uint16>::DataRead(Context, Archive, Size);
		Out.SetNum(Size);

		FTGOR_NetworkReadPackage Package(Archive, Context);
		for (S& s : Out)
		{
			Package.ReadEntry(s);
		}
		return true;
	}
};

// Serialises sets of supported types that aren't compressible
template<typename S>
class CTGOR_Pack<TSet<S>>
{
public:
	static FString DataType() { return CTGOR_Pack<S>::DataType() + "Set"; }
	static void DataWrite(UTGOR_Context* Context, CTGOR_Database* Database, uint32 Position, uint32& Size, const TSet<S>& In)
	{
		CTGOR_Pack<TArray<S>>::DataWrite(Context, Database, Position, Size, In.Array());
	}
	static bool DataRead(UTGOR_Context* Context, const CTGOR_Database* Database, uint32 Position, uint32 Size, TSet<S>& Out)
	{
		TArray<S> Array;
		CTGOR_Pack<TArray<S>>::DataRead(Context, Database, Position, Size, Array);
		Out = TSet<S>(Array);
		return true;
	}
};

template<typename S>
class CTGOR_Arch<TSet<S>, typename std::enable_if_t<!is_compressible<S>::value>>
{
public:
	static void DataWrite(UTGOR_Context* Context, CTGOR_Archive* Archive, const TSet<S>& In)
	{
		CTGOR_Pack<TArray<S>>::DataWrite(Context, Archive, In.Array());
	}
	static bool DataRead(UTGOR_Context* Context, const CTGOR_Archive* Archive, TSet<S>& Out)
	{
		TArray<S> Array;
		CTGOR_Pack<TArray<S>>::DataRead(Context, Archive, Array);
		Out = TSet<S>(Array);
		return true;
	}
};

// Serialises maps of supported types
template<typename R, typename S>
class CTGOR_Pack<TMap<R, S>>
{
public:
	static FString DataType() { return CTGOR_Pack<R>::DataType() + CTGOR_Pack<S>::DataType() + "Map"; }
	static void DataWrite(UTGOR_Context* Context, CTGOR_Database* Database, uint32 Position, uint32& Size, const TMap<R, S>& In)
	{
		FTGOR_MapWritePackage Package(Database, Context, Position);
		int32 Index = 0;
		for (const TTuple<R, S>& Pair : In)
		{
			Package.WriteEntry(Index, Pair.Key, Pair.Value);
			Index++;
		}
		Package.Package.WriteRegister(Size);
	}
	static bool DataRead(UTGOR_Context* Context, const CTGOR_Database* Database, uint32 Position, uint32 Size, TMap<R, S>& Out)
	{
		FTGOR_MapReadPackage Package(Database, Context, Position);
		Package.Package.ReadRegister(Size);

		TMap<R, S> Map;
		const int32 Num = Package.Package.GetRegisterCount();
		for (int32 Index = 0; Index < Num; Index++)
		{
			TTuple<R, S> Pair;
			Package.ReadKey(Index, Pair.Key);

			// Copy from previous map if exists so unserialised properties aren't lost
			S* Ptr = Out.Find(Pair.Key);
			if (Ptr) Pair.Value = *Ptr;
			Package.ReadValue(Index, Pair.Value);
			Map.Add(Pair);
		}
		Out = Map;
		return true;
	}
};

template<typename R, typename S>
class CTGOR_Arch<TMap<R, S>>
{
public:
	static void DataWrite(UTGOR_Context* Context, CTGOR_Archive* Archive, const TMap<R, S>& In)
	{
		const uint16 Size = In.Num();
		CTGOR_Arch<uint16>::DataWrite(Context, Archive, Size);

		FTGOR_NetworkWritePackage Package(Archive, Context);
		for (const TTuple<R, S>& Pair : In)
		{
			Package.WriteEntry(Pair.Key);
			Package.WriteEntry(Pair.Value);
		}
	}
	static bool DataRead(UTGOR_Context* Context, const CTGOR_Archive* Archive, TMap<R, S>& Out)
	{
		uint16 Size = 0;
		CTGOR_Arch<uint16>::DataRead(Context, Archive, Size);

		FTGOR_NetworkReadPackage Package(Archive, Context);
		TMap<R, S> Map;
		for (int32 Index = 0; Index < Size; Index++)
		{
			TTuple<R, S> Pair;
			Package.ReadEntry(Pair.Key);

			// Copy from previous map if exists so unserialised properties aren't lost
			S* Ptr = Out.Find(Pair.Key);
			if (Ptr) Pair.Value = *Ptr;
			Package.ReadEntry(Pair.Value);
			Map.Add(Pair);
		}
		Out = Map;
		return true;
	}
};


// Serialises byte buffer as special case, for now just relays to byte array
template<>
class CTGOR_Pack<FTGOR_Buffer>
{
public:
	static FString DataType() { return "Buffer"; }
	static void DataWrite(UTGOR_Context* Context, CTGOR_Database* Database, uint32 Position, uint32& Size, const FTGOR_Buffer& In)
	{
		CTGOR_Pack<TArray<uint8>>::DataWrite(Context, Database, Position, Size, In.Data);
	}
	static bool DataRead(UTGOR_Context* Context, const CTGOR_Database* Database, uint32 Position, uint32 Size, FTGOR_Buffer& Out)
	{
		return CTGOR_Pack<TArray<uint8>>::DataRead(Context, Database, Position, Size, Out.Data);
	}
};

template<>
class CTGOR_Arch<FTGOR_Buffer>
{
public:
	static void DataWrite(UTGOR_Context* Context, CTGOR_Archive* Archive, const FTGOR_Buffer& In)
	{
		CTGOR_Arch<TArray<uint8>>::DataWrite(Context, Archive, In.Data);
	}
	static bool DataRead(UTGOR_Context* Context, const CTGOR_Archive* Archive, FTGOR_Buffer& Out)
	{
		return CTGOR_Arch<TArray<uint8>>::DataRead(Context, Archive, Out.Data);
	}
};


// Timestamps
template<>
class CTGOR_Pack<FTGOR_Time>
{
public:
	static FString DataType() { return "Time"; }
	static void DataWrite(UTGOR_Context* Context, CTGOR_Database* Database, uint32 Position, uint32& Size, const FTGOR_Time& In)
	{
		CTGOR_Pack<float>::DataWrite(Context, Database, Position + 0, Size, In.Seconds);
		CTGOR_Pack<uint16>::DataWrite(Context, Database, Position + 4, Size, In.Epochs);
		Size = (uint32)(sizeof(float) + sizeof(uint16));
	}
	static bool DataRead(UTGOR_Context* Context, const CTGOR_Database* Database, uint32 Position, uint32 Size, FTGOR_Time& Out)
	{
		CTGOR_Pack<float>::DataRead(Context, Database, Position + 0, Size, Out.Seconds);
		CTGOR_Pack<uint16>::DataRead(Context, Database, Position + 4, Size, Out.Epochs);
		return true;
	}
};

template<>
class CTGOR_Arch<FTGOR_Time>
{
public:
	static void DataWrite(UTGOR_Context* Context, CTGOR_Archive* Archive, const FTGOR_Time& In)
	{
		CTGOR_Arch<float>::DataWrite(Context, Archive, In.Seconds);
		CTGOR_Arch<uint16>::DataWrite(Context, Archive, In.Epochs);
	}
	static bool DataRead(UTGOR_Context* Context, const CTGOR_Archive* Archive, FTGOR_Time& Out)
	{
		CTGOR_Arch<float>::DataRead(Context, Archive, Out.Seconds);
		CTGOR_Arch<uint16>::DataRead(Context, Archive, Out.Epochs);
		return true;
	}
};

// Normal (float in [0,1])
template<>
class CTGOR_Pack<FTGOR_Normal>
{
public:
	static FString DataType() { return "Normal"; }
	static void DataWrite(UTGOR_Context* Context, CTGOR_Database* Database, uint32 Position, uint32& Size, const FTGOR_Normal& In)
	{
		CTGOR_Pack<float>::DataWrite(Context, Database, Position, Size, In.Value);
	}
	static bool DataRead(UTGOR_Context* Context, const CTGOR_Database* Database, uint32 Position, uint32 Size, FTGOR_Normal& Out)
	{
		return CTGOR_Pack<float>::DataRead(Context, Database, Position, Size, Out.Value);
	}
};

template<>
class CTGOR_Arch<FTGOR_Normal>
{
public:
	static void DataWrite(UTGOR_Context* Context, CTGOR_Archive* Archive, const FTGOR_Normal& In)
	{
		const uint8 Normal = In.Pack();
		CTGOR_Arch<uint8>::DataWrite(Context, Archive, Normal);
	}
	static bool DataRead(UTGOR_Context* Context, const CTGOR_Archive* Archive, FTGOR_Normal& Out)
	{
		uint8 Normal;
		CTGOR_Arch<uint8>::DataRead(Context, Archive, Normal);
		Out.Unpack(Normal);
		return true;
	}
};

// Percentage (high precision float in [0,1])
template<>
class CTGOR_Pack<FTGOR_Percentage>
{
public:
	static FString DataType() { return "Percentage"; }
	static void DataWrite(UTGOR_Context* Context, CTGOR_Database* Database, uint32 Position, uint32& Size, const FTGOR_Percentage& In)
	{
		CTGOR_Pack<float>::DataWrite(Context, Database, Position, Size, In.Value);
	}
	static bool DataRead(UTGOR_Context* Context, const CTGOR_Database* Database, uint32 Position, uint32 Size, FTGOR_Percentage& Out)
	{
		return CTGOR_Pack<float>::DataRead(Context, Database, Position, Size, Out.Value);
	}
};

template<>
class CTGOR_Arch<FTGOR_Percentage>
{
public:
	static void DataWrite(UTGOR_Context* Context, CTGOR_Archive* Archive, const FTGOR_Percentage& In)
	{
		const uint16 Normal = In.Pack();
		CTGOR_Arch<uint16>::DataWrite(Context, Archive, Normal);
	}
	static bool DataRead(UTGOR_Context* Context, const CTGOR_Archive* Archive, FTGOR_Percentage& Out)
	{
		uint16 Normal;
		CTGOR_Arch<uint16>::DataRead(Context, Archive, Normal);
		Out.Unpack(Normal);
		return true;
	}
};

// Polar coordinates
template<>
class CTGOR_Pack<FTGOR_Polar>
{
public:
	static FString DataType() { return "Polar"; }
	static void DataWrite(UTGOR_Context* Context, CTGOR_Database* Database, uint32 Position, uint32& Size, const FTGOR_Polar& In)
	{
		const FVector Polar = FVector(In.X, In.Y, In.R);
		CTGOR_Pack<FVector>::DataWrite(Context, Database, Position, Size, Polar);
	}
	static bool DataRead(UTGOR_Context* Context, const CTGOR_Database* Database, uint32 Position, uint32 Size, FTGOR_Polar& Out)
	{
		FVector Polar;
		CTGOR_Pack<FVector>::DataRead(Context, Database, Position, Size, Polar);
		Out.X = Polar.X;
		Out.Y = Polar.Y;
		Out.R = Polar.Z;
		return true;
	}
};

template<>
class CTGOR_Arch<FTGOR_Polar>
{
public:
	static void DataWrite(UTGOR_Context* Context, CTGOR_Archive* Archive, const FTGOR_Polar& In)
	{
		const uint16 Polar = In.Pack();
		CTGOR_Arch<uint16>::DataWrite(Context, Archive, Polar);
	}
	static bool DataRead(UTGOR_Context* Context, const CTGOR_Archive* Archive, FTGOR_Polar& Out)
	{
		uint16 Polar;
		CTGOR_Arch<uint16>::DataRead(Context, Archive, Polar);
		Out.Unpack(Polar);
		return true;
	}
};

// Serial
template<>
class CTGOR_Pack<FTGOR_Serial>
{
public:
	static FString DataType() { return "Serial"; }
	static void DataWrite(UTGOR_Context* Context, CTGOR_Database* Database, uint32 Position, uint32& Size, const FTGOR_Serial& In)
	{
		CTGOR_Pack<int32>::DataWrite(Context, Database, Position, Size, In.Index);
	}
	static bool DataRead(UTGOR_Context* Context, const CTGOR_Database* Database, uint32 Position, uint32 Size, FTGOR_Serial& Out)
	{
		return CTGOR_Pack<int32>::DataRead(Context, Database, Position, Size, Out.Index);
	}
};

template<>
class CTGOR_Arch<FTGOR_Serial>
{
public:
	static void DataWrite(UTGOR_Context* Context, CTGOR_Archive* Archive, const FTGOR_Serial& In)
	{
		uint16 Serial = (uint16)In.Index;
		CTGOR_Arch<uint16>::DataWrite(Context, Archive, Serial);
	}
	static bool DataRead(UTGOR_Context* Context, const CTGOR_Archive* Archive, FTGOR_Serial& Out)
	{
		uint16 Serial = 0;
		CTGOR_Arch<uint16>::DataRead(Context, Archive, Serial);
		Out.Index = (int32)Serial;
		return true;
	}
};

// Index
template<>
class CTGOR_Pack<FTGOR_Index>
{
public:
	static FString DataType() { return "Index"; }
	static void DataWrite(UTGOR_Context* Context, CTGOR_Database* Database, uint32 Position, uint32& Size, const FTGOR_Index& In)
	{
		CTGOR_Pack<int32>::DataWrite(Context, Database, Position, Size, In.Index);
	}
	static bool DataRead(UTGOR_Context* Context, const CTGOR_Database* Database, uint32 Position, uint32 Size, FTGOR_Index& Out)
	{
		return CTGOR_Pack<int32>::DataRead(Context, Database, Position, Size, Out.Index);
	}
};

template<>
class CTGOR_Arch<FTGOR_Index>
{
public:
	static void DataWrite(UTGOR_Context* Context, CTGOR_Archive* Archive, const FTGOR_Index& In)
	{
		const bool IsValid = (In.Index > INDEX_NONE);
		CTGOR_Arch<bool>::DataWrite(Context, Archive, IsValid);

		if (IsValid)
		{
			CTGOR_Arch<FTGOR_Serial>::DataWrite(Context, Archive, In);
		}
	}
	static bool DataRead(UTGOR_Context* Context, const CTGOR_Archive* Archive, FTGOR_Index& Out)
	{
		bool IsValid;
		CTGOR_Arch<bool>::DataRead(Context, Archive, IsValid);

		if (IsValid)
		{
			return CTGOR_Arch<FTGOR_Serial>::DataRead(Context, Archive, Out);
		}
		else
		{
			Out.Index = INDEX_NONE;
		}
		return true;
	}
};



// Table
template<>
class CTGOR_Pack<FTGOR_Table>
{
public:
	static FString DataType() { return "Table"; }
	static void DataWrite(UTGOR_Context* Context, CTGOR_Database* Database, uint32 Position, uint32& Size, const FTGOR_Table& In)
	{
		CTGOR_Pack<TArray<int32>>::DataWrite(Context, Database, Position, Size, In.Values);
	}
	static bool DataRead(UTGOR_Context* Context, const CTGOR_Database* Database, uint32 Position, uint32 Size, FTGOR_Table& Out)
	{
		return CTGOR_Pack<TArray<int32>>::DataRead(Context, Database, Position, Size, Out.Values);
	}
};

template<>
class CTGOR_Arch<FTGOR_Table>
{
public:
	static void DataWrite(UTGOR_Context* Context, CTGOR_Archive* Archive, const FTGOR_Table& In)
	{
		const int32 Num = In.Capacities.Num();
		for (int32 Index = 0; Index < Num; Index++)
		{
			// Send mask bit if requested
			if (In.Default == INDEX_NONE)
			{
				CTGOR_Arch<bool>::DataWrite(Context, Archive, In.Mask[Index]);
			}
			
			if(In.Default > INDEX_NONE || In.Mask[Index])
			{
				const int32 Bits = UTGOR_Math::Log2(In.Capacities[Index]);
				Archive->WriteBits((const uint8*)&In.Values[Index], Bits);
			}
		}
	}
	static bool DataRead(UTGOR_Context* Context, const CTGOR_Archive* Archive, FTGOR_Table& Out)
	{
		const int32 Num = Out.Capacities.Num();
		for (int32 Index = 0; Index < Num; Index++)
		{
			// Receive mask bit if requested
			if (Out.Default == INDEX_NONE)
			{
				CTGOR_Arch<bool>::DataRead(Context, Archive, Out.Mask[Index]);
			}
			else
			{
				Out.Mask[Index] = true;
			}

			if (Out.Mask[Index])
			{
				const int32 Bits = UTGOR_Math::Log2(Out.Capacities[Index]);
				Archive->ReadBits((uint8*)&Out.Values[Index], Bits);
			}
		}
		return true;
	}
};
