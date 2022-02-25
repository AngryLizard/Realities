// The Gateway of Realities: Planes of Existence.

#include "TGOR_MergeActor.h"

#include "Engine/Canvas.h"
#include "Kismet/KismetRenderingLibrary.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Animation/MorphTarget.h"

#include "Realities/Base/Creature/TGOR_SkeletalMergeUserData.h"
#include "Realities/Base/Creature/TGOR_SkeletalMerge.h"

#include "Rendering/SkeletalMeshRenderData.h"
#include "Rendering/SkeletalMeshLODRenderData.h"
#include "Realities/Base/Creature/TGOR_TriangleRender.h"

#include "RealitiesUtility/Structures/TGOR_Matrix3x3.h"

#include "DrawDebugHelpers.h"


ATGOR_MergeActor::ATGOR_MergeActor(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;

	RootComponent = ObjectInitializer.CreateDefaultSubobject<USceneComponent>(this, FName(TEXT("RootComponent")));

    TargeComponent = ObjectInitializer.CreateDefaultSubobject<USkeletalMeshComponent>(this, FName(TEXT("TargeComponent")));
    TargeComponent->SetupAttachment(RootComponent);

    Packages.SizeX = 4096;
    Packages.SizeY = 4096;

    DrawDebugBases = false;
}

void ATGOR_MergeActor::DrawToTexture(UTextureRenderTarget2D* Target, UTexture2D* Texture, int32 PosX, int32 PosY)
{
    if (IsValid(Target) && IsValid(Texture))
    {
        UCanvas* Canvas;
        FVector2D Size;
        FDrawToRenderTargetContext Context;
        UKismetRenderingLibrary::BeginDrawCanvasToRenderTarget(this, Target, Canvas, Size, Context);
        Canvas->K2_DrawTexture(Texture, FVector2D(PosX, PosY), FVector2D(Texture->GetSizeX(), Texture->GetSizeY()), FVector2D(0.0f, 0.0f), FVector2D(1.0f, 1.0f));
        UKismetRenderingLibrary::EndDrawCanvasToRenderTarget(this, Context);
    }
}

void ATGOR_MergeActor::DrawToTriangles(UTextureRenderTarget2D* Target, UTGOR_GeometryUserData* Geometry, int32 SectionIndex, const FTransform& PointTransform, UTexture2D* Mask, UTexture2D* Texture, UTexture2D* Parent, UMaterialInstanceDynamic* Material)
{
    if (IsValid(Target) && IsValid(Material))
    {
        UCanvas* Canvas;
        FVector2D Size;
        FDrawToRenderTargetContext Context;
        UKismetRenderingLibrary::BeginDrawCanvasToRenderTarget(this, Target, Canvas, Size, Context);
        
        if (IsValid(Mask))
        {
            Material->SetTextureParameterValue("Mask", Mask);
        }
        if (IsValid(Texture))
        {
            Material->SetTextureParameterValue("Texture", Texture);
        }
        if (IsValid(Parent))
        {
            Material->SetTextureParameterValue("Parent", Parent);
        }

        if (Material && IsValid(Geometry) && Canvas)
        {
            FTGOR_TriangleItem TriangleItem(Geometry, SectionIndex, PointTransform, Material->GetRenderProxy());
            Canvas->DrawItem(TriangleItem);
        }

        //Canvas->K2_DrawMaterialTriangle(Material, Triangles);
        UKismetRenderingLibrary::EndDrawCanvasToRenderTarget(this, Context);
    }
}

FVector2D TransformUV(const FTransform& Transform, const FVector2D& UV)
{
    return FVector2D(Transform.TransformPosition(FVector(UV, 1.f)));
}

void ATGOR_MergeActor::MergeTest()
{
    const FTransform WorldTransform = GetActorTransform();

	USkeletalMesh* TargetMesh = NewObject<USkeletalMesh>();
	if (IsValid(TargetMesh) && IsValid(Packages.Skeleton))
	{
        TargetMesh->Skeleton = Packages.Skeleton;
        CurvatureMerger = nullptr;
        if (IsValid(Packages.CurvatureMerger))
        {
            CurvatureMerger = UMaterialInstanceDynamic::Create(Packages.CurvatureMerger, this);
        }
        NormalsMerger = nullptr;
        if (IsValid(Packages.NormalsMerger))
        {
            NormalsMerger = UMaterialInstanceDynamic::Create(Packages.NormalsMerger, this);
        }
        OcclusionMerger = nullptr;
        if (IsValid(Packages.OcclusionMerger))
        {
            OcclusionMerger = UMaterialInstanceDynamic::Create(Packages.OcclusionMerger, this);
        }

		TArray<USkeletalMesh*> Meshes;
        TArray<FTGOR_SkelMeshMergeSectionMapping> Mappings;
        FTGOR_SkelMeshMergeUVTransforms UVTransforms;

        CurvatureRenderTarget = ManualCurvatureRenderTarget;
        if (!IsValid(CurvatureRenderTarget))
        {
            CurvatureRenderTarget = UKismetRenderingLibrary::CreateRenderTarget2D(this, Packages.SizeX, Packages.SizeY, ETextureRenderTargetFormat::RTF_RGBA8, FLinearColor(0.0f, 0.0f, 0.0f, 0.0f), false);
        }

        NormalsRenderTarget = ManualNormalsRenderTarget;
        if (!IsValid(NormalsRenderTarget))
        {
            NormalsRenderTarget = UKismetRenderingLibrary::CreateRenderTarget2D(this, Packages.SizeX, Packages.SizeY, ETextureRenderTargetFormat::RTF_RG8, FLinearColor(0.0f, 0.0f, 0.0f, 1.0f), false);
        }

        OcclusionRenderTarget = ManualOcclusionRenderTarget;
        if (!IsValid(OcclusionRenderTarget))
        {
            OcclusionRenderTarget = UKismetRenderingLibrary::CreateRenderTarget2D(this, Packages.SizeX, Packages.SizeY, ETextureRenderTargetFormat::RTF_RGBA8, FLinearColor(0.0f, 0.0f, 0.0f, 0.0f), false);
        }

        for(const FTGOR_MeshMergePackage& Package : Packages.Packages)
        {
            Package.Curvature->GetPixelFormat();
            if (IsValid(Package.Mesh) && IsValid(Package.Curvature) && IsValid(Package.Normals) && IsValid(Package.Occlusion))
            {
                Meshes.Emplace(Package.Mesh);

                // ...
                DrawToTexture(CurvatureRenderTarget, Package.Curvature, Package.PosX, Package.PosY);
                DrawToTexture(NormalsRenderTarget, Package.Normals, Package.PosX, Package.PosY);
                DrawToTexture(OcclusionRenderTarget, Package.Occlusion, Package.PosX, Package.PosY);

                // ...
                FTGOR_SkelMeshMergeSectionMapping Mapping;
                Mapping.SectionIDs = Package.Sections;
                Mappings.Emplace(Mapping);

                // ...
                TArray<TArray<FTransform>> SectorTransforms;

                const int32 Num = Sectors.Num();
                for (int32 Index = 0; Index < Num; Index++)
                {
                    const FTGOR_MeshMergeSectors& Sector = Sectors[Index];
                    TArray<FTransform> ChannelTransforms;
                    if (Sector.DoesUVTransform)
                    {
                        FVector2D Size = FVector2D(Packages.SizeX, Packages.SizeY);

                        FTransform Transform;
                        Transform.SetLocation(FVector(Package.PosX, Package.PosY, 0.0f) / FVector(Size, 1.0f));
                        Transform.SetScale3D(FVector(Package.Curvature->GetSizeX(), Package.Curvature->GetSizeY(), 0.0f) / FVector(Size, 1.0f));
                        ChannelTransforms.Emplace(Transform);

                        if (Package.Parent != INDEX_NONE)
                        {
                            const FTGOR_MeshMergePackage& ParentPackage = Packages.Packages[Package.Parent];
                            UTGOR_SkeletalMergeUserData* MergeData = Package.Mesh->GetAssetUserData<UTGOR_SkeletalMergeUserData>();
                            if (IsValid(MergeData))
                            {

                                /*
                                if (IsValid(ManualReferenceRenderTarget))
                                {
                                    auto ColorToNormal = [&](FLinearColor Color) -> FVector {

                                        const FVector2D XY((Color.R - 0.5f) * 2, (Color.G - 0.5f) * 2);
                                        const float Z = FMath::Sqrt(1.0f - (FMath::Square(XY.X) + FMath::Square(XY.Y)));
                                        return FVector(XY, Z);

                                    };

                                    auto NormalToColor = [&](FVector Normal) -> FLinearColor {
                                        return FLinearColor((Normal.X * 0.5f) + 0.5f, (Normal.Y * 0.5f) + 0.5f, (Normal.Z * 0.5f) + 0.5f, 1.0f);
                                    };

                                    auto CheckNormals = [&](FVector2D SourcePoint, FVector2D RenderPoint, FVector Color) -> FLinearColor {

                                        FTransform ParentTransform;
                                        ParentTransform.SetLocation(FVector(ParentPackage.PosX, ParentPackage.PosY, 0.0f) / FVector(Size, 1.0f));
                                        ParentTransform.SetScale3D(FVector(ParentPackage.Curvature->GetSizeX(), ParentPackage.Curvature->GetSizeY(), 0.0f) / FVector(Packages.SizeX, Packages.SizeY, 1.0f));

                                        const FVector2D ParentPoint = TransformUV(ParentTransform, SourcePoint) * Size;

                                        FLinearColor SrcPixel = UKismetRenderingLibrary::ReadRenderTargetRawPixel(this, NormalsRenderTarget, FMath::FloorToInt(ParentPoint.X), FMath::FloorToInt(ParentPoint.Y));
                                        FLinearColor RefPixel = UKismetRenderingLibrary::ReadRenderTargetRawPixel(this, ManualReferenceRenderTarget, FMath::FloorToInt(RenderPoint.X), FMath::FloorToInt(RenderPoint.Y));

                                        const FVector Source = ColorToNormal(SrcPixel / 0xFF);
                                        const FVector Reference = ColorToNormal(RefPixel / 0xFF);

                                        const FVector2D SrcNormal = FVector2D(Source).GetSafeNormal();
                                        const FVector2D SrcOrtho = FVector2D(-SrcNormal.Y, SrcNormal.X);
                                        const FVector2D RefNormal = FVector2D(Reference).GetSafeNormal();

                                        const FVector Rotate = FVector(RefNormal | SrcNormal, RefNormal | SrcOrtho, Reference.Z / Source.Z);

                                        ////////////////////

                                        FVector Result;
                                        Result.X = FVector2D(Rotate.X, -Rotate.Y) | FVector2D(Source);
                                        Result.Y = FVector2D(Rotate.Y, Rotate.X) | FVector2D(Source);

                                        RPORT("----------");
                                        VPORT(Rotate);
                                        VPORT(Color);

                                        /// ////////////////

                                        return NormalToColor(Rotate);
                                    };

                                    for (FTGOR_MergeUVTri& Triangle : Triangles)
                                    {
                                        Triangle.V0_Color = CheckNormals(Triangle.V0_UV, Triangle.V0_Pos, ColorToNormal(Triangle.V0_Color));
                                        Triangle.V1_Color = CheckNormals(Triangle.V1_UV, Triangle.V1_Pos, ColorToNormal(Triangle.V0_Color));
                                        Triangle.V2_Color = CheckNormals(Triangle.V2_UV, Triangle.V2_Pos, ColorToNormal(Triangle.V0_Color));
                                    }
                                }
                                */

                                const FTransform PointTransform = Transform * FTransform(FQuat::Identity, FVector::ZeroVector, FVector(Size, 1.0f));

                                DrawToTriangles(CurvatureRenderTarget, MergeData, Index, PointTransform, ParentPackage.Occlusion, Package.Curvature, ParentPackage.Curvature, CurvatureMerger);
                                DrawToTriangles(NormalsRenderTarget, MergeData, Index, PointTransform, ParentPackage.Occlusion, Package.Normals, ParentPackage.Normals, NormalsMerger);
                                DrawToTriangles(OcclusionRenderTarget, MergeData, Index, PointTransform, ParentPackage.Occlusion, Package.Occlusion, ParentPackage.Occlusion, OcclusionMerger);


                                /*
                                if (IsValid(ManualReferenceRenderTarget))
                                {
                                    
                                    auto CheckPixel = [&](FVector2D SourcePoint, FVector2D RenderPoint) -> float {

                                        FLinearColor TrgPixel = UKismetRenderingLibrary::ReadRenderTargetRawPixel(this, NormalsRenderTarget, FMath::FloorToInt(RenderPoint.X), FMath::FloorToInt(RenderPoint.Y));
                                        FLinearColor RefPixel = UKismetRenderingLibrary::ReadRenderTargetRawPixel(this, ManualReferenceRenderTarget, FMath::RoundToInt(RenderPoint.X), FMath::RoundToInt(RenderPoint.Y));


                                        const float Err = (FVector2D(TrgPixel.R, TrgPixel.G) - FVector2D(RefPixel.R, RefPixel.G)).Size();
                                        RPORT(FString::Printf(TEXT("X: %d Y: %d | R: %.0f / %.0f | G: %.0f / %.0f | Err: %.0f"), FMath::FloorToInt(RenderPoint.X), FMath::FloorToInt(RenderPoint.Y), TrgPixel.R, RefPixel.R, TrgPixel.G, RefPixel.G, Err));
                                        return Err;
                                    };

                                    float Max = 0.0f;
                                    for (const FCanvasUVTri& Triangle : Triangles)
                                    {
                                        Max = FMath::Max(CheckPixel(Triangle.V0_UV, Triangle.V0_Pos), Max);
                                        Max = FMath::Max(CheckPixel(Triangle.V1_UV, Triangle.V1_Pos), Max);
                                        Max = FMath::Max(CheckPixel(Triangle.V2_UV, Triangle.V2_Pos), Max);
                                    }
                                    RPORT("----------");
                                    RPORT("----------");
                                    VPORT(Max);
                                }
                                */

                                /*
                                if (DrawDebugBases && IsValid(Package.Mesh))
                                {
                                    FlushPersistentDebugLines(GetWorld());

                                    const FSkeletalMeshRenderData* SourceMeshRenderData = Package.Mesh->GetResourceForRendering();
                                    const FSkeletalMeshLODRenderData& SourceMergeLODData = SourceMeshRenderData->LODRenderData[0];
                                    for (const FTGOR_SkeletalMergeVertex& V : MergeData->Vertices)
                                    {
                                        const FVector SourcePosition = SourceMergeLODData.StaticVertexBuffers.PositionVertexBuffer.VertexPosition(V.Index);
                                        const FVector SourceTangent = SourceMergeLODData.StaticVertexBuffers.StaticMeshVertexBuffer.VertexTangentX(V.Index);
                                        const FVector SourceBitangent = SourceMergeLODData.StaticVertexBuffers.StaticMeshVertexBuffer.VertexTangentY(V.Index);
                                        const FVector SourceNormal = SourceMergeLODData.StaticVertexBuffers.StaticMeshVertexBuffer.VertexTangentZ(V.Index);

                                        const FVector VertexTangent = FVector(V.Tangent);
                                        const FVector VertexBitangent = V.Bitangent;
                                        const FVector VertexNormal = (V.Bitangent ^ FVector(V.Tangent)) * V.Tangent.W;

                                        const FVector VertexTarget = FVector(V.Target);

                                        DrawDebugLine(GetWorld(), WorldTransform.TransformPosition(SourcePosition), WorldTransform.TransformPosition(SourcePosition + VertexTangent * 2.0f), FColor::Red, false, 10.0f, 0, 0.025f);
                                        DrawDebugLine(GetWorld(), WorldTransform.TransformPosition(SourcePosition), WorldTransform.TransformPosition(SourcePosition + VertexBitangent * 2.0f), FColor::Green, false, 10.0f, 0, 0.025f);
                                        DrawDebugLine(GetWorld(), WorldTransform.TransformPosition(SourcePosition), WorldTransform.TransformPosition(SourcePosition + VertexNormal * 2.0f), FColor::Blue, false, 10.0f, 0, 0.025f);
                                        DrawDebugLine(GetWorld(), WorldTransform.TransformPosition(SourcePosition), WorldTransform.TransformPosition(SourcePosition + VertexTangent * 4.0f), FColor::White, false, 10.0f, 0, 0.05f);
                                        //DrawDebugLine(GetWorld(), WorldTransform.TransformPosition(Position), WorldTransform.TransformPosition(Position + Bitangent * 5.0f), FColor::Green, false, 10.0f, 0, 0.025f);


                                        
                                        //DrawDebugLine(GetWorld(), WorldTransform.TransformPosition(Position), WorldTransform.TransformPosition(Position + (Tangent * V.Rotate.X + Bitangent * V.Rotate.Y) * 7.5f), FColor::White, false, 10.0f, 0, 0.025f);
                                        
                                        //DrawDebugLine(GetWorld(), WorldTransform.TransformPosition(Position), WorldTransform.TransformPosition(Position + (FQuat(Normal, V.Angle) * Tangent) * 7.5f), FColor::White, false, 10.0f, 0, 0.025f);
                                        //DrawDebugLine(GetWorld(), WorldTransform.TransformPosition(Position), WorldTransform.TransformPosition(Position + (FQuat(Normal, V.Angle) * Bitangent) * 7.5f), FColor::Black, false, 10.0f, 0, 0.025f);
                                    }

                                    if (IsValid(MergeData->ReferenceMesh))
                                    {
                                        const FSkeletalMeshRenderData* TargetMeshRenderData = MergeData->ReferenceMesh->GetResourceForRendering();
                                        const FSkeletalMeshLODRenderData& TargetMergeLODData = TargetMeshRenderData->LODRenderData[0];

                                        const uint32 NumTargetVertices = TargetMergeLODData.StaticVertexBuffers.StaticMeshVertexBuffer.GetNumVertices();
                                        for (uint32 TargetTertexIndex = 0; TargetTertexIndex < NumTargetVertices; TargetTertexIndex++)
                                        {
                                            const FVector Position = TargetMergeLODData.StaticVertexBuffers.PositionVertexBuffer.VertexPosition(TargetTertexIndex);
                                            const FVector Tangent = TargetMergeLODData.StaticVertexBuffers.StaticMeshVertexBuffer.VertexTangentX(TargetTertexIndex);
                                            const FVector Bitangent = TargetMergeLODData.StaticVertexBuffers.StaticMeshVertexBuffer.VertexTangentY(TargetTertexIndex);
                                            const FVector Normal = TargetMergeLODData.StaticVertexBuffers.StaticMeshVertexBuffer.VertexTangentZ(TargetTertexIndex);

                                            DrawDebugLine(GetWorld(), WorldTransform.TransformPosition(Position), WorldTransform.TransformPosition(Position + Tangent * 1.5f), FColor::Black, false, 10.0f, 0, 0.05f);
                                            DrawDebugLine(GetWorld(), WorldTransform.TransformPosition(Position), WorldTransform.TransformPosition(Position + Bitangent * 1.5f), FColor::Magenta, false, 10.0f, 0, 0.05f);
                                            DrawDebugLine(GetWorld(), WorldTransform.TransformPosition(Position), WorldTransform.TransformPosition(Position + Normal * 1.5f), FColor::Cyan, false, 10.0f, 0, 0.05f);
                                        }
                                    }
                                }
                                */


                            }
                        }
                    }
                    else
                    {
                        ChannelTransforms.Emplace(FTransform::Identity);
                    }
                    SectorTransforms.Emplace(ChannelTransforms);
                }

                UVTransforms.UVTransformsPerMesh.Emplace(SectorTransforms);
            }
        }

		FTGOR_SkeletalMeshMerge merge(TargetMesh, Meshes, Mappings, 1, EMeshBufferAccess::Default, UVTransforms);
        if (!merge.DoMerge())
        {
            RPORT("Merge failed");
        }
        else
        {
            Materials.Empty();

            const int32 Num = Sectors.Num();
            for(int32 Index = 0; Index < Num; Index++)
            {
                const FTGOR_MeshMergeSectors& Sector = Sectors[Index];
                UMaterialInstanceDynamic* Material = UMaterialInstanceDynamic::Create(Sector.MaterialInterface, this);

                TargetMesh->Materials[Index].MaterialInterface = Material;
                Materials.Emplace(Material);
            }

            TargeComponent->SetSkeletalMesh(TargetMesh);
            OnMeshMergeDone();
        }
	}
}


/*
// Fill out your copyright notice in the Description page of Project Settings.
#include "MeshMergeFunctionLibrary.h"
#include "SkeletalMeshMerge.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Engine/SkeletalMesh.h"
#include "Animation/Skeleton.h"
static void ToMergeParams(const TArray<FSkelMeshMergeSectionMapping_BP>& InSectionMappings, TArray<FSkelMeshMergeSectionMapping>& OutSectionMappings)
{
    if (InSectionMappings.Num() > 0)
    {
        OutSectionMappings.AddUninitialized(InSectionMappings.Num());
        for (int32 i = 0; i < InSectionMappings.Num(); ++i)
        {
            OutSectionMappings[i].SectionIDs = InSectionMappings[i].SectionIDs;
        }
    }
};
static void ToMergeParams(const TArray<FSkelMeshMergeUVTransformMapping>& InUVTransformsPerMesh, TArray<FSkelMeshMergeUVTransforms>& OutUVTransformsPerMesh)
{
    if (InUVTransformsPerMesh.Num() > 0)
    {
        OutUVTransformsPerMesh.Empty();
        OutUVTransformsPerMesh.AddUninitialized(InUVTransformsPerMesh.Num());
        for (int32 i = 0; i < InUVTransformsPerMesh.Num(); ++i)
        {
            TArray<TArray<FTransform>>& OutUVTransforms = OutUVTransformsPerMesh[i].UVTransformsPerMesh;
            const TArray<FSkelMeshMergeUVTransform>& InUVTransforms = InUVTransformsPerMesh[i].UVTransformsPerMesh;
            if (InUVTransforms.Num() > 0)
            {
                OutUVTransforms.Empty();
                OutUVTransforms.AddUninitialized(InUVTransforms.Num());
                for (int32 j = 0; j < InUVTransforms.Num(); j++)
                {
                    OutUVTransforms[i] = InUVTransforms[i].UVTransforms;
                }
            }
        }
    }
};
USkeletalMesh* UMeshMergeFunctionLibrary::MergeMeshes(const FSkeletalMeshMergeParams& Params)
{
    TArray<USkeletalMesh*> MeshesToMergeCopy = Params.MeshesToMerge;
    MeshesToMergeCopy.RemoveAll([](USkeletalMesh* InMesh)
    {
        return InMesh == nullptr;
    });
    if (MeshesToMergeCopy.Num() <= 1)
    {
        UE_LOG(LogTemp, Warning, TEXT("Must provide multiple valid Skeletal Meshes in order to perform a merge."));
        return nullptr;
    }
    EMeshBufferAccess BufferAccess = Params.bNeedsCpuAccess ?
        EMeshBufferAccess::ForceCPUAndGPU :
        EMeshBufferAccess::Default;
    TArray<FSkelMeshMergeSectionMapping> SectionMappings;
    TArray<FSkelMeshMergeUVTransforms> UvTransforms;
    ToMergeParams(Params.MeshSectionMappings, SectionMappings);
    ToMergeParams(Params.UVTransformsPerMesh, UvTransforms);
    bool bRunDuplicateCheck = false;
    USkeletalMesh* BaseMesh = NewObject<USkeletalMesh>();
    if (Params.Skeleton && Params.bSkeletonBefore)
    {
        BaseMesh->Skeleton = Params.Skeleton;
        bRunDuplicateCheck = true;
        for (USkeletalMeshSocket* Socket : BaseMesh->GetMeshOnlySocketList())
        {
            if (Socket)
            {
                UE_LOG(LogTemp, Warning, TEXT("SkelMeshSocket: %s"), *(Socket->SocketName.ToString()));
            }
        }
        for (USkeletalMeshSocket* Socket : BaseMesh->Skeleton->Sockets)
        {
            if (Socket)
            {
                UE_LOG(LogTemp, Warning, TEXT("SkelSocket: %s"), *(Socket->SocketName.ToString()));
            }
        }
    }
    FSkeletalMeshMerge Merger(BaseMesh, MeshesToMergeCopy, SectionMappings, Params.StripTopLODS, BufferAccess, UvTransforms.GetData());
    if (!Merger.DoMerge())
    {
        UE_LOG(LogTemp, Warning, TEXT("Merge failed!"));
        return nullptr;
    }
    if (Params.Skeleton && !Params.bSkeletonBefore)
    {
        BaseMesh->Skeleton = Params.Skeleton;
    }
    if (bRunDuplicateCheck)
    {
        TArray<FName> SkelMeshSockets;
        TArray<FName> SkelSockets;
        for (USkeletalMeshSocket* Socket : BaseMesh->GetMeshOnlySocketList())
        {
            if (Socket)
            {
                SkelMeshSockets.Add(Socket->GetFName());
                UE_LOG(LogTemp, Warning, TEXT("SkelMeshSocket: %s"), *(Socket->SocketName.ToString()));
            }
        }
        for (USkeletalMeshSocket* Socket : BaseMesh->Skeleton->Sockets)
        {
            if (Socket)
            {
                SkelSockets.Add(Socket->GetFName());
                UE_LOG(LogTemp, Warning, TEXT("SkelSocket: %s"), *(Socket->SocketName.ToString()));
            }
        }
        TSet<FName> UniqueSkelMeshSockets;
        TSet<FName> UniqueSkelSockets;
        UniqueSkelMeshSockets.Append(SkelMeshSockets);
        UniqueSkelSockets.Append(SkelSockets);
        int32 Total = SkelSockets.Num() + SkelMeshSockets.Num();
        int32 UniqueTotal = UniqueSkelMeshSockets.Num() + UniqueSkelSockets.Num();
        UE_LOG(LogTemp, Warning, TEXT("SkelMeshSocketCount: %d | SkelSocketCount: %d | Combined: %d"), SkelMeshSockets.Num(), SkelSockets.Num(), Total);
        UE_LOG(LogTemp, Warning, TEXT("SkelMeshSocketCount: %d | SkelSocketCount: %d | Combined: %d"), UniqueSkelMeshSockets.Num(), UniqueSkelSockets.Num(), UniqueTotal);
        UE_LOG(LogTemp, Warning, TEXT("Found Duplicates: %s"), *((Total != UniqueTotal) ? FString("True") : FString("False")));
    }
    return BaseMesh;
}
*/

