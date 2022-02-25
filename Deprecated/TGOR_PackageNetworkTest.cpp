// The Gateway of Realities: Planes of Existence.

#include "TGOR_PackageNetworkTest.h"

#include "Serialization/ArrayWriter.h"
#include "Serialization/ArrayReader.h"
#include "Realities/Mod/Spawner/Items/TGOR_Item.h"

#include "Realities/Base/TGOR_Singleton.h"
#include "RealitiesUGC/Mod/TGOR_ContentManager.h"
#include "Realities/Utility/Storage/TGOR_Package.h"

FTGOR_PackageNetworkTestPacket::FTGOR_PackageNetworkTestPacket()
: Index(0),
Float(0.0f),
Serial(0),
Byte(0)
{
}

FTGOR_PackageNetworkTestPacket::FTGOR_PackageNetworkTestPacket(FRandomStream& Stream)
{
	Index = Stream.RandRange(0, 1024);
	Float = Stream.FRandRange(0, 1024.0f);
	Serial = (int16)Stream.RandRange(0, 1024);
	Byte = (uint8)Stream.RandRange(0, 255);
}

void FTGOR_PackageNetworkTestPacket::Send(FTGOR_NetworkWritePackage& Package, UTGOR_Singleton* Context) const
{
	Package.WriteEntry(Index);
	Package.WriteEntry(Float);
	Package.WriteEntry(Serial);
	Package.WriteEntry(Byte);
}

void FTGOR_PackageNetworkTestPacket::Recv(FTGOR_NetworkReadPackage& Package, UTGOR_Singleton* Context)
{
	Package.ReadEntry(Index);
	Package.ReadEntry(Float);
	Package.ReadEntry(Serial);
	Package.ReadEntry(Byte);
}

void FTGOR_PackageNetworkTestPacket::Assert(const FTGOR_PackageNetworkTestPacket& Other, AFunctionalTest* Test) const
{
	Test->AssertEqual_Int(Index, Other.Index, "Index");
	Test->AssertEqual_Float(Float, Other.Float, "Float");
	Test->AssertEqual_Int((int32)Serial, (int32)Other.Serial, "Serial");
	Test->AssertEqual_Int((int32)Byte, (int32)Other.Byte, "Byte");
}

////////////////////////////////////////////////////////////////////////////////////////////////////

FTGOR_PackageNetworkTestInstance::FTGOR_PackageNetworkTestInstance()UPROPERTY()
: Vector(FVector::ZeroVector),
Vector2D(FVector2D::ZeroVector),
Rotation(FRotator::ZeroRotator),
LinearColour(FLinearColor::Black),
Colour(FColor::Black),
String(""),
Name(""),
Enum(ETGOR_PackageNetworkTestEnum::A)
{
}

FTGOR_PackageNetworkTestInstance::FTGOR_PackageNetworkTestInstance(FRandomStream& Stream, UTGOR_Singleton* Context)
{
	UTGOR_ContentManager* ContentManager = Context->GetContentManager();

	Vector = Stream.GetUnitVector();
	Vector2D = FVector2D(Stream.GetUnitVector());
	const FVector Value = Stream.GetUnitVector();
	Rotation = FRotator(Value.X, Value.Y, Value.Z);
	LinearColour = FLinearColor(Stream.GetUnitVector());
	Colour = FLinearColor(Stream.GetUnitVector()).ToFColor(true);
	String = FString::FromInt(Stream.GetUnsignedInt());
	Name = FName(*FString::FromInt(Stream.GetUnsignedInt()));
	Enum = (ETGOR_PackageNetworkTestEnum)Stream.RandRange(0, 2);
	ContentArray = ContentManager->GetTListFromType<UTGOR_Item>();
	for (int32 Count = 0; Count < Stream.RandRange(20, 40); Count++)
	{
		FString Key = FString::FromInt(Stream.GetUnsignedInt());
		PacketMap.Add(Key, FTGOR_PackageNetworkTestPacket(Stream));
	}
	Buffer.Data.SetNum(64);
	for (uint8& Byte : Buffer.Data)
	{
		Byte = (uint8)Stream.RandRange(0, 255);
	}
}

void FTGOR_PackageNetworkTestInstance::Send(FTGOR_NetworkWritePackage& Package, UTGOR_Singleton* Context) const
{
	Package.WriteEntry(Vector);
	Package.WriteEntry(Vector2D);
	Package.WriteEntry(Rotation);
	Package.WriteEntry(LinearColour);
	Package.WriteEntry(Colour);
	Package.WriteEntry(String);
	Package.WriteEntry(Name);
	Package.WriteEntry(Enum);
	Package.WriteEntry(ContentArray);
	Package.WriteEntry(PacketMap);
	Package.WriteEntry(Buffer);
}

void FTGOR_PackageNetworkTestInstance::Recv(FTGOR_NetworkReadPackage& Package, UTGOR_Singleton* Context)
{
	Package.ReadEntry(Vector);
	Package.ReadEntry(Vector2D);
	Package.ReadEntry(Rotation);
	Package.ReadEntry(LinearColour);
	Package.ReadEntry(Colour);
	Package.ReadEntry(String);
	Package.ReadEntry(Name);
	Package.ReadEntry(Enum);
	Package.ReadEntry(ContentArray);
	Package.ReadEntry(PacketMap);
	Package.ReadEntry(Buffer);
}

void FTGOR_PackageNetworkTestInstance::Assert(const FTGOR_PackageNetworkTestInstance& Other, AFunctionalTest* Test) const
{
	Test->AssertEqual_Vector(Vector, Other.Vector, "Vector", SMALL_NUMBER);
	Test->AssertEqual_Vector(FVector(Vector2D, 0.0f), FVector(Other.Vector2D, 0.0f), "Vector2D", SMALL_NUMBER);
	Test->AssertEqual_Rotator(Rotation, Other.Rotation, "Rotation", SMALL_NUMBER);
	Test->AssertTrue(LinearColour.Equals(Other.LinearColour, SMALL_NUMBER), "LinearColour");
	Test->AssertTrue(Colour == Other.Colour, "Colour");
	Test->AssertEqual_String(String, Other.String, "String");
	Test->AssertEqual_String(Name.ToString(), Other.Name.ToString(), "Name");
	Test->AssertEqual_Int((int32)Enum, (int32)Other.Enum, "Enum");

	for (int32 Index = 0; Index < ContentArray.Num(); Index++)
	{
		Test->AssertTrue(ContentArray[Index] == Other.ContentArray[Index], FString::Printf(TEXT("ContentArray_%d"), Index));
	}

	for (const auto& Pair : PacketMap)
	{
		const FTGOR_PackageNetworkTestPacket* Ptr = Other.PacketMap.Find(Pair.Key);
		if (Ptr)
		{
			Test->LogStep(ELogVerbosity::Log, FString::Printf(TEXT("Asserting for key (%s)"), *Pair.Key));
			Pair.Value.Assert(*Ptr, Test);
		}
		else
		{
			Test->AddError(FString::Printf(TEXT("Packet (%s) not present"), *Pair.Key));
		}
	}

	for (int32 Index = 0; Index < Buffer.Data.Num(); Index++)
	{
		Test->AssertEqual_Int((int32)Buffer.Data[Index], (int32)Other.Buffer.Data[Index], FString::Printf(TEXT("Buffer_%d"), Index));
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////

UTGOR_PackageNetworkTestObject::UTGOR_PackageNetworkTestObject()
	: Super()
{
}

void UTGOR_PackageNetworkTestObject::Initialise(int32 Seed, UTGOR_Singleton* Context)
{
	FRandomStream Stream(Seed);
	Instance = FTGOR_PackageNetworkTestInstance(Stream, Context);
}

void UTGOR_PackageNetworkTestObject::Assert(UTGOR_PackageNetworkTestObject* Other, AFunctionalTest* Test) const
{
	Instance.Assert(Other->Instance, Test);
}

void UTGOR_PackageNetworkTestObject::Send_Implementation(FTGOR_NetworkWritePackage& Package, UTGOR_Singleton* Context) const
{
	Package.WriteEntry(Instance);
}

bool UTGOR_PackageNetworkTestObject::Recv_Implementation(FTGOR_NetworkReadPackage& Package, UTGOR_Singleton* Context)
{
	return Package.ReadEntry(Instance);
}

////////////////////////////////////////////////////////////////////////////////////////////////////


ATGOR_PackageNetworkTest::ATGOR_PackageNetworkTest()
	: Super()
{
}


void ATGOR_PackageNetworkTest::StartTest()
{
	// Create archive to write to
	FArrayWriter Writer;
	CTGOR_Archive WriteArchive(Writer, nullptr);

	// Create default values
	UTGOR_PackageNetworkTestObject* Object = NewObject<UTGOR_PackageNetworkTestObject>(this);
	Object->Initialise(62, Singleton);

	// Create reference values
	UTGOR_PackageNetworkTestObject* Reference = NewObject<UTGOR_PackageNetworkTestObject>(this);
	Reference->Instance = Object->Instance;

	// Write package
	WriteArchive.Serialise(Singleton, Object);
	const int32 Num = Writer.Num();

	// Create archive to read from
	FArrayReader Reader;
	Reader.SetNum(Num);
	memcpy(Reader.GetData(), Writer.GetData(), Num * sizeof(uint8));
	CTGOR_Archive ReadArchive(Reader, nullptr);

	// Read archive
	ReadArchive.Serialise(Singleton, Object);

	// Assert differences
	Object->Assert(Reference, this);
}

bool ATGOR_PackageNetworkTest::IsReady_Implementation()
{
	EnsureSingleton(this);
	return IsValid(Singleton);
}

