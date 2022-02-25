// The Gateway of Realities: Planes of Existence.


#include "TGOR_SkinCustomisation.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Realities/Utility/Storage/TGOR_Package.h"
#include "Realities/Components/Creature/TGOR_ModularSkeletalMeshComponent.h"

#include "Realities/Mod/Skins/TGOR_Skin.h"
#include "Realities/Mod/Bodyparts/TGOR_Bodypart.h"
#include "Realities/Base/Content/TGOR_ContentManager.h"
#include "Realities/Base/TGOR_Singleton.h"


UTGOR_SkinCustomisation::UTGOR_SkinCustomisation()
	: Super()
{
	PayloadSize = 2;
}

bool UTGOR_SkinCustomisation::Validate(UTGOR_ModularSkeletalMeshComponent* Master, const TArray<uint8>& Payload, const FName& Param, UTGOR_Bodypart* Bodypart)
{
	UTGOR_Skin* Content;
	FromPayload(Payload, Content);

	// Only go ahead if valid input
	if (IsValid(Content) && !Master->Execute_SupportsContent(Master, Content))
	{
		return false;
	}
	return true;
}

void UTGOR_SkinCustomisation::Apply(UTGOR_ModularSkeletalMeshComponent* Master, const TArray<uint8>& Payload, const FName& Param, UTGOR_Bodypart* Bodypart, USkeletalMeshComponent* MeshComponent, UMaterialInstanceDynamic* Instance)
{
	UTGOR_Skin* Content;
	FromPayload(Payload, Content);
	if (IsValid(Content) && IsValid(Bodypart))
	{
		// Make sure skin is allowed by bodypart
		if (Bodypart->ContainsI<UTGOR_Skin>(Content))
		{
			if (Master->Execute_SupportsContent(Master, Content))
			{
				// Set all textures as instructed by skin resourceable
				for (const auto& Pair : Content->Textures)
				{
					Instance->SetTextureParameterValue(Pair.Key, Pair.Value);
				}
			}
			else
			{
				ERROR(Content->GetDefaultName() + " Skin not unlocked for " + Master->GetName(), Error);
			}
		}
		else
		{
			ERROR(Content->GetDefaultName() + " not allowed in " + Bodypart->GetDefaultName(), Error)
		}
	}
	else
	{
		// Set all textures to default
		for (const auto& Pair : Content->Textures)
		{
			UTexture* Texture = nullptr;
			if (Instance->GetTextureParameterDefaultValue(Pair.Key, Texture) && Texture)
			{
				Instance->SetTextureParameterValue(Pair.Key, Texture);
			}
		}
	}
}

void UTGOR_SkinCustomisation::Query(UTGOR_ModularSkeletalMeshComponent* Master, TArray<uint8>& Payload, const FName& Param, UTGOR_Bodypart* Bodypart, USkeletalMeshComponent* MeshComponent, UMaterialInstanceDynamic* Instance) const
{
	ToPayload(Payload, nullptr);
}

void UTGOR_SkinCustomisation::Write(FTGOR_GroupWritePackage& Package, const TArray<uint8>& Payload)
{
	UTGOR_Skin* Content;
	FromPayload(Payload, Content);
	Package.WriteEntry("Content", Content);
}

bool UTGOR_SkinCustomisation::Read(FTGOR_GroupReadPackage& Package, TArray<uint8>& Payload) const
{
	UTGOR_Skin* Content;
	Package.ReadEntry("Content", Content);
	ToPayload(Payload, Content);
	return true;
}

void UTGOR_SkinCustomisation::ToPayload(TArray<uint8>& Payload, UTGOR_Skin* Content) const
{
	INIT_TO_PAYLOAD
	CONTENT_TO_PAYLOAD(Skin)
}

void UTGOR_SkinCustomisation::FromPayload(const TArray<uint8>& Payload, UTGOR_Skin*& Content)
{
	INIT_FROM_PAYLOAD
	CONTENT_FROM_PAYLOAD(Skin)
}