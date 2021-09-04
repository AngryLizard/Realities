// The Gateway of Realities: Planes of Existence.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"



////////////////////////////////////////////////////////////////////////////////////////////////////

#define SERIALISE_INIT_HEADER \
public: bool NetSerialize(FArchive & Ar, class UPackageMap* Map, bool& bOutSuccess)

#define SERIALISE_INIT_ARCHIVE \
CTGOR_Archive Archive(Ar, Map); \
return bOutSuccess = Archive.Serialise(nullptr, *this);

#define SERIALISE_INIT_HEADER \
public: bool NetSerialize(FArchive & Ar, class UPackageMap* Map, bool& bOutSuccess)

#define SERIALISE_INIT_IMPLEMENT(T) \
bool T::NetSerialize(FArchive & Ar, class UPackageMap* Map, bool& bOutSuccess) { SERIALISE_INIT_ARCHIVE; }


#define SERIALISE_INIT_SOURCE \
UTGOR_Singleton* Singleton = UTGOR_GameInstance::EnsureSingletonFromWorld(Map->GetWorld()); \
if (!IsValid(Singleton)) { ERRET("Singleton invalid", Error, bOutSuccess = false); }

#define SERIALISE_INIT_SOURCE_ARCHIVE \
SERIALISE_INIT_SOURCE; CTGOR_Archive Archive(Ar, Map); \
return bOutSuccess = Archive.Serialise(Singleton, *this);

#define SERIALISE_INIT_SOURCE_IMPLEMENT(T) \
bool T::NetSerialize(FArchive & Ar, class UPackageMap* Map, bool& bOutSuccess) { SERIALISE_INIT_SOURCE_ARCHIVE; }



/*
// Is read by header tool, so can't make this easier.
#define SERIALISE_INIT_TRAITS(T) \
template<> struct TStructOpsTypeTraits<T> : public TStructOpsTypeTraitsBase2<T> \
{ enum { WithNetworkSerializer = true }; }
*/



#define SERIALISE_MASK_SOURCE(N, T) \
{ UTGOR_ContentManager* ContentManager = Singleton->GetContentManager(); \
int16 Serial = -1; Mask = IsValid(N); if (Mask) { Serial = ContentManager->GetTIndex<UTGOR_##T>(N); } Ar.SerializeBits(&Mask, 1); if (Mask) { Ar << Serial; \
N = Cast<UTGOR_##T>(ContentManager->GetTFromIndex<UTGOR_##T>(Serial)); } }

#define SERIALISE_MASK_ATTR_COND(N, B) \
Mask = (B); Ar.SerializeBits(&Mask, 1); if (Mask) Ar << N;

#define SERIALISE_MASK_BOOL(N) \
Ar.SerializeBits(&N, 1);

#define SERIALISE_MASK_ATTR(N, Z) \
SERIALISE_MASK_ATTR_COND(N, N != Z) else N = Z;

#define SERIALISE_MASK_NORMAL(N) \
{uint8 Normal = 0; if (Ar.IsSaving()) { Normal = (uint8)(FMath::Clamp((N + 1.0f) / 2, 0.0f, 1.0f) * 0xFF); } \
SERIALISE_MASK_ATTR(Normal, 0) if (Ar.IsLoading()) { N = FMath::Clamp(((float)Normal) / 0xFF, 0.0f, 1.0f) * 2 - 1.0f; }}

#define SERIALISE_MASK_LIST(N) \
SERIALISE_MASK_ATTR_COND(N, N.Num() != 0)

#define SERIALISE_MASK_CUSTOM_LIST(N) \
Mask = (N.Num() == 0); Ar.SerializeBits(&Mask, 1); \
if (!Mask) { Mask = (uint8)N.Num(); Ar << Mask; N.SetNumZeroed(Mask); \
for(int i = 0; i < Mask; i++) { N[i].NetSerialize(Ar, Map, bOutSuccess); }}

#define SERIALISE_MASK_OBJECT_PTR(N, T) \
Mask = N.IsValid(); \
Ar.SerializeBits(&Mask, 1); \
if (Mask) { UObject* Object = N.Get(); \
bOutSuccess = Map->SerializeObject(Ar, T::StaticClass(), Object) && bOutSuccess; \
N = TWeakObjectPtr<T>(Cast<T>(Object)); } else { N = nullptr; }

#define SERIALISE_MASK_OBJECT(N, T) \
Mask = IsValid(N); \
Ar.SerializeBits(&Mask, 1); \
if (Mask) { UObject* Object = N; \
bOutSuccess = Map->SerializeObject(Ar, T::StaticClass(), Object) && bOutSuccess; \
N = Cast<T>(Object); } else { N = nullptr; }

#define SERIALISE_MASK_VECT(N) \
Mask = !N.IsNearlyZero(); \
Ar.SerializeBits(&Mask, 1); \
if (Mask) { SerializePackedVector<10, 24>(N, Ar); } \
else { N = FVector::ZeroVector; }

#define SERIALISE_MASK_NORM(N) \
Mask = !N.IsNearlyZero(); \
Ar.SerializeBits(&Mask, 1); \
if (Mask) { SerializeFixedVector<1, 16>(N, Ar); } \
else { N = FVector::ZeroVector; }