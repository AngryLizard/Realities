// The Gateway of Realities: Planes of Existence.

#include "TGOR_Package.h"
#include "GenericPlatform/GenericPlatformFile.h"

template<> static FString CTGOR_Pack<bool>::DataType() { return "Boolean"; }
template<> static FString CTGOR_Pack<float>::DataType() { return "Float"; }
template<> static FString CTGOR_Pack<uint8>::DataType() { return "Byte"; }
template<> static FString CTGOR_Pack<int8>::DataType() { return "Char"; }
template<> static FString CTGOR_Pack<int16>::DataType() { return "Word"; }
template<> static FString CTGOR_Pack<uint16>::DataType() { return "Size"; }
template<> static FString CTGOR_Pack<int32>::DataType() { return "Integer"; }
template<> static FString CTGOR_Pack<uint32>::DataType() { return "Index"; }
//template<> static FString CTGOR_Pack<FVector>::DataType() { return "Vector"; }
template<> static FString CTGOR_Pack<FVector2D>::DataType() { return "Vector2D"; }
//template<> static FString CTGOR_Pack<FRotator>::DataType() { return "Rotation"; }
template<> static FString CTGOR_Pack<FLinearColor>::DataType() { return "Colour"; }
template<> static FString CTGOR_Pack<FColor>::DataType() { return "BGRA"; }

