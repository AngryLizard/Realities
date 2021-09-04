// The Gateway of Realities: Planes of Existence.


#include "TGOR_MergeCustomisation.h"
#include "CoreSystem/Storage/TGOR_Package.h"
#include "Materials/MaterialInstanceDynamic.h"

#include "CustomisationSystem/Content/TGOR_Bodypart.h"
#include "CustomisationSystem/Content/TGOR_Canvas.h"
#include "CustomisationSystem/UserData/TGOR_SkeletalMergeUserData.h"
#include "CustomisationSystem/UserData/TGOR_BodypartUserData.h"
#include "CustomisationSystem/Components/TGOR_CustomisationComponent.h"

UTGOR_MergeCustomisation::UTGOR_MergeCustomisation()
	: Super()
{
	PayloadSize = 0;
}


bool UTGOR_MergeCustomisation::Check(UTGOR_CustomisationComponent* Master, const FTGOR_CustomisationPayload& Payload, UTGOR_Bodypart* Bodypart)
{
	FromPayload(Payload);

	return true;
}

void UTGOR_MergeCustomisation::Apply(UTGOR_CustomisationComponent* Master, const FTGOR_CustomisationPayload& Payload, UTGOR_Bodypart* Bodypart, int32 NodeIndex, UMaterialInstanceDynamic* Instance, const FTGOR_CustomisationMeshSection& Allocation)
{
	FromPayload(Payload);
	if (IsValid(Bodypart))
	{
		UTGOR_BodypartUserData* UserData = Bodypart->Mesh->GetAssetUserData<UTGOR_BodypartUserData>();
		const int32 ParentIndex = Master->CustomisationEntries[NodeIndex].Parent;
		if (Master->CurrentAppearance.Bodyparts.IsValidIndex(ParentIndex) && IsValid(UserData))
		{
			for (auto& TexturePair : Allocation.Textures)
			{
				const FTGOR_BodypartInstance& ParentInstance = Master->CurrentAppearance.Bodyparts[ParentIndex];
				const FTGOR_CustomisationMeshEntry& ParentEntry = Master->CustomisationEntries[ParentIndex];
				const FTGOR_CustomisationMeshSection* ParentSection = ParentEntry.Sections.Find(Instanced_SectionInsertion.Collection);
				UTGOR_BodypartUserData* ParentUserData = ParentInstance.Content->Mesh->GetAssetUserData<UTGOR_BodypartUserData>();
				if (IsValid(ParentUserData) && ParentSection)
				{
					const FTGOR_CustomisationTextureAllocation* ParentTexture = ParentSection->Textures.Find(TexturePair.Key);
					if (ParentTexture)
					{
						// TODO: Cannot write to a RT we're reading from. Find some swap-chain idea or whatever to make this happen.
						/*
						UTGOR_SkeletalMergeUserData* MergeUserData = Bodypart->Mesh->GetAssetUserData<UTGOR_SkeletalMergeUserData>();
						const FTGOR_CustomisationMeshTexture& Cache = Master->CustomisationTextures[TexturePair.Key];
						if (IsValid(MergeUserData) && IsValid(Cache.RenderTarget))
						{
							const FTransform PointTransform = FTransform(FQuat::Identity, FVector(TexturePair.Value.Pos, 0.0f), FVector(TexturePair.Value.Size, 1.0f));
							TexturePair.Key->RenderBlend(MergeUserData, ParentUserData, Bodypart, Instanced_SectionInsertion.Collection, Cache.RenderTarget, PointTransform, Cache.Canvas);
						}
						else
						{
							TexturePair.Key->RenderBlendTexture(UserData, ParentUserData, Bodypart, Instanced_SectionInsertion.Collection, Cache.RenderTarget, Cache.Canvas, TexturePair.Value.Pos, TexturePair.Value.Size);
						}
						*/
					}
				}
			}
		}
	}
}

void UTGOR_MergeCustomisation::Query(const UTGOR_CustomisationComponent* Master, FTGOR_CustomisationPayload& Payload, UTGOR_Bodypart* Bodypart, int32 NodeIndex, UMaterialInstanceDynamic* Instance, const FTGOR_CustomisationMeshSection& Allocation) const
{
	ToPayload(Payload);
}

void UTGOR_MergeCustomisation::Write(FTGOR_GroupWritePackage& Package, const FTGOR_CustomisationPayload& Payload)
{
}

bool UTGOR_MergeCustomisation::Read(FTGOR_GroupReadPackage& Package, FTGOR_CustomisationPayload& Payload) const
{
	return true;
}


void UTGOR_MergeCustomisation::ToPayload(FTGOR_CustomisationPayload& Payload) const
{
	INIT_TO_PAYLOAD;
}

void UTGOR_MergeCustomisation::FromPayload(const FTGOR_CustomisationPayload& Payload)
{
	INIT_FROM_PAYLOAD;
}