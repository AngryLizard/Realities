// The Gateway of Realities: Planes of Existence.


#include "TGOR_PackageMap.h"

uint32 CTGOR_RegisterList<CTGOR_MapPackageRegister>::GetRegisterSize(const CTGOR_MapPackageRegister& Register) const { return Register.KeySize + Register.ValueSize; }
