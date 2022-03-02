// The Gateway of Realities: Planes of Existence.
#pragma once

#include "CoreMinimal.h"
#include "Dom/JsonValue.h"

#include "../TGOR_BodypartInstance.h"


struct CUSTOMISATIONSYSTEM_API FShortVector4
{
	FShortVector4(uint16 V = 0)
	: X(V), Y(V), Z(V), W(V)
	{}
	uint16 X, Y, Z, W;
};

struct CUSTOMISATIONSYSTEM_API FByteVector4
{
	FByteVector4(uint8 V = 0)
		: X(V), Y(V), Z(V), W(V)
	{}
	uint8 X, Y, Z, W;
};


struct CUSTOMISATIONSYSTEM_API FTGOR_ExportBone
{
	FString Name = "Bone";
	TArray<int32> Children;
	FVector3f Location = FVector3f::ZeroVector;
	FQuat4f Rotation = FQuat4f::Identity;
	FVector3f Scale = FVector3f::OneVector;
};

struct CUSTOMISATIONSYSTEM_API FTGOR_ExportVertex
{
	FVector3f Position = FVector3f::ZeroVector;
	FVector3f Normal = FVector3f::ZeroVector;
	FVector4f Tangent = FVector4f(ForceInit);
	FVector4f Color = FVector4f(ForceInit);
	FVector2f UV = FVector2f::ZeroVector;
	FShortVector4 Joints0 = FShortVector4();
	FShortVector4 Joints1 = FShortVector4();
	FByteVector4 Weights0 = FByteVector4();
	FByteVector4 Weights1 = FByteVector4();
};

struct CUSTOMISATIONSYSTEM_API FTGOR_ExportMorph
{
	FVector3f Position = FVector::ZeroVector;
	FVector3f Normal = FVector::ZeroVector;
};

struct CUSTOMISATIONSYSTEM_API FTGOR_ExportSection
{
	FString Name = "Section";

	FTGOR_ExportVertex MinVertex;
	FTGOR_ExportVertex MaxVertex;

	// Mesh properties
	int32 BaseIndex;
	int32 IndexCount;
	int32 BaseVertex;
	int32 VertexCount;

	int32 EmissiveTexture;
	int32 OcclusionTexture;
	int32 NormalTexture;

	// Export properties
	int32 IndexBufferView;
	int32 VertexBufferView;

	int32 IndexAccessor;
	int32 PositionAccessor;
	int32 NormalAccessor;
	int32 TangentAccessor;
	int32 UVAccessor;
	int32 ColorAccessor;
	int32 Joints0Accessor;
	int32 Joints1Accessor;
	int32 Weights0Accessor;
	int32 Weights1Accessor;
};

struct CUSTOMISATIONSYSTEM_API FTGOR_ExportTarget
{
	// Mesh properties
	int32 BufferOffset = 0;
	FString Name = "Morph";
	float Default = 0.0f;

	FTGOR_ExportMorph MinMorph;
	FTGOR_ExportMorph MaxMorph;

	// Export properties
	int32 BufferView;

	int32 PositionAccessor;
	int32 NormalAccessor;
};

struct CUSTOMISATIONSYSTEM_API FTGOR_ExportTexture
{
	// Mesh properties
	FString Name = "Texture";
	UTextureRenderTarget2D* Texture;

	// Export properties
	bool IsValid = false;
	int32 Buffer = 0;
	int32 BufferOffset = 0;
	int32 BufferView = 0;
	int32 Image = 0;
};

/**
* 
*/
class CUSTOMISATIONSYSTEM_API FTGOR_SkeletalMeshExport
{
public:

	FTGOR_SkeletalMeshExport(const FTGOR_BodypartMergeOutput& MergeOutput);

	// Export info

	int32 TextureCount;
	TArray<FTGOR_ExportTexture> ExportTextures;

	int32 VertexCount;
	TArray<uint32> IndexBuffer;
	FTGOR_ExportVertex MinVertex;
	FTGOR_ExportVertex MaxVertex;
	TArray<FTGOR_ExportVertex> ExportVertices;

	int32 BoneCount;
	FMatrix44f MinBind;
	FMatrix44f MaxBind;
	TArray<FMatrix44f> BindMatrices;
	TArray<FTransform3f> GlobalTransform;
	TArray<FTGOR_ExportBone> ExportBones;

	int32 MorphCount;
	TArray<FTGOR_ExportTarget> ExportMorphTargets;
	TArray<TArray<FTGOR_ExportMorph>> MorphBuffers;

	int32 SectionCount;
	TArray<FTGOR_ExportSection> ExportSections;

	////////////////////////////////////////////////////////////////////////////////////////////////////

	// Buffer Info

	TArray<uint8> MeshBuffer;
	int32 BindBufferOffset;
	int32 IndexBufferOffset;
	int32 VertexBufferOffset;
	TArray<TArray<uint8>> TextureBuffers;

	void GenerateBuffers();

	////////////////////////////////////////////////////////////////////////////////////////////////////

	int32 MeshBufferIndex;
	TArray<TSharedPtr<FJsonValue>> RegisterBuffers();

	int32 InverseBindViewIndex;
	TArray<TSharedPtr<FJsonValue>> RegisterBufferViews();

	int32 InverseBindAccessorIndex;
	TArray<TSharedPtr<FJsonValue>> RegisterAccessors();
	TArray<TSharedPtr<FJsonValue>> RegisterImages();

	int32 SamplerIndex;
	TArray<TSharedPtr<FJsonValue>> RegisterSamplers();
	TArray<TSharedPtr<FJsonValue>> RegisterTextures();
	TArray<TSharedPtr<FJsonValue>> RegisterMaterials();

	int32 SkinIndex;
	TArray<TSharedPtr<FJsonValue>> RegisterSkins();

	int32 MeshIndex;
	TArray<TSharedPtr<FJsonValue>> RegisterMeshes();

	int32 SkeletonIndex;
	TArray<TSharedPtr<FJsonValue>> RegisterNodes();
	TArray<TSharedPtr<FJsonValue>> RegisterScenes();

	////////////////////////////////////////////////////////////////////////////////////////////////////

#if WITH_EDITOR
	bool OpenFileDialogue(const FString& FileName, FString& OutFilePath);
	void Export(const FString& Filename);
#endif
};
