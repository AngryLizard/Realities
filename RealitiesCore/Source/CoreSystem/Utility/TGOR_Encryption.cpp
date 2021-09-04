// The Gateway of Realities: Planes of Existence.

#include "TGOR_Encryption.h"


////////////////////////////////////////////////////////////////////////////////////////////////////

FTGOR_Buffer UTGOR_Encryption::Encrypt(const FTGOR_Buffer& Message, const FTGOR_Buffer& PublicKey)
{
	return(Message);
}

FTGOR_Buffer UTGOR_Encryption::Decrypt(const FTGOR_Buffer& Message, const FTGOR_Buffer& PrivateKey)
{
	return(Message);
}

FTGOR_Buffer UTGOR_Encryption::CreatePrivateKey()
{
	FTGOR_Buffer PrivateKey;
	PrivateKey.Data.SetNum(8);
	return(PrivateKey);
}

FTGOR_Buffer UTGOR_Encryption::CreatePublicKey(const FTGOR_Buffer& PrivateKey)
{
	FTGOR_Buffer PublicKey;
	PublicKey.Data.SetNum(8);
	return(PublicKey);
}

FTGOR_Buffer UTGOR_Encryption::CreatePassword()
{
	const int32 n = 8;

	FTGOR_Buffer Password;
	Password.Data.SetNum(n);
	for (int i = 0; i < n; i++)
	{
		Password.Data[i] = (uint8)(FMath::Rand() % 0xFF);
	}
	return(Password);
}