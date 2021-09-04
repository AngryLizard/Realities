// The Gateway of Realities: Planes of Existence.

#include "TGOR_ProceduralLibrary.h"
#include "ProceduralMeshComponent/Public/ProceduralMeshComponent.h"
#include "RealitiesUtility/Utility/TGOR_Math.h"
#include "Components/InstancedStaticMeshComponent.h"

FTGOR_ProceduralMaterialParams::FTGOR_ProceduralMaterialParams()
:	NormalisedUV(false),
	TextureSize(1024.0f, 1024.0f),
	TexelDensity(512.0f, 512.0f),
	UVOffset(FVector2D::ZeroVector),
	UVScale(FVector2D(1.0f)),
	Material(nullptr),
	VertexColor(FLinearColor::White)
{
}

FVector2D FTGOR_ProceduralMaterialParams::Transform(const FVector2D& UV, const FVector2D& Bounds) const
{
	if (UVScale.GetAbs().GetMin() < SMALL_NUMBER) return FVector2D();
	if (NormalisedUV)
	{
		return (UV + UVOffset) * UVScale;
	}

	const FVector2D Spread = FVector2D::Max(Bounds, FVector2D(SMALL_NUMBER));
	const FVector2D Ratio = TexelDensity * Spread / FVector2D::Max(TextureSize, FVector2D(SMALL_NUMBER));
	return (UV * 0.01f * Ratio + UVOffset) * UVScale;
}

FVector2D FTGOR_ProceduralMaterialParams::Inverse(const FVector2D& UV, const FVector2D& Bounds) const
{
	if (UVScale.GetAbs().GetMin() < SMALL_NUMBER) return FVector2D();
	if (NormalisedUV)
	{
		return (UV / UVScale) - UVOffset;
	}

	const FVector2D Spread = FVector2D::Max(Bounds, FVector2D(SMALL_NUMBER));
	const FVector2D Ratio = TexelDensity * Spread / FVector2D::Max(TextureSize, FVector2D(SMALL_NUMBER));
	return (UV / UVScale - UVOffset) / Ratio / 0.01f;
}

FTGOR_ProceduralStaticMesh::FTGOR_ProceduralStaticMesh()
:	Weight(1.0f),
	Offset(FVector::ZeroVector),
	StaticMesh(nullptr),
	Axis(ESplineMeshAxis::X)
{
}

FTGOR_ProceduralSplineMesh::FTGOR_ProceduralSplineMesh()
:	MinLength(0.0f),
	MaxLength(0.0f),
	CanBeTail(true),
	CanBeSegment(true),
	CanBeHead(true)
{
}

FTGOR_ProceduralPostMesh::FTGOR_ProceduralPostMesh()
:	MinAngle(0.0f),
	MaxAngle(180.0f)
{
}

FTGOR_ProceduralSplineMeshArray::FTGOR_ProceduralSplineMeshArray()
:	PointAligned(false),
	PostDistances(0.0f),
	Offset(FVector::ZeroVector)
{
}

FTGOR_ProceduralMeshContainer::FTGOR_ProceduralMeshContainer()
{
}


FVector2D UTGOR_ProceduralLibrary::ProjectUV(const FVector& Location, const FVector& Normal, const FVector2D& Bounds)
{
	// Use local vertex location for projection
	FVector XAxis = FVector::ForwardVector;
	FVector YAxis = FVector::RightVector;
	FVector ZAxis = Normal;
	FVector::CreateOrthonormalBasis(XAxis, YAxis, ZAxis);
	const float U = Location | XAxis;
	const float V = Location | YAxis;
	return FVector2D(U, V) / FVector2D::Max(Bounds, FVector2D(SMALL_NUMBER));
}

FVector2D UTGOR_ProceduralLibrary::ComputeTwinBounds(USplineComponent* Left, USplineComponent* Right, const FVector& Normal)
{
	return ProjectUV((Left->Bounds + Right->Bounds).BoxExtent * 2, Normal, FVector2D(1.0f));
}

FVector2D UTGOR_ProceduralLibrary::ComputeBounds(USplineComponent* Spline, const FVector& Normal)
{
	return ProjectUV(Spline->Bounds.BoxExtent * 2, Normal, FVector2D(1.0f));
}


float UTGOR_ProceduralLibrary::GetAveragePointDistance(USplineComponent* Left, USplineComponent* Right)
{
	float AverageDistance = 0.0f;

	const int32 LeftNum = Left->GetNumberOfSplineSegments();
	const int32 RightNum = Right->GetNumberOfSplineSegments();
	const int32 Points = FMath::Min(LeftNum, RightNum);
	for (int32 Point = 0; Point < Points; Point++)
	{
		// Compute position on the spline
		const FVector From = Left->GetLocationAtSplinePoint(Point, ESplineCoordinateSpace::World);
		const FVector To = Right->GetLocationAtSplinePoint(Point, ESplineCoordinateSpace::World);
		AverageDistance += (To - From).Size();
	}
	return AverageDistance / Points;
}


float UTGOR_ProceduralLibrary::GetMaxPointDistance(USplineComponent* Left, USplineComponent* Right)
{
	float MaxDistance = 0.0f;

	const int32 LeftNum = Left->GetNumberOfSplineSegments();
	const int32 RightNum = Right->GetNumberOfSplineSegments();
	const int32 Points = FMath::Min(LeftNum, RightNum);
	for (int32 Point = 0; Point < Points; Point++)
	{
		// Compute position on the spline
		const FVector From = Left->GetLocationAtSplinePoint(Point, ESplineCoordinateSpace::World);
		const FVector To = Right->GetLocationAtSplinePoint(Point, ESplineCoordinateSpace::World);
		MaxDistance = FMath::Max(MaxDistance, (To - From).Size());
	}
	return MaxDistance / Points;
}

float UTGOR_ProceduralLibrary::GetMinPointDistance(USplineComponent* Left, USplineComponent* Right)
{
	float MinDistance = 0.0f;

	const int32 LeftNum = Left->GetNumberOfSplineSegments();
	const int32 RightNum = Right->GetNumberOfSplineSegments();
	const int32 Points = FMath::Min(LeftNum, RightNum);
	for (int32 Point = 0; Point < Points; Point++)
	{
		// Compute position on the spline
		const FVector From = Left->GetLocationAtSplinePoint(Point, ESplineCoordinateSpace::World);
		const FVector To = Right->GetLocationAtSplinePoint(Point, ESplineCoordinateSpace::World);
		MinDistance = FMath::Min(MinDistance, (To - From).Size());
	}
	return MinDistance / Points;
}


FTGOR_TriangleMesh UTGOR_ProceduralLibrary::CombineMesheSection(const FTGOR_TriangleMesh& Mesh, const FTGOR_TriangleMesh& Other)
{
	FTGOR_TriangleMesh Output = Mesh;

	const int32 VertexNum = Mesh.Triangulation.Points.Num();
	const int32 TriangleNum = Mesh.Triangulation.Triangles.Num();
	TArray<FTGOR_Triangle> Triangles = Other.Triangulation.Triangles;
	for (FTGOR_Triangle& Triangle : Triangles)
	{
		Triangle.Verts[0] += VertexNum;
		Triangle.Verts[1] += VertexNum;
		Triangle.Verts[2] += VertexNum;
		Triangle.Adjs[0] += TriangleNum;
		Triangle.Adjs[1] += TriangleNum;
		Triangle.Adjs[2] += TriangleNum;
	}
	Output.Triangulation.Points.Append(Other.Triangulation.Points);
	Output.Triangulation.Triangles.Append(Triangles);
	Output.Vertices.Append(Other.Vertices);
	Output.Convex.Append(Other.Convex);
	return Output;
}

TArray<FTGOR_TriangleMesh> UTGOR_ProceduralLibrary::CombineMeshes(const TArray<FTGOR_TriangleMesh>& Meshes, const TArray<FTGOR_TriangleMesh>& Other)
{
	TArray<FTGOR_TriangleMesh> Output;
	Output.Append(Meshes);
	Output.Append(Other);
	return Output;
}

TArray<FTGOR_TriangleMesh> UTGOR_ProceduralLibrary::MergeMaterials(const TArray<FTGOR_TriangleMesh>& Meshes)
{
	TArray<FTGOR_TriangleMesh> Output;
	TArray<FTGOR_TriangleMesh> Tmp = Meshes;

	int32 Num = Tmp.Num();
	for (int32 Index = 0; Index < Num; Index++)
	{
		// Look ahead for possible merges
		bool Combined = false;
		for (int32 Other = Index + 1; Other < Num; Other++)
		{
			if (Tmp[Index].Material == Tmp[Other].Material)
			{
				Tmp[Other] = CombineMesheSection(Tmp[Index], Tmp[Other]);
				Combined = true;
				break;
			}
		}

		// Only add mesh to list if all merges have been done
		if (!Combined && IsValid(Tmp[Index].Material))
		{
			Output.Emplace(Tmp[Index]);
		}
	}

	return Output;
}

void UTGOR_ProceduralLibrary::PopulateInstancedMeshes(TArray<UInstancedStaticMeshComponent*> Components, const TArray<FTransform>& Transforms)
{
	FRandomStream Random;

	// Randomly assign lists of transforms
	TArray<TArray<FTransform>> Samples;
	const int32 Num = Components.Num();
	Samples.SetNum(Num);

	for (const FTransform& Transform : Transforms)
	{
		const int32 Index = Random.RandRange(0, Num - 1);
		Samples[Index].Emplace(Transform);
	}

	// Assign instances
	for (int32 Index = 0; Index < Num; Index++)
	{
		const int32 Count = Samples[Index].Num();

		UInstancedStaticMeshComponent* Component = Components[Index];
		while (Component->GetNumRenderInstances() > Count)
		{
			Component->RemoveInstance(0);
		}

		while (Component->GetNumRenderInstances() < Count)
		{
			Component->AddInstance(FTransform());
		}

		Component->BatchUpdateInstancesTransforms(0, Samples[Index], false, true, false);
	}
}

void UTGOR_ProceduralLibrary::PopulateInstancedMesh(UInstancedStaticMeshComponent* Component, const TArray<FTransform>& Transforms)
{
	TArray<UInstancedStaticMeshComponent*> Components;
	Components.Emplace(Component);
	PopulateInstancedMeshes(Components, Transforms);
}


void UTGOR_ProceduralLibrary::ApplyToMeshes(UProceduralMeshComponent* ProceduralMesh, const TArray<FTGOR_TriangleMesh>& Meshes, bool EnableCollision)
{
	if (IsValid(ProceduralMesh))
	{
		ProceduralMesh->ClearCollisionConvexMeshes();
		ProceduralMesh->EmptyOverrideMaterials();
		ProceduralMesh->ClearAllMeshSections();

		const int32 MeshNum = Meshes.Num();
		for (int32 Index = 0; Index < MeshNum; Index++)
		{
			const FTGOR_TriangleMesh& Mesh = Meshes[Index];

			bool HasConvex = false;
			for (const FTGOR_ConvexMesh& Convex : Mesh.Convex)
			{
				if (Convex.Points.Num() >= 4)
				{
					ProceduralMesh->AddCollisionConvexMesh(Convex.Points);
					HasConvex = true;
				}
			}

			// Convert into mesh
			TArray<int32> Faces;
			for (const FTGOR_Triangle& Triangle : Mesh.Triangulation.Triangles)
			{
				if (Triangle.Enabled)
				{
					Faces.Append({ Triangle.Verts[0], Triangle.Verts[1], Triangle.Verts[2] });
				}
			}

			TArray<FVector> Normals;
			TArray<FVector2D> UVs;
			TArray<FColor> Colors;
			TArray<FProcMeshTangent> Tangents;
			for (const FTGOR_TriangleVertex& Vertex : Mesh.Vertices)
			{
				Normals.Emplace(Vertex.Normal);
				UVs.Emplace(Vertex.UV);
				Colors.Emplace(Vertex.Color);
				Tangents.Emplace(FProcMeshTangent(Vertex.Tangent, false));
			}

			ProceduralMesh->CreateMeshSection(Index, Mesh.Triangulation.Points, Faces, Normals, UVs, Colors, Tangents, EnableCollision && (ProceduralMesh->bUseComplexAsSimpleCollision || HasConvex));

			ProceduralMesh->SetMaterial(Index, Mesh.Material);
		}
	}
}

int32 FTGOR_ProceduralSplineMeshArray::SamplePostMeshIndex(float Angle, FRandomStream& Random) const
{
	float Weight = 0.0f;

	TArray<int32> Indices;

	const int32 Num = PostMeshes.Num();
	for (int32 Index = 0; Index < Num; Index++)
	{
		const FTGOR_ProceduralPostMesh& PostMesh = PostMeshes[Index];
		if (PostMesh.MinAngle <= Angle && Angle <= PostMesh.MaxAngle)
		{
			Weight += PostMeshes[Index].Weight;
			Indices.Emplace(Index);
		}
	}

	float Sample = Random.RandRange(0.0f, Weight);

	const int32 SampleNum = Indices.Num();
	for (int32 Index = 0; Index < SampleNum; Index++)
	{
		const FTGOR_ProceduralPostMesh& PostMesh = PostMeshes[Indices[Index]];
		Sample -= PostMesh.Weight;

		if (Sample <= 0.0f)
		{
			return Indices[Index];
		}
	}
	return INDEX_NONE;
}

int32 FTGOR_ProceduralSplineMeshArray::SampleSplineIndex(const TArray<int32>& Indices, FRandomStream& Random) const
{
	float Weight = 0.0f;

	const int32 Num = Indices.Num();
	for (int32 Index = 0; Index < Num; Index++)
	{
		const FTGOR_ProceduralSplineMesh& SplineMesh = SplineMeshes[Indices[Index]];
		Weight += SplineMesh.Weight;
	}

	float Sample = Random.RandRange(0.0f, Weight);
	for (int32 Index = 0; Index < Num; Index++)
	{
		const FTGOR_ProceduralSplineMesh& SplineMesh = SplineMeshes[Indices[Index]];
		Sample -= SplineMesh.Weight;

		if (Sample < 0.0f)
		{
			return Index;
		}
	}
	return Num - 1;
}

int32 FTGOR_ProceduralSplineMeshArray::SampleSplineMesh(float Distance, bool IsFirst, FRandomStream& Random) const
{
	TArray<int32> StrongCap; // Is a cap and fits
	TArray<int32> WeakCap; // Is a cap but doesn't fit
	TArray<int32> Match; // Is a segment and fits
	TArray<int32> Tiled; // Is a segment and only a little bit too big
	TArray<int32> Outed; // Is a segment but doesn't fit

	const int32 MeshNum = SplineMeshes.Num();
	for (int32 MeshIndex = 0; MeshIndex < MeshNum; MeshIndex++)
	{
		const FTGOR_ProceduralSplineMesh& SplineMesh = SplineMeshes[MeshIndex];

		if ((SplineMesh.CanBeHead && IsFirst) || (SplineMesh.CanBeTail && Distance < SplineMesh.MaxLength))
		{
			if (SplineMesh.MinLength < Distance)
			{
				StrongCap.Emplace(MeshIndex);
			}
			else
			{
				WeakCap.Emplace(MeshIndex);
			}
		}
		else if (SplineMesh.MinLength < Distance && Distance < SplineMesh.MaxLength && SplineMesh.CanBeSegment)
		{
			Match.Emplace(MeshIndex);
		}
		else if (SplineMesh.MinLength < Distance && Distance * 0.75f < SplineMesh.MaxLength && SplineMesh.CanBeSegment)
		{
			Tiled.Emplace(MeshIndex);
		}
		else if(SplineMesh.CanBeSegment)
		{
			Outed.Emplace(MeshIndex);
		}
	}

	if (StrongCap.Num() > 0)
	{
		return SampleSplineIndex(StrongCap, Random);
	}
	else if (WeakCap.Num() > 0)
	{
		return SampleSplineIndex(WeakCap, Random);
	}
	else if (Match.Num() > 0)
	{
		return SampleSplineIndex(Match, Random);
	}
	else if (Tiled.Num() > 0)
	{
		return SampleSplineIndex(Tiled, Random);
	}
	return SampleSplineIndex(Outed, Random);
}

void UTGOR_ProceduralLibrary::ResetSplineMeshes(FTGOR_ProceduralMeshContainer& MeshContainer)
{
	for (USplineMeshComponent* SplineMesh : MeshContainer.SplineMeshes)
	{
		SplineMesh->DestroyComponent();
	}
	for (USplineMeshComponent* HoleMesh : MeshContainer.HoleMeshes)
	{
		HoleMesh->DestroyComponent();
	}
	for (UStaticMeshComponent* PostMesh : MeshContainer.PostMeshes)
	{
		PostMesh->DestroyComponent();
	}
}

template<typename T>
T* CreateMeshToSplineParent(USplineComponent* Spline, const FTGOR_ProceduralStaticMesh& StaticMesh, TArray<T*>& Container, int32& Count)
{
	T* Mesh = nullptr;
	if (Container.IsValidIndex(Count))
	{
		Mesh = Container[Count];
	}
	else
	{
		USceneComponent* Root = Spline->GetOwner()->GetRootComponent();
		Mesh = NewObject<T>(Root);
		Mesh->SetupAttachment(Root);
		Mesh->RegisterComponent();
		Mesh->SetCullDistance(Spline->CachedMaxDrawDistance);
	}
	Count += 1;

	Mesh->SetStaticMesh(StaticMesh.StaticMesh);

	Mesh->EmptyOverrideMaterials();
	const int32 MaterialNum = StaticMesh.Materials.Num();
	for (int32 MaterialIndex = 0; MaterialIndex < MaterialNum; MaterialIndex++)
	{
		Mesh->SetMaterial(MaterialIndex, StaticMesh.Materials[MaterialIndex]);
	}
	return Mesh;
}

void UpdateSplineMesh(USplineComponent* Spline, USplineMeshComponent* Mesh, float Start, float End, float Ratio, const FVector& Offset, const FTGOR_ProceduralStaticMesh& StaticMesh)
{
	const FVector FinalOffset = Offset + StaticMesh.Offset;
	const float FinalStartDistance = Start - FinalOffset.X;
	const float FinalEndDistance = End + FinalOffset.X;

	if (IsValid(Mesh))
	{
		// Update SplineMesh
		Mesh->SetForwardAxis(StaticMesh.Axis, false);

		Mesh->SetStartAndEnd(
			Spline->GetLocationAtDistanceAlongSpline(FinalStartDistance, ESplineCoordinateSpace::Local),
			Spline->GetTangentAtDistanceAlongSpline(FinalStartDistance, ESplineCoordinateSpace::Local) * Ratio,
			Spline->GetLocationAtDistanceAlongSpline(FinalEndDistance, ESplineCoordinateSpace::Local),
			Spline->GetTangentAtDistanceAlongSpline(FinalEndDistance, ESplineCoordinateSpace::Local) * Ratio, false);

		Mesh->SetStartScale(FVector2D(Spline->GetScaleAtDistanceAlongSpline(FinalStartDistance)), false);
		Mesh->SetEndScale(FVector2D(Spline->GetScaleAtDistanceAlongSpline(FinalEndDistance)), false);

		Mesh->SetStartRoll(FMath::DegreesToRadians(Spline->GetRollAtDistanceAlongSpline(FinalStartDistance, ESplineCoordinateSpace::Local)), false);
		Mesh->SetEndRoll(FMath::DegreesToRadians(Spline->GetRollAtDistanceAlongSpline(FinalEndDistance, ESplineCoordinateSpace::Local)), false);

		Mesh->SetStartOffset(FVector2D(FinalOffset.Y, FinalOffset.Z), false);
		Mesh->SetEndOffset(FVector2D(FinalOffset.Y, FinalOffset.Z), false);

		Mesh->UpdateMesh();
	}
}

void UTGOR_ProceduralLibrary::CreateSplineMeshes(USplineComponent* Spline, const TArray<FTGOR_ProceduralSplineMeshArray>& MeshArrays, int32 Seed, FTGOR_ProceduralMeshContainer& MeshContainer)
{
	int32 SplineMeshCount = 0;
	int32 HoleMeshCount = 0;
	int32 PostMeshCount = 0;
	MeshContainer.Holes.Empty();

	if (IsValid(Spline) && MeshArrays.Num() > 0)
	{
		FRandomStream Random(Seed);

		for (const FTGOR_ProceduralSplineMeshArray& MeshArray : MeshArrays)
		{

			auto CreatePost = [&](float Distance, float Left, float Right) {

				const FTransform Transform = Spline->GetTransformAtDistanceAlongSpline(Distance, ESplineCoordinateSpace::Local);
				const FVector Before = Spline->GetLocationAtDistanceAlongSpline(Left, ESplineCoordinateSpace::Local);
				const FVector After = Spline->GetLocationAtDistanceAlongSpline(Right, ESplineCoordinateSpace::Local);
				const float Angle = FMath::Acos(UTGOR_Math::Normalize(After - Transform.GetLocation()) | UTGOR_Math::Normalize(Transform.GetLocation() - Before)) * 180.0f / PI;

				const int32 PostMeshIndex = MeshArray.SamplePostMeshIndex(Angle, Random);
				if (MeshArray.PostMeshes.IsValidIndex(PostMeshIndex))
				{

					const FTGOR_ProceduralPostMesh& PostMesh = MeshArray.PostMeshes[PostMeshIndex];
					UStaticMeshComponent* Mesh = CreateMeshToSplineParent<UStaticMeshComponent>(Spline, PostMesh, MeshContainer.PostMeshes, PostMeshCount);
					Mesh->SetRelativeTransform(FTransform(FQuat::Identity, MeshArray.Offset + PostMesh.Offset, FVector::OneVector) * Transform);
				}
			};


			int32 SegmentStart = 0;
			int32 SegmentEnd = 0;

			const int32 SegmentNum = Spline->GetNumberOfSplineSegments();
			while (SegmentEnd <= SegmentNum)
			{
				const bool IsLast = (SegmentEnd == SegmentNum);
				const bool IsHole = MeshArray.Holes.Contains(SegmentEnd);
				const bool IsSegmented = IsHole || IsLast;
				if (MeshArray.PointAligned || IsSegmented)
				{
					if (IsHole)
					{
						const float Start = Spline->GetDistanceAlongSplineAtSplinePoint(SegmentEnd);
						const float Stop = Spline->GetDistanceAlongSplineAtSplinePoint(SegmentEnd+1);
						MeshContainer.Holes.Emplace(Spline->GetTransformAtDistanceAlongSpline((Start + Stop) / 2, ESplineCoordinateSpace::World));

						const FTGOR_ProceduralStaticMesh& SplineMesh = MeshArray.Holes[SegmentEnd];
						USplineMeshComponent* Mesh = CreateMeshToSplineParent<USplineMeshComponent>(Spline, SplineMesh, MeshContainer.HoleMeshes, HoleMeshCount);
						UpdateSplineMesh(Spline, Mesh, Start, Stop, 1.0f, MeshArray.Offset, SplineMesh);
					}

					if (SegmentStart != SegmentEnd)
					{
						TArray<float> Poles;

						const float Start = Spline->GetDistanceAlongSplineAtSplinePoint(SegmentStart);
						const float Stop = Spline->GetDistanceAlongSplineAtSplinePoint(SegmentEnd);
						const float Total = Stop - Start;

						TArray<int32> Collection;

						// Sample meshes to fill spline
						float RestLength = Total;
						float TotalGap = 0.0f;
						while (RestLength > 0.0f)
						{
							const int32 MeshIndex = MeshArray.SampleSplineMesh(RestLength, TotalGap < SMALL_NUMBER, Random);

							if (MeshIndex != INDEX_NONE)
							{
								const FTGOR_ProceduralSplineMesh& SplineMesh = MeshArray.SplineMeshes[MeshIndex];

								TotalGap += SplineMesh.MaxLength - SplineMesh.MinLength;
								RestLength -= SplineMesh.MaxLength;

								Collection.Emplace(MeshIndex);
							}
						}

						// Make sure total gap is not 0 so we can divide
						// The result is 0 either way so it doesn't matter
						TotalGap = FMath::Max(TotalGap, SMALL_NUMBER);
						const float RestGap = -RestLength;
						float StartDistance = Start;

						// Add and stretch meshes according to their gapsize
						const int32 CollectionNum = Collection.Num();
						for (int32 Index = 0; Index < CollectionNum; Index++)
						{
							// Compute location on the spline
							const FTGOR_ProceduralSplineMesh& SplineMesh = MeshArray.SplineMeshes[Collection[Index]];
							const float Length = SplineMesh.MaxLength - RestGap * (SplineMesh.MaxLength - SplineMesh.MinLength) / TotalGap;
							const float EndDistance = StartDistance + Length;

							Poles.Emplace(StartDistance);

							USplineMeshComponent* Mesh = CreateMeshToSplineParent<USplineMeshComponent>(Spline, SplineMesh, MeshContainer.SplineMeshes, SplineMeshCount);
							UpdateSplineMesh(Spline, Mesh, StartDistance, EndDistance, Length / Total, MeshArray.Offset, SplineMesh);

							StartDistance = EndDistance;
						}

						// Only add last post if necessary
						if (!MeshArray.PointAligned || IsHole || (IsLast && !Spline->IsClosedLoop()))
						{
							Poles.Emplace(StartDistance);
						}


						// Create poles
						const int32 PoleNum = Poles.Num();
						if (PoleNum > 1)
						{
							const float FirstPole = Poles[0];
							float LastPole = FirstPole;

							if (MeshArray.PostDistances < SMALL_NUMBER)
							{
								for (int32 PoleIndex = 1; PoleIndex < PoleNum - 1; PoleIndex++)
								{
									const float NextPole = Poles[PoleIndex + 1];
									CreatePost(Poles[PoleIndex], LastPole, NextPole);
									LastPole = Poles[PoleIndex];
								}
							}
							else
							{
								float CurrentPole = LastPole + MeshArray.PostDistances;
								while (CurrentPole < Poles.Last())
								{
									const float NextPole = CurrentPole + MeshArray.PostDistances;
									CreatePost(CurrentPole, LastPole, NextPole);
									LastPole = CurrentPole;
									CurrentPole = NextPole;
								}
							}

							if (Spline->IsClosedLoop())
							{
								CreatePost(0.0f, LastPole, Poles[1]);
							}
							else
							{
								CreatePost(FirstPole, FirstPole, Poles[1]);
								CreatePost(Poles.Last(), LastPole, Poles.Last());
							}
						}
					}

					// Go to next segment
					SegmentStart = SegmentEnd;
					if (IsHole)
					{
						// Skip if hole
						SegmentStart += 1;
					}
				}
				SegmentEnd += 1;
			}
		}
	}

	while (MeshContainer.SplineMeshes.IsValidIndex(SplineMeshCount))
	{
		MeshContainer.SplineMeshes.Pop()->DestroyComponent();
	}

	while (MeshContainer.HoleMeshes.IsValidIndex(HoleMeshCount))
	{
		MeshContainer.HoleMeshes.Pop()->DestroyComponent();
	}

	while (MeshContainer.PostMeshes.IsValidIndex(PostMeshCount))
	{
		MeshContainer.PostMeshes.Pop()->DestroyComponent();
	}
}

