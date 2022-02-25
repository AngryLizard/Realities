// The Gateway of Realities: Planes of Existence.

#pragma once

#include "CoreMinimal.h"
#include "Engine/Canvas.h"
#include "Engine/TextureRenderTarget2D.h"
#include "Kismet/KismetRenderingLibrary.h"

#include "Realities/CoreSystem/Storage/TGOR_Serialisation.h"
#include "Realities/CoreSystem/Storage/TGOR_PackageGroup.h"
#include "Realities/CoreSystem/Storage/TGOR_PackageNetwork.h"

#include "Engine/UserDefinedStruct.h"
#include "TGOR_BodypartInstance.generated.h"

class UTGOR_ChildSkeletalMeshComponent;
class UTGOR_Customisation;
class UTGOR_Bodypart;
class UTGOR_Section;
class UTGOR_Canvas;

/**
*
*/
USTRUCT(BlueprintType)
struct REALITIES_API FTGOR_CustomisationPayload
{
	GENERATED_USTRUCT_BODY();

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TArray<uint8> Bytes;
};

/**
*
*/
USTRUCT(BlueprintType)
struct REALITIES_API FTGOR_CustomisationPayloadCollection
{
	GENERATED_USTRUCT_BODY();

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TArray<FTGOR_CustomisationPayload> List;
};

/**
* 
*/
USTRUCT(BlueprintType)
struct REALITIES_API FTGOR_CustomisationInstance
{
	GENERATED_USTRUCT_BODY();
	SERIALISE_INIT_HEADER;

	FTGOR_CustomisationInstance();

	CTGOR_GroupPackageCache LegacyCache;
	void Write(FTGOR_GroupWritePackage& Package, UTGOR_Singleton* Context) const;
	void Read(FTGOR_GroupReadPackage& Package, UTGOR_Singleton* Context);
	void Send(FTGOR_NetworkWritePackage& Package, UTGOR_Context* Context) const;
	void Recv(FTGOR_NetworkReadPackage& Package, UTGOR_Context* Context);

	////////////////////////////////////////////////////////////////////////////////////////////////////

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		UTGOR_Customisation* Content;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FTGOR_CustomisationPayload Payload;

};

template<>
struct TStructOpsTypeTraits<FTGOR_CustomisationInstance> : public TStructOpsTypeTraitsBase2<FTGOR_CustomisationInstance>
{
	enum
	{
		WithNetSerializer = true
	};
};

/**
*
*/
USTRUCT(BlueprintType)
struct FTGOR_BodypartInstance
{
	GENERATED_USTRUCT_BODY();
	SERIALISE_INIT_HEADER;

	FTGOR_BodypartInstance();

	CTGOR_GroupPackageCache LegacyCache;
	void Write(FTGOR_GroupWritePackage& Package, UTGOR_Singleton* Context) const;
	void Read(FTGOR_GroupReadPackage& Package, UTGOR_Singleton* Context);
	void Send(FTGOR_NetworkWritePackage& Package, UTGOR_Context* Context) const;
	void Recv(FTGOR_NetworkReadPackage& Package, UTGOR_Context* Context);

	////////////////////////////////////////////////////////////////////////////////////////////////////

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		UTGOR_Bodypart* Content;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		uint8 Depth;

	////////////////////////////////////////////////////////////////////////////////////////////////////

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TArray<FTGOR_CustomisationInstance> CustomisationInstances;

};

template<>
struct TStructOpsTypeTraits<FTGOR_BodypartInstance> : public TStructOpsTypeTraitsBase2<FTGOR_BodypartInstance>
{
	enum
	{
		WithNetSerializer = true
	};
};


/**
* TGOR_AppearanceInstance stores character appearance information
*/
USTRUCT(BlueprintType)
struct FTGOR_AppearanceInstance
{
	GENERATED_USTRUCT_BODY()
		SERIALISE_INIT_HEADER;

	FTGOR_AppearanceInstance();

	CTGOR_GroupPackageCache LegacyCache;
	void Write(FTGOR_GroupWritePackage& Package, UTGOR_Singleton* Context) const;
	void Read(FTGOR_GroupReadPackage& Package, UTGOR_Singleton* Context);
	void Send(FTGOR_NetworkWritePackage& Package, UTGOR_Context* Context) const;
	void Recv(FTGOR_NetworkReadPackage& Package, UTGOR_Context* Context);

	////////////////////////////////////////////////////////////////////////////////////////////////////

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "!TGOR Bodypart")
		TArray<FTGOR_BodypartInstance> Bodyparts;

};

template<>
struct TStructOpsTypeTraits<FTGOR_AppearanceInstance> : public TStructOpsTypeTraitsBase2<FTGOR_AppearanceInstance>
{
	enum
	{
		WithNetSerializer = true
	};
};


UENUM(BlueprintType)
enum class ETGOR_BodypartChildType : uint8
{
	Optional, // Optionally loaded, changed by the user
	Required, // Required to be loaded, changed by the user
	Automatic // Optionally loaded, changed by code
};


USTRUCT(BlueprintType)
struct FTGOR_BodypartChild
{
	GENERATED_USTRUCT_BODY();

	/** Bodypart class */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "!TGOR Bodypart")
		TSubclassOf<UTGOR_Bodypart> Class;

	/** Loading behaviour of this child */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "!TGOR Bodypart")
		ETGOR_BodypartChildType Type;
};

// Holds a specific window on a rendertarget
USTRUCT(BlueprintType)
struct FTGOR_TextureAllocation
{
	GENERATED_USTRUCT_BODY()

	/**  */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "!TGOR Bodypart")
		FVector2D Pos = FVector2D::ZeroVector;

	/**  */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "!TGOR Bodypart")
		FVector2D Size = FVector2D::ZeroVector;

	/**  */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "!TGOR Bodypart")
		UTextureRenderTarget2D* RenderTarget = nullptr;

	/**  */
	FCanvas* Canvas;

	/**  */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "!TGOR Bodypart")
		FDrawToRenderTargetContext Context;
};

USTRUCT(BlueprintType)
struct FTGOR_TextureScale
{
	GENERATED_USTRUCT_BODY()

	/** Power of two scale to width */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "!TGOR Bodypart")
		int32 ScaleX = 0;

	/** Power of two scale to height */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "!TGOR Bodypart")
		int32 ScaleY = 0;
};

USTRUCT(BlueprintType)
struct FTGOR_BodypartCustomisationAllocation
{
	GENERATED_USTRUCT_BODY()

	/** Texture allocation in case this uses rendertargets */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "!TGOR Bodypart")
		TMap<UTGOR_Canvas*, FTGOR_TextureAllocation> Textures;
};

USTRUCT(BlueprintType)
struct FTGOR_BodypartPlacement
{
	GENERATED_USTRUCT_BODY()

	/** */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "!TGOR Bodypart")
		int32 PosX;

	/** */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "!TGOR Bodypart")
		int32 PosY;

	/** */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "!TGOR Bodypart")
		int32 SizeX;

	/** */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "!TGOR Bodypart")
		int32 SizeY;
};

// Holds mesh and assigned textures
USTRUCT(BlueprintType)
struct FTGOR_BodypartMeshResources
{
	GENERATED_USTRUCT_BODY();

	/**  */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "!TGOR Bodypart")
		const UTGOR_ChildSkeletalMeshComponent* Mesh;

	/**  */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "!TGOR Bodypart")
		int32 SectionIndex;

	/**  */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "!TGOR Bodypart")
		int32 SizeY;

	/**  */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "!TGOR Bodypart")
		int32 SizeX;

	/**  */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "!TGOR Bodypart")
		TMap<UTGOR_Canvas*, UTexture*> Textures;
};

// Holds a list of compatible meshes
USTRUCT(BlueprintType)
struct FTGOR_BodypartSectionResources
{
	GENERATED_USTRUCT_BODY();

	/**  */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "!TGOR Bodypart")
		TArray<FTGOR_BodypartMeshResources> Meshes;
};

// Holds a specific window on a rendertarget
USTRUCT(BlueprintType)
struct FTGOR_BodypartResources
{
	GENERATED_USTRUCT_BODY();

	/** Sections to be merged */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "!TGOR Bodypart")
		TMap<UTGOR_Section*, FTGOR_BodypartSectionResources> Sections;

	/** Merged customisations */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "!TGOR Bodypart")
		TMap<UTGOR_Customisation*, FTGOR_CustomisationPayloadCollection> Customisations;
};

// Merged section output
USTRUCT(BlueprintType)
struct FTGOR_BodypartSectionOutput
{
	GENERATED_USTRUCT_BODY();

	/**  */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "!TGOR Bodypart")
		UTGOR_Section* Section;

	/**  */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "!TGOR Bodypart")
		TSet<UTGOR_Canvas*> Textures;
};

// Merged body output
USTRUCT(BlueprintType)
struct FTGOR_BodypartMergeOutput
{
	GENERATED_USTRUCT_BODY();

	/**  */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "!TGOR Bodypart")
		USkeletalMesh* Mesh;

	/**  */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "!TGOR Bodypart")
		TArray<FTGOR_BodypartSectionOutput> Sections;

	/**  */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "!TGOR Bodypart")
		TMap<UTGOR_Canvas*, UTextureRenderTarget2D*> Textures;

	/**  */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TArray<FTGOR_CustomisationInstance> Customisations;
};

////////////////////////////////////////////////////////////////////////////////////////////////////

USTRUCT(BlueprintType)
struct FTGOR_CustomisationTextureAllocation
{
	GENERATED_USTRUCT_BODY()

		UPROPERTY()
		FVector2D Pos = FVector2D::ZeroVector;

	UPROPERTY()
		FVector2D Size = FVector2D::ZeroVector;

	UPROPERTY()
		FIntPoint Allocation = FIntPoint::ZeroValue;
};

USTRUCT(BlueprintType)
struct FTGOR_CustomisationMeshSection
{
	GENERATED_USTRUCT_BODY()

		UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "!TGOR Bodypart")
		TMap<UTGOR_Canvas*, FTGOR_CustomisationTextureAllocation> Textures;
};

USTRUCT(BlueprintType)
struct FTGOR_CustomisationMeshEntry
{
	GENERATED_USTRUCT_BODY()

		UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "!TGOR Bodypart")
		int32 Parent = INDEX_NONE;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "!TGOR Bodypart")
		TArray<int32> Children;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "!TGOR Bodypart")
		TMap<UTGOR_Section*, FTGOR_CustomisationMeshSection> Sections;
};

USTRUCT(BlueprintType)
struct FTGOR_CustomisationMeshTexture
{
	GENERATED_USTRUCT_BODY()

		UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "!TGOR Bodypart")
		UTextureRenderTarget2D* RenderTarget = nullptr;

	FCanvas* Canvas;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "!TGOR Bodypart")
		FDrawToRenderTargetContext Context;
};
