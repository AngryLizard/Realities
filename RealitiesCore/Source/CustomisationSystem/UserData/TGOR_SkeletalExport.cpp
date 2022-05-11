#include "TGOR_SkeletalExport.h"
#include "Dom/JsonObject.h"
#include "Serialization/JsonWriter.h"
#include "Serialization/JsonSerializer.h"
#include "Misc/FileHelper.h"
#include "Misc/Base64.h"

#include "CustomisationSystem/Components/TGOR_CustomisationComponent.h"
#include "CustomisationSystem/Content/TGOR_Section.h"
#include "CustomisationSystem/Content/TGOR_Canvas.h"
#include "Rendering/Texture2DResource.h"
#include "IImageWrapperModule.h"
#include "IImageWrapper.h"

#include "Rendering/SkeletalMeshRenderData.h"
#include "Rendering/SkeletalMeshLODRenderData.h"
#include "Animation/MorphTarget.h"

#if WITH_EDITOR
#include "DesktopPlatformModule.h"
#include "IDesktopPlatform.h"
#endif

#define LOCTEXT_NAMESPACE "TGOR_SkeletalExport"

template<typename T>
T Min4(const T& A, const T& B)
{
	T Out;
	Out.X = FMath::Min(A.X, B.X);
	Out.Y = FMath::Min(A.Y, B.Y);
	Out.Z = FMath::Min(A.Z, B.Z);
	Out.W = FMath::Min(A.W, B.W);
	return Out;
}

template<typename T>
T Max4(const T& A, const T& B)
{
	T Out;
	Out.X = FMath::Max(A.X, B.X);
	Out.Y = FMath::Max(A.Y, B.Y);
	Out.Z = FMath::Max(A.Z, B.Z);
	Out.W = FMath::Max(A.W, B.W);
	return Out;
}

// This function in FReferenceSkeleton for some reason isn't DLL'd ):
int32 GetDirectChildBones(const FReferenceSkeleton& ReferenceSkeleton, int32 ParentBoneIndex, TArray<int32>& Children)
{
	Children.Reset();

	const int32 NumBones = ReferenceSkeleton.GetNum();
	for (int32 ChildIndex = ParentBoneIndex + 1; ChildIndex < NumBones; ChildIndex++)
	{
		if (ParentBoneIndex == ReferenceSkeleton.GetParentIndex(ChildIndex))
		{
			Children.Add(ChildIndex);
		}
	}

	return Children.Num();
}

FTGOR_SkeletalMeshExport::FTGOR_SkeletalMeshExport(const FTGOR_BodypartMergeOutput& MergeOutput)
{
	FSkeletalMeshRenderData* RenderData = MergeOutput.Mesh->GetResourceForRendering();
	const FSkeletalMeshLODRenderData& LODRenderData = RenderData->LODRenderData[0];

	// Create texture data
	TextureCount = MergeOutput.Textures.Num();
	ExportTextures.Empty();

	TMap<UTGOR_Canvas*, int32> CanvasMapping;
	for (const auto& Pair : MergeOutput.Textures)
	{
		const int32 TextureIndex = ExportTextures.Emplace(FTGOR_ExportTexture());
		FTGOR_ExportTexture& Texture = ExportTextures.Last();

		Texture.Texture = Pair.Value;
		Texture.Name = Pair.Key->GetDefaultName();

		CanvasMapping.Emplace(Pair.Key, TextureIndex);
	}

	// Create index data (All faces are flipped for some reason)
	LODRenderData.MultiSizeIndexContainer.GetIndexBuffer(IndexBuffer);
	const int32 FaceCount = LODRenderData.GetTotalFaces();
	for (int32 FaceIndex = 0; FaceIndex < FaceCount; FaceIndex++)
	{
		const uint32 A = IndexBuffer[FaceIndex * 3 + 1];
		const uint32 B = IndexBuffer[FaceIndex * 3 + 2];
		IndexBuffer[FaceIndex * 3 + 1] = B;
		IndexBuffer[FaceIndex * 3 + 2] = A;
	}


	// Create vertex data
	VertexCount = LODRenderData.GetNumVertices();
	ExportVertices.SetNumUninitialized(VertexCount);

	for (int32 VertexIndex = 0; VertexIndex < VertexCount; VertexIndex++)
	{
		FTGOR_ExportVertex& Vertex = ExportVertices[VertexIndex];
		Vertex.Position = LODRenderData.StaticVertexBuffers.PositionVertexBuffer.VertexPosition(VertexIndex);
		Vertex.Normal = LODRenderData.StaticVertexBuffers.StaticMeshVertexBuffer.VertexTangentZ(VertexIndex);
		Vertex.Tangent = LODRenderData.StaticVertexBuffers.StaticMeshVertexBuffer.VertexTangentX(VertexIndex);
		Vertex.UV = LODRenderData.StaticVertexBuffers.StaticMeshVertexBuffer.GetVertexUV(VertexIndex, 0);
		Vertex.Color = FLinearColor(LODRenderData.StaticVertexBuffers.ColorVertexBuffer.VertexColor(VertexIndex));

		int32 SectionIndex;
		int32 SectionVertexIndex;
		LODRenderData.GetSectionFromVertexIndex(VertexIndex, SectionIndex, SectionVertexIndex);
		const FSkelMeshRenderSection& Section = LODRenderData.RenderSections[SectionIndex];

		Vertex.Joints0 = FShortVector4();
		Vertex.Joints0.X = Section.BoneMap[LODRenderData.SkinWeightVertexBuffer.GetBoneIndex(VertexIndex, 0)];
		Vertex.Joints0.Y = Section.BoneMap[LODRenderData.SkinWeightVertexBuffer.GetBoneIndex(VertexIndex, 1)];
		Vertex.Joints0.Z = Section.BoneMap[LODRenderData.SkinWeightVertexBuffer.GetBoneIndex(VertexIndex, 2)];
		Vertex.Joints0.W = Section.BoneMap[LODRenderData.SkinWeightVertexBuffer.GetBoneIndex(VertexIndex, 3)];

		Vertex.Joints1 = FShortVector4();
		Vertex.Joints1.X = Section.BoneMap[LODRenderData.SkinWeightVertexBuffer.GetBoneIndex(VertexIndex, 4)];
		Vertex.Joints1.Y = Section.BoneMap[LODRenderData.SkinWeightVertexBuffer.GetBoneIndex(VertexIndex, 5)];
		Vertex.Joints1.Z = Section.BoneMap[LODRenderData.SkinWeightVertexBuffer.GetBoneIndex(VertexIndex, 6)];
		Vertex.Joints1.W = Section.BoneMap[LODRenderData.SkinWeightVertexBuffer.GetBoneIndex(VertexIndex, 7)];

		Vertex.Weights0 = FByteVector4();
		Vertex.Weights0.X = LODRenderData.SkinWeightVertexBuffer.GetBoneWeight(VertexIndex, 0);
		Vertex.Weights0.Y = LODRenderData.SkinWeightVertexBuffer.GetBoneWeight(VertexIndex, 1);
		Vertex.Weights0.Z = LODRenderData.SkinWeightVertexBuffer.GetBoneWeight(VertexIndex, 2);
		Vertex.Weights0.W = LODRenderData.SkinWeightVertexBuffer.GetBoneWeight(VertexIndex, 3);

		Vertex.Weights1 = FByteVector4();
		Vertex.Weights1.X = LODRenderData.SkinWeightVertexBuffer.GetBoneWeight(VertexIndex, 4);
		Vertex.Weights1.Y = LODRenderData.SkinWeightVertexBuffer.GetBoneWeight(VertexIndex, 5);
		Vertex.Weights1.Z = LODRenderData.SkinWeightVertexBuffer.GetBoneWeight(VertexIndex, 6);
		Vertex.Weights1.W = LODRenderData.SkinWeightVertexBuffer.GetBoneWeight(VertexIndex, 7);
	}

	// Create bone data

	const FReferenceSkeleton& OutputSkeleton = MergeOutput.Mesh->GetRefSkeleton();
	const TArray<FMatrix44f>& OutputInvMatrices = MergeOutput.Mesh->GetRefBasesInvMatrix();

	BoneCount = OutputSkeleton.GetNum();
	BindMatrices.SetNum(BoneCount);
	ExportBones.SetNum(BoneCount);

	FPlane4f PMax(FVector4f(FLT_MAX, FLT_MAX, FLT_MAX, FLT_MAX));
	FPlane4f PMin(FVector4f(-FLT_MAX, -FLT_MAX, -FLT_MAX, -FLT_MAX));
	MinBind = FMatrix44f(PMax, PMax, PMax, PMax);
	MaxBind = FMatrix44f(PMin, PMin, PMin, PMin);
	for (int32 BoneIndex = 0; BoneIndex < BoneCount; BoneIndex++)
	{
		FTGOR_ExportBone& Bone = ExportBones[BoneIndex];

		// Bone properties
		Bone.Name = OutputSkeleton.GetBoneName(BoneIndex).ToString();
		GetDirectChildBones(OutputSkeleton, BoneIndex, Bone.Children);

		// Local transform
		FTransform BoneTransform = OutputSkeleton.GetRefBonePose()[BoneIndex];
		Bone.Location = FVector3f(BoneTransform.GetLocation());
		Bone.Rotation = FQuat4f(BoneTransform.GetRotation());
		Bone.Scale = FVector3f(BoneTransform.GetScale3D());


		// Set bind matrix
		FMatrix44f& Matrix = BindMatrices[BoneIndex];
		Matrix = OutputInvMatrices[BoneIndex];

		for (int32 I = 0; I < 4; I++)
		{
			for (int32 J = 0; J < 4; J++)
			{
				MinBind.M[I][J] = FMath::Min(MinBind.M[I][J], Matrix.M[I][J]);
				MaxBind.M[I][J] = FMath::Max(MaxBind.M[I][J], Matrix.M[I][J]);
			}
		}
	}

	// Create morph data
	const TArray<UMorphTarget*>& OutputMorphTargets = MergeOutput.Mesh->GetMorphTargets();
	MorphCount = OutputMorphTargets.Num();
	ExportMorphTargets.SetNum(MorphCount);
	MorphBuffers.SetNumUninitialized(MorphCount);

	for (int32 MorphIndex = 0; MorphIndex < MorphCount; MorphIndex++)
	{
		FTGOR_ExportTarget& MorphTarget = ExportMorphTargets[MorphIndex];
		TArray<FTGOR_ExportMorph>& MorphBuffer = MorphBuffers[MorphIndex];
		MorphBuffer.SetNum(VertexCount);

		MorphTarget.MinMorph = FTGOR_ExportMorph();
		MorphTarget.MinMorph.Position = FVector3f(FLT_MAX);
		MorphTarget.MinMorph.Normal = FVector3f(FLT_MAX);

		MorphTarget.MaxMorph = FTGOR_ExportMorph();
		MorphTarget.MaxMorph.Position = FVector3f(-FLT_MAX);
		MorphTarget.MaxMorph.Normal = FVector3f(-FLT_MAX);

		UMorphTarget* MorphAsset = OutputMorphTargets[MorphIndex];
		const FMorphTargetLODModel& Model = MorphAsset->GetMorphLODModels()[0];
		for (const FMorphTargetDelta& Delta : Model.Vertices)
		{
			FTGOR_ExportMorph& Morph = MorphBuffer[Delta.SourceIdx];
			Morph.Position = Delta.PositionDelta;
			Morph.Normal = Delta.TangentZDelta;

			MorphTarget.MinMorph.Position = MorphTarget.MinMorph.Position.ComponentMin(Morph.Position);
			MorphTarget.MinMorph.Normal = MorphTarget.MinMorph.Normal.ComponentMin(Morph.Normal);

			MorphTarget.MaxMorph.Position = MorphTarget.MaxMorph.Position.ComponentMax(Morph.Position);
			MorphTarget.MaxMorph.Normal = MorphTarget.MaxMorph.Normal.ComponentMax(Morph.Normal);
		}

		MorphTarget.Default = 0.0f; // TODO: Read from customisation
		MorphTarget.Name = MorphAsset->GetName();
	}

	// Generate section data
	SectionCount = LODRenderData.RenderSections.Num();
	ExportSections.SetNum(SectionCount);

	for (int32 SectionIndex = 0; SectionIndex < SectionCount; SectionIndex++)
	{
		FTGOR_ExportSection& Section = ExportSections[SectionIndex];
		const FTGOR_BodypartSectionOutput& SectionOutput = MergeOutput.Sections[SectionIndex];
		Section.Name = SectionOutput.Section->GetDefaultName();

		Section.BaseIndex = LODRenderData.RenderSections[SectionIndex].BaseIndex;
		Section.IndexCount = LODRenderData.RenderSections[SectionIndex].NumTriangles * 3;
		Section.BaseVertex = LODRenderData.RenderSections[SectionIndex].BaseVertexIndex;
		Section.VertexCount = LODRenderData.RenderSections[SectionIndex].NumVertices;

		Section.MinVertex = FTGOR_ExportVertex();
		Section.MaxVertex = FTGOR_ExportVertex();
		Section.MinVertex.Position = FVector3f(FLT_MAX);
		Section.MinVertex.Normal = FVector3f(FLT_MAX);
		Section.MinVertex.Tangent = FVector4f(FLT_MAX, FLT_MAX, FLT_MAX, FLT_MAX);
		Section.MinVertex.UV = FVector2f(FLT_MAX);
		Section.MinVertex.Color = FVector4f(FLT_MAX, FLT_MAX, FLT_MAX, FLT_MAX);
		Section.MinVertex.Joints0 = FShortVector4(UINT16_MAX);
		Section.MinVertex.Joints1 = FShortVector4(UINT16_MAX);
		Section.MinVertex.Weights0 = FByteVector4(BYTE_MAX);
		Section.MinVertex.Weights1 = FByteVector4(BYTE_MAX);

		Section.MaxVertex.Position = FVector3f(-FLT_MAX);
		Section.MaxVertex.Normal = FVector3f(-FLT_MAX);
		Section.MaxVertex.Tangent = FVector4f(-FLT_MAX, -FLT_MAX, -FLT_MAX, -FLT_MAX);
		Section.MaxVertex.UV = FVector2f(-FLT_MAX);
		Section.MaxVertex.Color = FVector4f(-FLT_MAX, -FLT_MAX, -FLT_MAX, -FLT_MAX);
		Section.MaxVertex.Joints0 = FShortVector4(0);
		Section.MaxVertex.Joints1 = FShortVector4(0);
		Section.MaxVertex.Weights0 = FByteVector4(0);
		Section.MaxVertex.Weights1 = FByteVector4(0);

		// Compute min/max values for packing
		for (int32 VertexIndex = Section.BaseVertex; VertexIndex < Section.BaseVertex + Section.VertexCount; VertexIndex++)
		{
			const FTGOR_ExportVertex& Vertex = ExportVertices[VertexIndex];

			Section.MinVertex.Position = Section.MinVertex.Position.ComponentMin(Vertex.Position);
			Section.MinVertex.Normal = Section.MinVertex.Normal.ComponentMin(Vertex.Normal);
			Section.MinVertex.Tangent = Min4(Section.MinVertex.Tangent, Vertex.Tangent);
			Section.MinVertex.UV = FVector2f::Min(Section.MinVertex.UV, Vertex.UV);
			Section.MinVertex.Color = Min4(Section.MinVertex.Color, Vertex.Color);
			Section.MinVertex.Joints0 = Min4(Section.MinVertex.Joints0, Vertex.Joints0);
			Section.MinVertex.Joints1 = Min4(Section.MinVertex.Joints1, Vertex.Joints1);
			Section.MinVertex.Weights0 = Min4(Section.MinVertex.Weights0, Vertex.Weights0);
			Section.MinVertex.Weights1 = Min4(Section.MinVertex.Weights1, Vertex.Weights1);

			Section.MaxVertex.Position = Section.MaxVertex.Position.ComponentMax(Vertex.Position);
			Section.MaxVertex.Normal = Section.MaxVertex.Normal.ComponentMax(Vertex.Normal);
			Section.MaxVertex.Tangent = Max4(Section.MaxVertex.Tangent, Vertex.Tangent);
			Section.MaxVertex.UV = FVector2f::Max(Section.MaxVertex.UV, Vertex.UV);
			Section.MaxVertex.Color = Max4(Section.MaxVertex.Color, Vertex.Color);
			Section.MaxVertex.Joints0 = Max4(Section.MaxVertex.Joints0, Vertex.Joints0);
			Section.MaxVertex.Joints1 = Max4(Section.MaxVertex.Joints1, Vertex.Joints1);
			Section.MaxVertex.Weights0 = Max4(Section.MaxVertex.Weights0, Vertex.Weights0);
			Section.MaxVertex.Weights1 = Max4(Section.MaxVertex.Weights1, Vertex.Weights1);
		}

		// Recenter face indices
		for (int32 FaceIndex = Section.BaseIndex; FaceIndex < Section.BaseIndex + Section.IndexCount; FaceIndex++)
		{
			IndexBuffer[FaceIndex] -= Section.BaseVertex;
		}

		// Assign image data
		Section.EmissiveTexture = INDEX_NONE;
		Section.OcclusionTexture = INDEX_NONE;
		Section.NormalTexture = INDEX_NONE;

		for (UTGOR_Canvas* Canvas : SectionOutput.Textures)
		{
			const int32 TextureIndex = CanvasMapping[Canvas];
			if (Canvas->ExportType == ETGOR_PBRExport::Emissive)
			{
				Section.EmissiveTexture = TextureIndex;
			}
			if (Canvas->ExportType == ETGOR_PBRExport::Occlusion)
			{
				Section.OcclusionTexture = TextureIndex;
			}
			if (Canvas->ExportType == ETGOR_PBRExport::Normal)
			{
				Section.NormalTexture = TextureIndex;
			}
		}
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void FTGOR_SkeletalMeshExport::GenerateBuffers()
{
	// Write all to buffers
	MeshBuffer.Empty();
	FMemoryWriter MeshWriter(MeshBuffer);

	BindBufferOffset = MeshBuffer.Num();
	MeshWriter.Serialize(BindMatrices.GetData(), BindMatrices.Num() * BindMatrices.GetTypeSize());
	IndexBufferOffset = MeshBuffer.Num();
	MeshWriter.Serialize(IndexBuffer.GetData(), IndexBuffer.Num() * IndexBuffer.GetTypeSize());
	VertexBufferOffset = MeshBuffer.Num();
	MeshWriter.Serialize(ExportVertices.GetData(), ExportVertices.Num() * ExportVertices.GetTypeSize());

	for (int32 MorphIndex = 0; MorphIndex < MorphCount; MorphIndex++)
	{
		FTGOR_ExportTarget& MorphTarget = ExportMorphTargets[MorphIndex];
		TArray<FTGOR_ExportMorph>& MorphBuffer = MorphBuffers[MorphIndex];

		MorphTarget.BufferOffset = MeshBuffer.Num();
		MeshWriter.Serialize(MorphBuffer.GetData(), MorphBuffer.Num() * MorphBuffer.GetTypeSize());
	}

	TextureBuffers.SetNum(TextureCount);
	IImageWrapperModule* ImageWrapperModule = FModuleManager::GetModulePtr<IImageWrapperModule>("ImageWrapper");
	if (ImageWrapperModule)
	{
		TSharedPtr<IImageWrapper> ImageWrapper = ImageWrapperModule->CreateImageWrapper(EImageFormat::PNG);
		if (ImageWrapper.IsValid())
		{
			for (int32 TextureIndex = 0; TextureIndex < TextureCount; TextureIndex++)
			{
				TArray<uint8>& TextureBuffer = TextureBuffers[TextureIndex];
				FTGOR_ExportTexture& Texture = ExportTextures[TextureIndex];

				TextureBuffer.Empty();
				Texture.IsValid = false;

				TArray<FColor> SurfData;
				FRenderTarget* RenderTarget = Texture.Texture->GameThread_GetRenderTargetResource();
				if (RenderTarget)
				{
					RenderTarget->ReadPixels(SurfData);

					const FIntPoint Size = RenderTarget->GetSizeXY();
					if (ImageWrapper->SetRaw(SurfData.GetData(), SurfData.Num() * sizeof(FColor), Size.X, Size.Y, ERGBFormat::BGRA, 8))
					{
						TextureBuffer = ImageWrapper->GetCompressed(0);
						Texture.IsValid = (TextureBuffer.Num() > 0);
					}
				}
			}
		}
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////


enum GL_ComponentType : int32
{
	FLOAT = 5126,
	UNSIGNED_INT = 5125,
	UNSIGNED_SHORT = 5123,
	UNSIGNED_BYTE = 5121
};


template<typename T> struct ComponentTypeSelector { static GL_ComponentType Type(); static bool Normalized(); };
template<typename T> struct VectorTypeSelector { static FString Type(); };

template<> static GL_ComponentType ComponentTypeSelector<FMatrix44f>::Type() { return GL_ComponentType::FLOAT; }
template<> static GL_ComponentType ComponentTypeSelector<FShortVector4>::Type() { return GL_ComponentType::UNSIGNED_SHORT; }
template<> static GL_ComponentType ComponentTypeSelector<FByteVector4>::Type() { return GL_ComponentType::UNSIGNED_BYTE; }
template<> static GL_ComponentType ComponentTypeSelector<FVector4f>::Type() { return GL_ComponentType::FLOAT; }
template<> static GL_ComponentType ComponentTypeSelector<FVector3f>::Type() { return GL_ComponentType::FLOAT; }
template<> static GL_ComponentType ComponentTypeSelector<FVector2f>::Type() { return GL_ComponentType::FLOAT; }
template<> static GL_ComponentType ComponentTypeSelector<uint32>::Type() { return GL_ComponentType::UNSIGNED_INT; }

template<> static bool ComponentTypeSelector<FMatrix44f>::Normalized() { return false; }
template<> static bool ComponentTypeSelector<FShortVector4>::Normalized() { return false; }
template<> static bool ComponentTypeSelector<FByteVector4>::Normalized() { return true; }
template<> static bool ComponentTypeSelector<FVector4f>::Normalized() { return false; }
template<> static bool ComponentTypeSelector<FVector3f>::Normalized() { return false; }
template<> static bool ComponentTypeSelector<FVector2f>::Normalized() { return false; }
template<> static bool ComponentTypeSelector<uint32>::Normalized() { return false; }

template<> static FString VectorTypeSelector<FMatrix44f>::Type() { return "MAT4"; }
template<> static FString VectorTypeSelector<FShortVector4>::Type() { return "VEC4"; }
template<> static FString VectorTypeSelector<FByteVector4>::Type() { return "VEC4"; }
template<> static FString VectorTypeSelector<FVector4f>::Type() { return "VEC4"; }
template<> static FString VectorTypeSelector<FVector3f>::Type() { return "VEC3"; }
template<> static FString VectorTypeSelector<FVector2f>::Type() { return "VEC2"; }
template<> static FString VectorTypeSelector<uint32>::Type() { return "SCALAR"; }

TArray<TSharedPtr<FJsonValue>> RegisterVector(const FMatrix44f& Vector)
{
	TArray<TSharedPtr<FJsonValue>> List;
	for (int32 I = 0; I < 4; I++)
	{
		for (int32 J = 0; J < 4; J++)
		{
			List.Emplace(MakeShareable(new FJsonValueNumber(Vector.M[I][J])));
		}
	}
	return List;
}

TArray<TSharedPtr<FJsonValue>> RegisterVector(const FShortVector4& Vector)
{
	TArray<TSharedPtr<FJsonValue>> List;
	List.Emplace(MakeShareable(new FJsonValueNumber(Vector.X)));
	List.Emplace(MakeShareable(new FJsonValueNumber(Vector.Y)));
	List.Emplace(MakeShareable(new FJsonValueNumber(Vector.Z)));
	List.Emplace(MakeShareable(new FJsonValueNumber(Vector.W)));
	return List;
}

TArray<TSharedPtr<FJsonValue>> RegisterVector(const FByteVector4& Vector)
{
	TArray<TSharedPtr<FJsonValue>> List;
	List.Emplace(MakeShareable(new FJsonValueNumber(Vector.X)));
	List.Emplace(MakeShareable(new FJsonValueNumber(Vector.Y)));
	List.Emplace(MakeShareable(new FJsonValueNumber(Vector.Z)));
	List.Emplace(MakeShareable(new FJsonValueNumber(Vector.W)));
	return List;
}

TArray<TSharedPtr<FJsonValue>> RegisterVector(const FVector4f& Vector)
{
	TArray<TSharedPtr<FJsonValue>> List;
	List.Emplace(MakeShareable(new FJsonValueNumber(Vector.X)));
	List.Emplace(MakeShareable(new FJsonValueNumber(Vector.Y)));
	List.Emplace(MakeShareable(new FJsonValueNumber(Vector.Z)));
	List.Emplace(MakeShareable(new FJsonValueNumber(Vector.W)));
	return List;
}

TArray<TSharedPtr<FJsonValue>> RegisterVector(const FVector3f& Vector)
{
	TArray<TSharedPtr<FJsonValue>> List;
	List.Emplace(MakeShareable(new FJsonValueNumber(Vector.X)));
	List.Emplace(MakeShareable(new FJsonValueNumber(Vector.Y)));
	List.Emplace(MakeShareable(new FJsonValueNumber(Vector.Z)));
	return List;
}

TArray<TSharedPtr<FJsonValue>> RegisterVector(const FVector2f& Vector)
{
	TArray<TSharedPtr<FJsonValue>> List;
	List.Emplace(MakeShareable(new FJsonValueNumber(Vector.X)));
	List.Emplace(MakeShareable(new FJsonValueNumber(Vector.Y)));
	return List;
}

TArray<TSharedPtr<FJsonValue>> RegisterVector(uint32 Vector)
{
	TArray<TSharedPtr<FJsonValue>> List;
	List.Emplace(MakeShareable(new FJsonValueNumber(Vector)));
	return List;
}

#include "RealitiesUtility/Utility/TGOR_Error.h"

////////////////////////////////////////////////////////////////////////////////////////////////////

TArray<TSharedPtr<FJsonValue>> FTGOR_SkeletalMeshExport::RegisterBuffers()
{
	TArray<TSharedPtr<FJsonValue>> Buffers;

	{
		TSharedPtr<FJsonObject> Buffer = MakeShareable(new FJsonObject);
		Buffer->SetNumberField("byteLength", MeshBuffer.Num());
		Buffer->SetStringField("uri", FString("data:application/octet-stream;base64,") + FBase64::Encode(MeshBuffer));
		MeshBufferIndex = Buffers.Emplace(MakeShareable(new FJsonValueObject(Buffer)));
	}

	for (int32 TextureIndex = 0; TextureIndex < TextureCount; TextureIndex++)
	{
		FTGOR_ExportTexture& Texture = ExportTextures[TextureIndex];
		const TArray<uint8>& TextureBuffer = TextureBuffers[TextureIndex];

		if(Texture.IsValid)
		{
			TSharedPtr<FJsonObject> Buffer = MakeShareable(new FJsonObject);
			Buffer->SetNumberField("byteLength", TextureBuffer.Num());
			Buffer->SetStringField("uri", FString("data:application/octet-stream;base64,") + FBase64::Encode(TextureBuffer));
			Texture.Buffer = Buffers.Emplace(MakeShareable(new FJsonValueObject(Buffer)));
		}
	}

	return Buffers;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

TArray<TSharedPtr<FJsonValue>> FTGOR_SkeletalMeshExport::RegisterBufferViews()
{
	TArray<TSharedPtr<FJsonValue>> BufferViews;

	{
		TSharedPtr<FJsonObject> InverseBindBufferView = MakeShareable(new FJsonObject);
		InverseBindBufferView->SetNumberField("buffer", MeshBufferIndex);
		InverseBindBufferView->SetNumberField("byteLength", BoneCount * sizeof(FMatrix));
		InverseBindBufferView->SetNumberField("byteOffset", BindBufferOffset);
		InverseBindBufferView->SetNumberField("target", 34963);
		InverseBindViewIndex = BufferViews.Emplace(MakeShareable(new FJsonValueObject(InverseBindBufferView)));
	}

	for (int32 SectionIndex = 0; SectionIndex < SectionCount; SectionIndex++)
	{
		FTGOR_ExportSection& Section = ExportSections[SectionIndex];

		{
			TSharedPtr<FJsonObject> IndexBufferView = MakeShareable(new FJsonObject);
			IndexBufferView->SetNumberField("buffer", MeshBufferIndex);
			IndexBufferView->SetNumberField("byteLength", Section.IndexCount * sizeof(uint32));
			IndexBufferView->SetNumberField("byteOffset", IndexBufferOffset + Section.BaseIndex * sizeof(uint32));
			IndexBufferView->SetNumberField("target", 34963);
			Section.IndexBufferView = BufferViews.Emplace(MakeShareable(new FJsonValueObject(IndexBufferView)));
		}

		{
			TSharedPtr<FJsonObject> VertexBufferView = MakeShareable(new FJsonObject);
			VertexBufferView->SetNumberField("buffer", MeshBufferIndex);
			VertexBufferView->SetNumberField("byteLength", Section.VertexCount * sizeof(FTGOR_ExportVertex));
			VertexBufferView->SetNumberField("byteOffset", VertexBufferOffset + Section.BaseVertex * sizeof(FTGOR_ExportVertex));
			VertexBufferView->SetNumberField("byteStride", sizeof(FTGOR_ExportVertex));
			VertexBufferView->SetNumberField("target", 34962);
			Section.VertexBufferView = BufferViews.Emplace(MakeShareable(new FJsonValueObject(VertexBufferView)));
		}

		for (int32 MorphIndex = 0; MorphIndex < MorphCount; MorphIndex++)
		{
			FTGOR_ExportTarget& MorphTarget = ExportMorphTargets[MorphIndex];

			{
				TSharedPtr<FJsonObject> MorphBufferView = MakeShareable(new FJsonObject);
				MorphBufferView->SetNumberField("buffer", MeshBufferIndex);
				MorphBufferView->SetNumberField("byteLength", Section.VertexCount * sizeof(FTGOR_ExportMorph));
				MorphBufferView->SetNumberField("byteOffset", MorphTarget.BufferOffset + Section.BaseVertex * sizeof(FTGOR_ExportMorph));
				MorphBufferView->SetNumberField("byteStride", sizeof(FTGOR_ExportMorph));
				MorphBufferView->SetNumberField("target", 34962);
				MorphTarget.BufferView = BufferViews.Emplace(MakeShareable(new FJsonValueObject(MorphBufferView)));
			}
		}
	}

	for (int32 TextureIndex = 0; TextureIndex < TextureCount; TextureIndex++)
	{
		FTGOR_ExportTexture& Texture = ExportTextures[TextureIndex];
		const TArray<uint8>& TextureBuffer = TextureBuffers[TextureIndex];

		if (Texture.IsValid)
		{
			TSharedPtr<FJsonObject> TextureBufferView = MakeShareable(new FJsonObject);
			TextureBufferView->SetNumberField("buffer", Texture.Buffer);
			TextureBufferView->SetNumberField("byteLength", TextureBuffer.Num());
			TextureBufferView->SetNumberField("byteOffset", Texture.BufferOffset);
			Texture.BufferView = BufferViews.Emplace(MakeShareable(new FJsonValueObject(TextureBufferView)));
		}
	}

	return BufferViews;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

template<typename T>
TSharedPtr<FJsonValue> RegisterAccessor(
	int32 BufferView,
	int32 ByteOffset, 
	int32 Count,
	const T& Min, const T& Max)
{
	TSharedPtr<FJsonObject> Accessor = MakeShareable(new FJsonObject);
	Accessor->SetNumberField("bufferView", BufferView);
	Accessor->SetNumberField("byteOffset", ByteOffset);
	Accessor->SetNumberField("componentType", ComponentTypeSelector<T>::Type());
	Accessor->SetBoolField("normalized", ComponentTypeSelector<T>::Normalized());
	Accessor->SetNumberField("count", Count);

	Accessor->SetArrayField("min", RegisterVector(Min));
	Accessor->SetArrayField("max", RegisterVector(Max));

	Accessor->SetStringField("type", VectorTypeSelector<T>::Type());
	return MakeShareable(new FJsonValueObject(Accessor));
}


TArray<TSharedPtr<FJsonValue>> FTGOR_SkeletalMeshExport::RegisterAccessors()
{
	TArray<TSharedPtr<FJsonValue>> Accessors;

	// ...
	InverseBindAccessorIndex = Accessors.Emplace(RegisterAccessor<FMatrix44f>(InverseBindViewIndex, 0, BoneCount, MinBind, MaxBind));

	for (int32 SectionIndex = 0; SectionIndex < SectionCount; SectionIndex++)
	{
		FTGOR_ExportSection& Section = ExportSections[SectionIndex];

		Section.IndexAccessor = Accessors.Emplace(RegisterAccessor<uint32>(Section.IndexBufferView, 0, Section.IndexCount, 0, Section.VertexCount-1));


#define VERTEX_ACCESSOR(N) Section.N##Accessor = Accessors.Emplace(RegisterAccessor(Section.VertexBufferView, offsetof(FTGOR_ExportVertex, N), Section.VertexCount, Section.MinVertex.##N, Section.MaxVertex.##N))

		VERTEX_ACCESSOR(Position);
		VERTEX_ACCESSOR(Normal);
		VERTEX_ACCESSOR(Tangent);
		VERTEX_ACCESSOR(UV);
		VERTEX_ACCESSOR(Color);
		VERTEX_ACCESSOR(Joints0);
		VERTEX_ACCESSOR(Joints1);
		VERTEX_ACCESSOR(Weights0);
		VERTEX_ACCESSOR(Weights1);

		for (int32 MorphIndex = 0; MorphIndex < MorphCount; MorphIndex++)
		{
			FTGOR_ExportTarget& MorphTarget = ExportMorphTargets[MorphIndex];

#define MORPH_ACCESSOR(N) MorphTarget.N##Accessor = Accessors.Emplace(RegisterAccessor(MorphTarget.BufferView, offsetof(FTGOR_ExportMorph, N), Section.VertexCount, MorphTarget.MinMorph.##N, MorphTarget.MinMorph.##N))

			MORPH_ACCESSOR(Position);
			MORPH_ACCESSOR(Normal);
		}
	}

	return Accessors;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

TArray<TSharedPtr<FJsonValue>> FTGOR_SkeletalMeshExport::RegisterImages()
{
	TArray<TSharedPtr<FJsonValue>> Images;

	for (int32 TextureIndex = 0; TextureIndex < TextureCount; TextureIndex++)
	{
		FTGOR_ExportTexture& Texture = ExportTextures[TextureIndex];

		if(Texture.IsValid)
		{
			TSharedPtr<FJsonObject> Image = MakeShareable(new FJsonObject);
			Image->SetNumberField("bufferView", Texture.BufferView);
			Image->SetStringField("mimeType", "image/png");
			Texture.Image = Images.Emplace(MakeShareable(new FJsonValueObject(Image)));
		}
	}

	return Images;
}

TArray<TSharedPtr<FJsonValue>> FTGOR_SkeletalMeshExport::RegisterSamplers()
{
	TArray<TSharedPtr<FJsonValue>> Samplers;

	{
		TSharedPtr<FJsonObject> Sampler = MakeShareable(new FJsonObject);
		Sampler->SetNumberField("magFilter", 9729);
		Sampler->SetNumberField("minFilter", 9987);
		Sampler->SetNumberField("wrapS", 10497);
		Sampler->SetNumberField("wrapT", 10497);

		SamplerIndex = Samplers.Emplace(MakeShareable(new FJsonValueObject(Sampler)));
	}

	return Samplers;
}

TArray<TSharedPtr<FJsonValue>> FTGOR_SkeletalMeshExport::RegisterTextures()
{
	TArray<TSharedPtr<FJsonValue>> Textures;

	for (int32 TextureIndex = 0; TextureIndex < TextureCount; TextureIndex++)
	{
		FTGOR_ExportTexture& ExportTexture = ExportTextures[TextureIndex];

		if (ExportTexture.IsValid)
		{
			TSharedPtr<FJsonObject> Texture = MakeShareable(new FJsonObject);
			Texture->SetNumberField("source", ExportTexture.Image);
			Texture->SetNumberField("sampler", SamplerIndex);

			Textures.Emplace(MakeShareable(new FJsonValueObject(Texture)));
		}
	}

	return Textures;
}

TArray<TSharedPtr<FJsonValue>> FTGOR_SkeletalMeshExport::RegisterMaterials()
{
	TArray<TSharedPtr<FJsonValue>> Materials;

	for (int32 SectionIndex = 0; SectionIndex < SectionCount; SectionIndex++)
	{
		const FTGOR_ExportSection& Section = ExportSections[SectionIndex];

		{
			TSharedPtr<FJsonObject> Material = MakeShareable(new FJsonObject);

			Material->SetStringField("name", Section.Name);

			if (Section.EmissiveTexture != INDEX_NONE)
			{
				TSharedPtr<FJsonObject> EmissiveTexture = MakeShareable(new FJsonObject);

				EmissiveTexture->SetNumberField("index", Section.EmissiveTexture);
				EmissiveTexture->SetNumberField("texCoord", 0);

				Material->SetObjectField("emissiveTexture", EmissiveTexture);
			}

			if (Section.OcclusionTexture != INDEX_NONE)
			{
				TSharedPtr<FJsonObject> OcclusionTexture = MakeShareable(new FJsonObject);

				OcclusionTexture->SetNumberField("index", Section.OcclusionTexture);
				OcclusionTexture->SetNumberField("texCoord", 0);

				Material->SetObjectField("occlusionTexture", OcclusionTexture);
			}

			if (Section.NormalTexture != INDEX_NONE)
			{
				TSharedPtr<FJsonObject> NormalTexture = MakeShareable(new FJsonObject);

				NormalTexture->SetNumberField("index", Section.NormalTexture);
				NormalTexture->SetNumberField("texCoord", 0);

				Material->SetObjectField("normalTexture", NormalTexture);
			}

			Materials.Emplace(MakeShareable(new FJsonValueObject(Material)));
		}
	}

	return Materials;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

TArray<TSharedPtr<FJsonValue>> FTGOR_SkeletalMeshExport::RegisterSkins()
{
	TArray<TSharedPtr<FJsonValue>> Skins;

	SkeletonIndex = 1;
	{
		TSharedPtr<FJsonObject> Skin = MakeShareable(new FJsonObject);

		Skin->SetStringField("name", "Skin");
		Skin->SetNumberField("inverseBindMatrices", InverseBindAccessorIndex);

		{
			TArray<TSharedPtr<FJsonValue>> Joints;

			for (int32 BoneIndex = 0; BoneIndex < BoneCount; BoneIndex++)
			{
				Joints.Emplace(MakeShareable(new FJsonValueNumber(BoneIndex + SkeletonIndex)));
			}

			Skin->SetArrayField("joints", Joints);
		}

		Skin->SetNumberField("skeleton", SkeletonIndex);

		SkinIndex = Skins.Emplace(MakeShareable(new FJsonValueObject(Skin)));
	}

	return Skins;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

TArray<TSharedPtr<FJsonValue>> FTGOR_SkeletalMeshExport::RegisterMeshes()
{
	TArray<TSharedPtr<FJsonValue>> Meshes;

	{
		TSharedPtr<FJsonObject> RootMesh = MakeShareable(new FJsonObject);

		// ...
		RootMesh->SetStringField("name", "Mesh");

		// ...
		{
			TArray<TSharedPtr<FJsonValue>> Primitives;

			for (int32 SectionIndex = 0; SectionIndex < SectionCount; SectionIndex++)
			{
				const FTGOR_ExportSection& Section = ExportSections[SectionIndex];

				{
					TSharedPtr<FJsonObject> Primitive = MakeShareable(new FJsonObject);

					// ...
					{
						TSharedPtr<FJsonObject> Attributes = MakeShareable(new FJsonObject);

						Attributes->SetNumberField("POSITION", Section.PositionAccessor);
						Attributes->SetNumberField("NORMAL", Section.NormalAccessor);
						Attributes->SetNumberField("TANGENT", Section.TangentAccessor);
						Attributes->SetNumberField("TEXCOORD_0", Section.UVAccessor);
						Attributes->SetNumberField("COLOR_0", Section.ColorAccessor);
						Attributes->SetNumberField("JOINTS_0", Section.Joints0Accessor);
						Attributes->SetNumberField("JOINTS_1", Section.Joints1Accessor);
						Attributes->SetNumberField("WEIGHTS_0", Section.Weights0Accessor);
						Attributes->SetNumberField("WEIGHTS_1", Section.Weights1Accessor);

						Primitive->SetObjectField("attributes", Attributes);
					}

					// ...
					Primitive->SetNumberField("indices", Section.IndexAccessor);
					Primitive->SetNumberField("material", SectionIndex);
					Primitive->SetNumberField("mode", 4);

					// ...
					if(MorphCount > 0)
					{
						TArray<TSharedPtr<FJsonValue>> Targets;

						for (int32 MorphIndex = 0; MorphIndex < MorphCount; MorphIndex++)
						{
							FTGOR_ExportTarget& MorphTarget = ExportMorphTargets[MorphIndex];

							{
								TSharedPtr<FJsonObject> Target = MakeShareable(new FJsonObject);

								Target->SetNumberField("POSITION", MorphTarget.PositionAccessor);
								Target->SetNumberField("NORMAL", MorphTarget.NormalAccessor);

								Targets.Emplace(MakeShareable(new FJsonValueObject(Target)));
							}
						}

						Primitive->SetArrayField("targets", Targets);
					}

					Primitives.Emplace(MakeShareable(new FJsonValueObject(Primitive)));
				}
			}
			RootMesh->SetArrayField("primitives", Primitives);
		}

		// ...
		if (MorphCount > 0)
		{

			TArray<TSharedPtr<FJsonValue>> Weights;

			for (int32 MorphIndex = 0; MorphIndex < MorphCount; MorphIndex++)
			{
				FTGOR_ExportTarget& MorphTarget = ExportMorphTargets[MorphIndex];
				Weights.Emplace(MakeShareable(new FJsonValueNumber(MorphTarget.Default)));
			}
			RootMesh->SetArrayField("weights", Weights);
		}


		// ...
		{
			TSharedPtr<FJsonObject> Extras = MakeShareable(new FJsonObject);

			if (MorphCount > 0)
			{
				TArray<TSharedPtr<FJsonValue>> TargetNames;

				for (int32 MorphIndex = 0; MorphIndex < MorphCount; MorphIndex++)
				{
					FTGOR_ExportTarget& MorphTarget = ExportMorphTargets[MorphIndex];


					TargetNames.Emplace(MakeShareable(new FJsonValueString(MorphTarget.Name)));
				}
				Extras->SetArrayField("targetMames", TargetNames);
			}

			Extras->SetObjectField("extras", Extras);
		}


		MeshIndex = Meshes.Emplace(MakeShareable(new FJsonValueObject(RootMesh)));
	}

	return Meshes;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

TArray<TSharedPtr<FJsonValue>> FTGOR_SkeletalMeshExport::RegisterNodes()
{
	TArray<TSharedPtr<FJsonValue>> Nodes;

	{
		TSharedPtr<FJsonObject> ArmatureNode = MakeShareable(new FJsonObject);

		ArmatureNode->SetStringField("name", "Armature");

		TArray<TSharedPtr<FJsonValue>> Children;
		Children.Emplace(MakeShareable(new FJsonValueNumber(SkeletonIndex)));
		ArmatureNode->SetArrayField("children", Children);

		ArmatureNode->SetStringField("name", "Mesh");
		ArmatureNode->SetNumberField("mesh", MeshIndex);
		ArmatureNode->SetNumberField("skin", SkinIndex);

		Nodes.Emplace(MakeShareable(new FJsonValueObject(ArmatureNode)));
	}

	// Define skeleton
	for (int32 BoneIndex = 0; BoneIndex < BoneCount; BoneIndex++)
	{
		FTGOR_ExportBone& Bone = ExportBones[BoneIndex];

		{
			TSharedPtr<FJsonObject> Node = MakeShareable(new FJsonObject);

			// ...
			Node->SetStringField("name", Bone.Name);

			// ...
			if(Bone.Children.Num() > 0)
			{
				TArray<TSharedPtr<FJsonValue>> Children;
				for (int32 Child : Bone.Children)
				{
					Children.Emplace(MakeShareable(new FJsonValueNumber(Child + SkeletonIndex)));
				}
				Node->SetArrayField("children", Children);
			}

			// ...
			{
				TArray<TSharedPtr<FJsonValue>> Translation;
				Translation.Emplace(MakeShareable(new FJsonValueNumber(Bone.Location.X)));
				Translation.Emplace(MakeShareable(new FJsonValueNumber(Bone.Location.Y)));
				Translation.Emplace(MakeShareable(new FJsonValueNumber(Bone.Location.Z)));
				Node->SetArrayField("translation", Translation);
			}

			// ...
			{
				TArray<TSharedPtr<FJsonValue>> Rotation;
				Rotation.Emplace(MakeShareable(new FJsonValueNumber(Bone.Rotation.X)));
				Rotation.Emplace(MakeShareable(new FJsonValueNumber(Bone.Rotation.Y)));
				Rotation.Emplace(MakeShareable(new FJsonValueNumber(Bone.Rotation.Z)));
				Rotation.Emplace(MakeShareable(new FJsonValueNumber(Bone.Rotation.W)));
				Node->SetArrayField("rotation", Rotation);
			}

			// ...
			{
				TArray<TSharedPtr<FJsonValue>> Scale;
				Scale.Emplace(MakeShareable(new FJsonValueNumber(Bone.Scale.X)));
				Scale.Emplace(MakeShareable(new FJsonValueNumber(Bone.Scale.Y)));
				Scale.Emplace(MakeShareable(new FJsonValueNumber(Bone.Scale.Z)));
				Node->SetArrayField("scale", Scale);
			}

			Nodes.Emplace(MakeShareable(new FJsonValueObject(Node)));
		}
	}

	return Nodes;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

TArray<TSharedPtr<FJsonValue>> FTGOR_SkeletalMeshExport::RegisterScenes()
{
	TArray<TSharedPtr<FJsonValue>> Scenes;

	{
		TSharedPtr<FJsonObject> Scene = MakeShareable(new FJsonObject);

		Scene->SetStringField("name", "Scene");

		{
			TArray<TSharedPtr<FJsonValue>> Nodes;
			Nodes.Emplace(MakeShareable(new FJsonValueNumber(0)));
			Scene->SetArrayField("nodes", Nodes);
		}

		Scenes.Emplace(MakeShareable(new FJsonValueObject(Scene)));
	}

	return Scenes;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

#if WITH_EDITOR
bool FTGOR_SkeletalMeshExport::OpenFileDialogue(const FString& FileName, FString& OutFilePath)
{
	const FString NDisplayFileDescription = LOCTEXT("GLTFDescription", "GLTF Export").ToString();
	const FString NDisplayFileExtension = TEXT("*.gltf");
	const FString FileTypes = FString::Printf(TEXT("%s (%s)|%s"), *NDisplayFileDescription, *NDisplayFileExtension, *NDisplayFileExtension);

	// Prompt the user for the filenames
	TArray<FString> OpenFilenames;
	IDesktopPlatform* DesktopPlatform = FDesktopPlatformModule::Get();
	bool bFileSelected = false;

	// Open file dialog
	if (DesktopPlatform)
	{
		const void* ParentWindowWindowHandle = FSlateApplication::Get().FindBestParentWindowHandleForDialogs(nullptr);

		bFileSelected = DesktopPlatform->SaveFileDialog(
			ParentWindowWindowHandle,
			LOCTEXT("ExportDialogTitle", "Export").ToString(),
			FPaths::ConvertRelativePathToFull(FPaths::ProjectUserDir()),
			FileName,
			FileTypes,
			EFileDialogFlags::None,
			OpenFilenames
		);
	}

	// Load directory
	if (bFileSelected)
	{
		if (OpenFilenames.Num() > 0)
		{
			OutFilePath = OpenFilenames[0];
			return true;
		}
	}

	return false;
}


void FTGOR_SkeletalMeshExport::Export(const FString& Filename)
{
	FString FilePath;
	if (!OpenFileDialogue(Filename, FilePath))
	{
		return;
	}


	GenerateBuffers();

	// Indentations are for readability and copy-pasta error safety
	TSharedPtr<FJsonObject> Data = MakeShareable(new FJsonObject);

	// Asset definition
	{
		TSharedPtr<FJsonObject> Asset = MakeShareable(new FJsonObject);
		Asset->SetStringField("version", "2.0");
		Asset->SetStringField("generator", "TGOR_GLTF");
		Asset->SetStringField("copyright", "TGOR");
		Data->SetObjectField("asset", Asset);
	}

	// Buffer definitions
	TArray<TSharedPtr<FJsonValue>> Buffers = RegisterBuffers();
	if (Buffers.Num() > 0)
	{
		Data->SetArrayField("buffers", Buffers);
	}

	// Bufferview definitions
	TArray<TSharedPtr<FJsonValue>> BufferViews = RegisterBufferViews();
	if (BufferViews.Num() > 0)
	{
		Data->SetArrayField("bufferViews", BufferViews);
	}
	
	// Accessor definitions
	TArray<TSharedPtr<FJsonValue>> Accessors = RegisterAccessors();
	if (Accessors.Num() > 0)
	{
		Data->SetArrayField("accessors", Accessors);
	}
	
	// Image definitions
	TArray<TSharedPtr<FJsonValue>> Images = RegisterImages();
	if (Images.Num() > 0)
	{
		Data->SetArrayField("images", Images);
	}

	// Sampler definitions
	TArray<TSharedPtr<FJsonValue>> Samplers = RegisterSamplers();
	if (Samplers.Num() > 0)
	{
		Data->SetArrayField("samplers", Samplers);
	}

	// Texture definitions
	TArray<TSharedPtr<FJsonValue>> Textures = RegisterTextures();
	if (Textures.Num() > 0)
	{
		Data->SetArrayField("textures", Textures);
	}

	// Material definitions
	TArray<TSharedPtr<FJsonValue>> Materials = RegisterMaterials();
	if (Materials.Num() > 0)
	{
		Data->SetArrayField("materials", Materials);
	}

	// Skin definitions
	TArray<TSharedPtr<FJsonValue>> Skins = RegisterSkins();
	if (Skins.Num() > 0)
	{
		Data->SetArrayField("skins", Skins);
	}

	// Mesh definitions
	TArray<TSharedPtr<FJsonValue>> Meshes = RegisterMeshes();
	if (Meshes.Num() > 0)
	{
		Data->SetArrayField("meshes", Meshes);
	}

	// Node definitions
	TArray<TSharedPtr<FJsonValue>> Nodes = RegisterNodes();
	if (Nodes.Num() > 0)
	{
		Data->SetArrayField("nodes", Nodes);
	}

	// Scene definition
	TArray<TSharedPtr<FJsonValue>> Scenes = RegisterScenes();
	if (Scenes.Num() > 0)
	{
		Data->SetNumberField("scene", 0);
		Data->SetArrayField("scenes", Scenes);
	}

	// Write to file
	FString Output;
	TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&Output);
	FJsonSerializer::Serialize(Data.ToSharedRef(), Writer);
	FFileHelper::SaveStringToFile(Output, *FilePath);
}
#endif


#undef LOCTEXT_NAMESPACE
