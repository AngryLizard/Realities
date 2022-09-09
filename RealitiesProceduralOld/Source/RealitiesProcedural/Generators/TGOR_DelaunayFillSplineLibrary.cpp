// The Gateway of Realities: Planes of Existence.

#include "TGOR_DelaunayFillSplineLibrary.h"
#include "ProceduralMeshComponent/Public/ProceduralMeshComponent.h"
#include "RealitiesUtility/Structures/TGOR_Matrix3x3.h"
#include "RealitiesUtility/Utility/TGOR_Math.h"


FTGOR_DelaunaySurfaceParams::FTGOR_DelaunaySurfaceParams()
:	UpVector(FVector::UpVector),
	FillerMaxSize(200.0f),
	FillerHeight(100.0f),
	Delaunay(true)
{
}

float FTGOR_DelaunaySurfaceParams::SampleCurve(int32 Index, float Time) const
{
	const FRichCurve* Curve = Curves[Index].GetRichCurveConst();
	if (Curve->Keys.Num() <= 1)
	{
		return Curve->Eval(0.0f);
	}
	const float Start = Curve->GetFirstKey().Time;
	const float End = Curve->GetLastKey().Time;
	return Curve->Eval(Start + Time * (End - Start));
}

float FTGOR_DelaunaySurfaceParams::SampleCurves(float X, float Y) const
{
	const int32 Num = Curves.Num();
	if (Num == 0)
	{
		return 0.0f;
	}
	if (Num == 1)
	{
		const FRichCurve* Curve = Curves[0].GetRichCurveConst();
		return Curve->Eval(Y);
	}

	const float InputKey = FMath::Clamp(X, 0.0f, 1.0f) * (Num - 1);
	const int32 LeftIndex = FMath::FloorToInt(InputKey);
	const float LeftValue = SampleCurve(LeftIndex, Y);

	const int32 RightIndex = FMath::CeilToInt(InputKey);
	const float RightValue = SampleCurve(RightIndex, Y);

	return FMath::Lerp(LeftValue, RightValue, InputKey - (float)LeftIndex);
}

FTGOR_DelaunayMaterialParams::FTGOR_DelaunayMaterialParams()
:	ProjectUV(false)
{
}

FTGOR_DelaunayInstanceParams::FTGOR_DelaunayInstanceParams()
:	InstanceExtend(50.0f, 50.0f, 50.0f),
	PlaceOnMean(true),
	RotateAlongPrincipal(false)
{
	InstanceAlignments.Emplace(FVector(1.0f, 0.0f, 0.0f));

	InstanceRotations.Emplace(FVector2D(0.0f, 0.0f));
	InstanceRotations.Emplace(FVector2D(90.0f, 90.0f));
	InstanceRotations.Emplace(FVector2D(180.0f, 180.0f));
	InstanceRotations.Emplace(FVector2D(270.0f, 270.0f));
}

FVector FTGOR_DelaunayInstanceParams::SampleAlignment(FRandomStream& Random)
{
	if (InstanceAlignments.Num() == 0)
	{
		return FVector::UpVector;
	}
	return InstanceAlignments[Random.RandRange(0, InstanceAlignments.Num() - 1)];
}

float FTGOR_DelaunayInstanceParams::SampleRotation(FRandomStream& Random)
{
	if (InstanceRotations.Num() == 0)
	{
		return 0.0f;
	}
	const FVector2D Range = InstanceRotations[Random.RandRange(0, InstanceRotations.Num() - 1)];
	return Random.FRandRange(FMath::Min(Range.X, Range.Y), FMath::Max(Range.X, Range.Y));
}


FTGOR_DelaunayHoleParams::FTGOR_DelaunayHoleParams()
:	Radius(100.0f)
{
}




void UTGOR_DelaunayFillSplineLibrary::GenerateDelaunay(
	USplineComponent* Left,
	USplineComponent* Right,
	const FTransform& Transform,
	FTGOR_DelaunaySurfaceParams Surface,
	FTGOR_DelaunayMaterialParams Material,
	FTGOR_DelaunayInstanceParams Instances,
	FTGOR_DelaunayHoleParams Holes,

	TArray<FTGOR_TriangleMesh>& Meshes,
	TArray<FTransform>& Transforms)
{
	if (IsValid(Left) && IsValid(Right) && Surface.FillerMaxSize >= SMALL_NUMBER)
	{
		// Create 2D grid to sample for
		TArray<FVector2D> Samples;
		const float LeftLength = Left->GetSplineLength();
		const float RightLength = Right->GetSplineLength();
		const float AverageDistance = UTGOR_ProceduralLibrary::GetAveragePointDistance(Left, Right);
		const FVector2D Bounds = FVector2D((LeftLength + RightLength) / 2, AverageDistance);

		const FVector Project = UTGOR_Math::Normalize(Transform.TransformVectorNoScale(Surface.UpVector));
		const FVector2D Boundary = UTGOR_ProceduralLibrary::ComputeTwinBounds(Left, Right, Project);

		const int32 Segments = FMath::CeilToInt(FMath::Max(LeftLength, RightLength) / Surface.FillerMaxSize);
		for (int32 Segment = 0; Segment <= Segments; Segment++)
		{
			const float SegmentTime = ((float)Segment) / Segments;
			const FVector From = Left->GetLocationAtTime(SegmentTime, ESplineCoordinateSpace::World);
			const FVector To = Right->GetLocationAtTime(SegmentTime, ESplineCoordinateSpace::World);
			const float Distance = (To - From).Size();

			const int32 Cells = FMath::CeilToInt((To - From).Size() / Surface.FillerMaxSize);
			for (int32 Cell = 0; Cell <= Cells; Cell++)
			{
				const float CellTime = ((float)Cell) / Cells;

				Samples.Emplace(FVector2D(SegmentTime, CellTime));
			}
		}

		/*
		TArray<int32> Nums;
		Nums.Append({ 2,3,4,5,5,5,5,4,3,3 });
		const int32 Segments = 9;
		for (int32 Segment = 0; Segment <= Segments; Segment++)
		{
			const float SegmentTime = ((float)Segment) / Segments;

			const int32 Cells = Nums[Segment];
			for (int32 Cell = 0; Cell <= Cells; Cell++)
			{
				const float CellTime = ((float)Cell) / Cells;
				Samples.Emplace(FVector2D(SegmentTime, CellTime));
			}
		}
		*/

		// Create triangulation
		FTGOR_Triangulation2D Triangulation2D;
		Triangulation2D.QHull(Samples, -1);
		Triangulation2D.FixTriangles(-1);

		/*
		Triangulation2D.SetBorders(FVector2D(0.0f, 0.0f), FVector2D(1.0f, 1.0f));

		const int32 SampleTotal = FMath::Min(Samples.Num(), MeshIterations);
		for (int32 SampleIndex = 0; SampleIndex < SampleTotal; SampleIndex++)
		{
			const FVector2D& Sample = Samples[SampleIndex];

			Triangulation2D.AddPoints(Sample);
			Triangulation2D.FixTriangles(DelaunayIterations);
		}
		*/

		// Create vertices
		FTGOR_TriangleMesh TriangleMesh;

		TriangleMesh.Triangulation.Triangles = Triangulation2D.Triangles;
		for (const FVector2D& Point : Triangulation2D.Points)
		{
			const FVector From = Left->GetLocationAtDistanceAlongSpline(Point.X * LeftLength, ESplineCoordinateSpace::World);
			const FVector To = Right->GetLocationAtDistanceAlongSpline(Point.X * RightLength, ESplineCoordinateSpace::World);
			const float Distance = (To - From).Size();

			const FVector FromTangent = Left->GetTangentAtDistanceAlongSpline(Point.X * LeftLength, ESplineCoordinateSpace::World);
			const FVector ToTangent = Right->GetTangentAtDistanceAlongSpline(Point.X * RightLength, ESplineCoordinateSpace::World);
			const FVector Tangent = FMath::Lerp(FromTangent, ToTangent, Point.Y).GetSafeNormal();

			// Set vertex position
			FVector VertexNormal = Project;
			FVector VertexLocation = From;
			if (Distance > SMALL_NUMBER)
			{
				const FVector Normal = (To - From) / Distance;
				const FVector UpVector = (Normal ^ Tangent).GetSafeNormal();
				const FVector Direction = (UpVector ^ Tangent).GetSafeNormal();

				const float CurveValue = Surface.SampleCurves(Point.X, Point.Y);

				const float Height = Surface.FillerHeight * Transform.GetScale3D().GetMax();
				VertexLocation = FMath::Lerp(From, To, Point.Y) + CurveValue * UpVector * Height;
				VertexNormal = UpVector;
			}

			const FVector VertexPoint = Transform.InverseTransformPosition(VertexLocation);
			TriangleMesh.Triangulation.Points.Emplace(VertexPoint);

			FTGOR_TriangleVertex Vertex;
			const FVector VertexProject = Transform.InverseTransformVectorNoScale(Project);
			if (Material.ProjectUV)
			{
				Vertex.UV = Material.Material.Transform(UTGOR_ProceduralLibrary::ProjectUV(VertexPoint, VertexProject, Boundary), Boundary);
			}
			else
			{
				Vertex.UV = Material.Material.Transform(FVector2D(Point.X, Point.Y), Bounds);
			}

			Vertex.Normal = Transform.InverseTransformVectorNoScale(VertexNormal);
			Vertex.Tangent = Transform.InverseTransformVectorNoScale(Tangent);
			Vertex.Color = Material.Material.VertexColor.ToFColor(false);

			TriangleMesh.Vertices.Emplace(Vertex);
		}

		// Delaunay triangulation to improve the surface
		if (Surface.Delaunay)
		{
			TriangleMesh.Triangulation.FixTriangles(-1);
		}
		//else if (DrawDebug)
		//{
		//	TriangleMesh.Triangulation.DrawTriangles(GetWorld(), Transform);
		//}

		// Remove triangles around given hole
		TArray<int32> Changed;
		for (FTGOR_Triangle& Triangle : TriangleMesh.Triangulation.Triangles)
		{
			const FVector A = TriangleMesh.Triangulation.Points[Triangle.Verts[0]];
			const FVector B = TriangleMesh.Triangulation.Points[Triangle.Verts[1]];
			const FVector C = TriangleMesh.Triangulation.Points[Triangle.Verts[2]];
			const FVector Center = (A + B + C) / 3;

			for (const FTransform& Hole : Holes.Transforms)
			{
				const float LA = Hole.InverseTransformPosition(A).GetAbsMax();
				const float LB = Hole.InverseTransformPosition(B).GetAbsMax();
				const float LC = Hole.InverseTransformPosition(C).GetAbsMax();
				const float LR = Hole.InverseTransformPosition(Center).GetAbsMax();

				const float HR = Holes.Radius;
				if (LA < HR || LB < HR || LC < HR || LR < HR)
				{
					Triangle.Enabled = false;
					break;
				}
			}
		}

		// Compute adjacency list from triangulation
		TArray<TArray<int32>> AdjPoints;
		TArray<TArray<FVector>> AdjNormals;
		AdjPoints.SetNum(TriangleMesh.Triangulation.Points.Num());
		AdjNormals.SetNum(TriangleMesh.Triangulation.Points.Num());
		for (int32 Index = 0; Index < TriangleMesh.Triangulation.Triangles.Num(); Index++)
		{
			const FTGOR_Triangle& Triangle = TriangleMesh.Triangulation.Triangles[Index];
			if (Triangle.Enabled)
			{
				const int32 A = Triangle.Verts[0];
				const int32 B = Triangle.Verts[1];
				const int32 C = Triangle.Verts[2];

				AdjPoints[A].AddUnique(B); AdjPoints[B].AddUnique(A);
				AdjPoints[B].AddUnique(C); AdjPoints[C].AddUnique(B);
				AdjPoints[C].AddUnique(A); AdjPoints[A].AddUnique(C);

				const FVector AB = TriangleMesh.Triangulation.Points[B] - TriangleMesh.Triangulation.Points[A];
				const FVector AC = TriangleMesh.Triangulation.Points[C] - TriangleMesh.Triangulation.Points[A];
				const FVector Normal = UTGOR_Math::Normalize(AC ^ AB);

				AdjNormals[A].Add(Normal);
				AdjNormals[Triangle.Verts[1]].AddUnique(Normal);
				AdjNormals[Triangle.Verts[2]].AddUnique(Normal);
			}
		}

		FRandomStream RandomStream;
		for (int32 Index = 0; Index < AdjPoints.Num(); Index++)
		{
			const FVector Point = TriangleMesh.Triangulation.Points[Index];
			const TArray<int32> Points = AdjPoints[Index];
			const TArray<FVector> Normals = AdjNormals[Index];

			const int32 N = Points.Num();
			if (N > 1)
			{
				// Compute normal from adjacent triangles
				FVector Normal = FVector::ZeroVector;
				for (const FVector& Adj : Normals)
				{
					Normal += Adj;
				}
				Normal = UTGOR_Math::Normalize(Normal);
				TriangleMesh.Vertices[Index].Normal = Normal;

				// Compute mean location
				FVector Sum = FVector::ZeroVector;
				for (int32 Adj : Points)
				{
					Sum += (TriangleMesh.Triangulation.Points[Adj] - Point) * 0.5f;
				}
				const FVector Center = Point + (Instances.PlaceOnMean ? (Sum / N) : (((Sum / N) | Normal) * Normal));


				// Get half-way point of all neighbours, compute mean location
				TArray<FVector> Deltas;
				for (int32 Adj : Points)
				{
					const FVector Delta = (TriangleMesh.Triangulation.Points[Adj] - Center) / 2;
					Deltas.Emplace(FVector::VectorPlaneProject(Delta, Normal));
				}

				// Get principal eigenvector along data
				const FVector Tangent = TriangleMesh.Vertices[Index].Tangent;
				const FVector Principal = FTGOR_Matrix3x3(Deltas).PowerMethod(Tangent, 16);
				const FVector Residual = UTGOR_Math::Normalize(Principal ^ Normal);

				FTransform Basis;
				Basis.SetLocation(Center);


				const FVector TileUpVector = UTGOR_Math::Normalize(Instances.SampleAlignment(RandomStream));
				if (TileUpVector.SizeSquared() > SMALL_NUMBER)
				{
					const FQuat Align = FQuat::FindBetweenNormals(FVector::UpVector, TileUpVector);

					const FVector RightVector = Align.RotateVector(FVector::RightVector);

					const float TileAngle = Instances.SampleRotation(RandomStream);
					const FVector TileRightVector = RightVector.RotateAngleAxis(TileAngle, TileUpVector);


					FQuat Quat = FQuat::FindBetweenNormals(TileUpVector, Normal);
					const FVector Axis = UTGOR_Math::Normalize(Quat * TileRightVector);

					if (Instances.RotateAlongPrincipal)
					{
						Quat = FQuat::FindBetweenNormals(Axis, Principal) * Quat;
					}
					else
					{
						const FVector Forward = UTGOR_Math::Normalize((Tangent ^ Normal) ^ Normal);
						Quat = FQuat::FindBetweenNormals(Axis, Forward) * Quat;
					}

					Basis.SetRotation(Quat);
					const FVector TileForwardVector = TileUpVector ^ TileRightVector;

					FVector Extend = FVector::ZeroVector;
					for (const FVector& Delta : Deltas)
					{
						Extend.X = FMath::Max(Extend.X, FMath::Abs(Delta | (Quat * TileForwardVector)));
						Extend.Y = FMath::Max(Extend.Y, FMath::Abs(Delta | (Quat * TileRightVector)));
						Extend.Z = FMath::Max(Extend.Z, FMath::Abs(Delta | (Quat * TileUpVector)));
					}

					const FVector TileScale = TileUpVector * (Surface.FillerMaxSize / Instances.InstanceExtend.Z) + TileForwardVector * (Extend.X / Instances.InstanceExtend.X) + TileRightVector * (Extend.Y / Instances.InstanceExtend.Y);
					Basis.SetScale3D(TileScale.GetAbs());

					Transforms.Emplace(Basis);

					/*
					if (DrawDebug)
					{
						const FVector QuadExtend = FVector(Extend | TileForwardVector, Extend | TileRightVector, Extend | TileUpVector);
						DrawDebugBox(GetWorld(), Transform.TransformPosition(Basis.GetTranslation()), QuadExtend, Transform.TransformRotation(Basis.GetRotation()), FColor::White, true, -1.0f, 0, 1.0f);

						DrawDebugDirectionalArrow(GetWorld(), Transform.TransformPosition(Point), Transform.TransformPosition(Point + Normal * 20.0f), 2.0f, FColor::Red, true, -1.0f, 0, 1.0f);
						DrawDebugDirectionalArrow(GetWorld(), Transform.TransformPosition(Point), Transform.TransformPosition(Point + Principal * 20.0f), 2.0f, FColor::Green, true, -1.0f, 0, 1.0f);
					}
					*/
				}
			}
		}

		TriangleMesh.Material = Material.Material.Material;
		Meshes.Emplace(TriangleMesh);
	}
}

