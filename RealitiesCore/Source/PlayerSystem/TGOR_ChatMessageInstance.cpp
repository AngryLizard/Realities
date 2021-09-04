// The Gateway of Realities: Planes of Existence.


#include "TGOR_ChatMessageInstance.h"

#include "RealitiesUGC/Mod/TGOR_ContentManager.h"
#include "CoreSystem/Gameplay/TGOR_GameInstance.h"
#include "CoreSystem/TGOR_Singleton.h"

#include "Content/TGOR_Channel.h"
#include "Content/TGOR_Command.h"



FTGOR_ChatMessageInstance::FTGOR_ChatMessageInstance()
:	Domain(ETGOR_ChatDomainEnumeration::Public),
	Command(nullptr),
	Channel(nullptr),
	UserSerial(-1),
	Sender(""),
	Message("")
{
}

/*
bool FTGOR_ChatMessageInstance::Write(UTGOR_Singleton* Singleton, FilePack& Pack) const
{
	SET_FILE_NUMBER(0, BYTE, Domain, b, uint8, DIRECT)

	SET_FILE_SOURCE(1, Command, Command, DIRECT)
	SET_FILE_SOURCE(2, Channel, Channel, DIRECT)
	SET_FILE_NUMBER(3, SIGNED, UserSerial, i, int32, DIRECT)

	SET_FILE_STRING(4, Sender, DIRECT)
	SET_FILE_STRING(5, Message, DIRECT)
	return(true);
}

bool FTGOR_ChatMessageInstance::Read(UTGOR_Singleton* Singleton, const FilePack& Pack)
{
	GET_FILE_NUMBER(0, BYTE, Domain, b, ETGOR_ChatDomainEnumeration, DIRECT)

	GET_FILE_SOURCE(1, Command, Command, DIRECT)
	GET_FILE_SOURCE(2, Channel, Channel, DIRECT)
	GET_FILE_NUMBER(3, SIGNED, UserSerial, i, int32, DIRECT)

	GET_FILE_STRING(4, Sender, DIRECT)
	GET_FILE_STRING(5, Message, DIRECT)
	return(true);
}
*/

bool FTGOR_ChatMessageInstance::NetSerialize(FArchive& Ar, class UPackageMap* Map, bool& bOutSuccess)
{
	bOutSuccess = true;
	Ar << Domain;

	uint8 Mask;
	SERIALISE_INIT_SOURCE
	switch (Domain)
	{
		case ETGOR_ChatDomainEnumeration::System:
		{
			SERIALISE_MASK_SOURCE(Command, Command)
		}
		case ETGOR_ChatDomainEnumeration::Public:
		{
			SERIALISE_MASK_SOURCE(Channel, Channel)
		}
		case ETGOR_ChatDomainEnumeration::Private:
		{
			Ar << UserSerial;
		}
	}

	Ar << Sender;
	Ar << Message;

	return(bOutSuccess);
}