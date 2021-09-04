// The Gateway of Realities: Planes of Existence.

#include "TGOR_Package.h"

template<> FString CTGOR_Pack<bool>::DataType() { return "Boolean"; }
template<> FString CTGOR_Pack<float>::DataType() { return "Float"; }
template<> FString CTGOR_Pack<uint8>::DataType() { return "Byte"; }
template<> FString CTGOR_Pack<int8>::DataType() { return "Char"; }
template<> FString CTGOR_Pack<int16>::DataType() { return "Word"; }
template<> FString CTGOR_Pack<uint16>::DataType() { return "Size"; }
template<> FString CTGOR_Pack<int32>::DataType() { return "Integer"; }
template<> FString CTGOR_Pack<uint32>::DataType() { return "Index"; }
//template<> FString CTGOR_Pack<FVector>::DataType() { return "Vector"; } // Specialised directly in the header
template<> FString CTGOR_Pack<FVector2D>::DataType() { return "Vector2D"; }
//template<> FString CTGOR_Pack<FRotator>::DataType() { return "Rotation"; } // Specialised directly in the header
template<> FString CTGOR_Pack<FLinearColor>::DataType() { return "Color"; }
template<> FString CTGOR_Pack<FColor>::DataType() { return "BGRA"; }

