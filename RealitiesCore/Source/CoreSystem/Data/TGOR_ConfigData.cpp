// The Gateway of Realities: Planes of Existence.


#include "TGOR_ConfigData.h"
#include "CoreSystem/Storage/TGOR_Package.h"

#include "CoreSystem/Utility/TGOR_Encryption.h"
#include "CoreSystem/TGOR_Singleton.h"


FTGOR_UserMapData::FTGOR_UserMapData()
	: UserKey(-1)
{
}

void FTGOR_UserMapData::Write(FTGOR_GroupWritePackage& Package, UTGOR_Singleton* Context) const
{
	Package.WriteEntry("UserKey", UserKey);
	Package.WriteEntry("PrivateKey", PrivateKey);
}

void FTGOR_UserMapData::Read(FTGOR_GroupReadPackage& Package, UTGOR_Singleton* Context)
{
	Package.ReadEntry("UserKey", UserKey);
	Package.ReadEntry("PrivateKey", PrivateKey);
}


UTGOR_ConfigData::UTGOR_ConfigData()
{
	UserData.UserKey = -1;
	UserData.PrivateKey = UTGOR_Encryption::CreatePrivateKey();
}

void UTGOR_ConfigData::Write_Implementation(FTGOR_GroupWritePackage& Package, UTGOR_Singleton* Context) const
{
	Package.WriteEntry("UserData", UserData);
}

bool UTGOR_ConfigData::Read_Implementation(FTGOR_GroupReadPackage& Package, UTGOR_Singleton* Context)
{
	return Package.ReadEntry("UserData", UserData);
}

FTGOR_UserMapData& UTGOR_ConfigData::GetUserData()
{
	return UserData;
}

