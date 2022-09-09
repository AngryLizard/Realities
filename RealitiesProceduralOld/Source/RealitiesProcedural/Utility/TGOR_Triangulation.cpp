// The Gateway of Realities: Planes of Existence. By Salireths.


#include "TGOR_Triangulation.h"
#include "RealitiesUtility/Utility/TGOR_Math.h"
#include "DrawDebugHelpers.h"


FTGOR_Edge::FTGOR_Edge()
: T(INDEX_NONE), E(INDEX_NONE)
{
}

FTGOR_Edge::FTGOR_Edge(int32 T, int32 E)
	: T(T), E(E)
{

}

FTGOR_Edge::FTGOR_Edge(const FTGOR_Edge& Other)
	: T(Other.T), E(Other.E)
{
}


FTGOR_Convex::FTGOR_Convex()
{
}

FTGOR_Convex::FTGOR_Convex(int32 A, int32 B)
{
	Vertices.Append({A, B});
}


FTGOR_Triangle::FTGOR_Triangle()
	: Enabled(true)
{
	Verts[0] = INDEX_NONE;
	Verts[1] = INDEX_NONE;
	Verts[2] = INDEX_NONE;
	Adjs[0] = INDEX_NONE;
	Adjs[1] = INDEX_NONE;
	Adjs[2] = INDEX_NONE;
}

FTGOR_Triangle::FTGOR_Triangle(int32 A, int32 B, int32 C)
	: Enabled(true)
{
	Verts[0] = A;
	Verts[1] = B;
	Verts[2] = C;
	Adjs[0] = INDEX_NONE;
	Adjs[1] = INDEX_NONE;
	Adjs[2] = INDEX_NONE;
}

FTGOR_Triangle::FTGOR_Triangle(const FTGOR_Triangle& Other)
	: Enabled(true)
{
	Verts[0] = Other.Verts[0];
	Verts[1] = Other.Verts[1];
	Verts[2] = Other.Verts[2];
	Adjs[0] = Other.Adjs[0];
	Adjs[1] = Other.Adjs[1];
	Adjs[2] = Other.Adjs[2];
}

void FTGOR_Triangle::ClearAdjs()
{
	Adjs[0] = INDEX_NONE;
	Adjs[1] = INDEX_NONE;
	Adjs[2] = INDEX_NONE;
}

bool FTGOR_Triangle::HasVertex(int32 Vertex) const
{
	return Verts[0] == Vertex || Verts[1] == Vertex || Verts[2] == Vertex;
}

void FTGOR_Triangle::ReplaceAdj(int32 From, int32 To)
{
	for (int32& Adj : Adjs)
	{
		if (Adj == From)
		{
			Adj = To;
			return;
		}
	}
}

int32 FTGOR_Triangle::OppositeOf(const FTGOR_Triangle& Other) const
{
	for (int32 VertIndex = 0; VertIndex < 3; VertIndex++)
	{
		const int32 Vert = Verts[VertIndex];
		if (!Other.HasVertex(Vert))
		{
			return VertIndex;
		}
	}
	return INDEX_NONE;
}

bool FTGOR_Triangle::IsConnected(const FTGOR_Triangle& Other) const
{
	int32 Count = 0;
	for (int32 VertIndex = 0; VertIndex < 3; VertIndex++)
	{
		const int32 Vert = Verts[VertIndex];
		if (Other.HasVertex(Vert))
		{
			Count++;
		}
	}
	return Count == 2;
}



void FTGOR_Triangulation::Reparent(const TArray<int32>& TriangleIndices)
{
	// Gather all parents and reset input triangles
	TArray<int32> Neighbours;
	for (int32 TriangleIndex : TriangleIndices)
	{
		FTGOR_Triangle& Triangle = Triangles[TriangleIndex];

		if (Triangles.IsValidIndex(Triangle.Adjs[0])) Neighbours.AddUnique(Triangle.Adjs[0]);
		if (Triangles.IsValidIndex(Triangle.Adjs[1])) Neighbours.AddUnique(Triangle.Adjs[1]);
		if (Triangles.IsValidIndex(Triangle.Adjs[2])) Neighbours.AddUnique(Triangle.Adjs[2]);
		Neighbours.AddUnique(TriangleIndex);
		Triangle.ClearAdjs();
	}

	for (int32 MineIndex : Neighbours)
	{
		FTGOR_Triangle& Mine = Triangles[MineIndex];
		for (int32 YourIndex : Neighbours)
		{
			FTGOR_Triangle& Your = Triangles[YourIndex];
			if (Your.IsConnected(Mine))
			{
				Your.Adjs[Your.OppositeOf(Mine)] = MineIndex;
				Mine.Adjs[Mine.OppositeOf(Your)] = YourIndex;
			}
		}
	}
}





void FTGOR_Triangulation3D::DrawTriangles(UWorld* World, const FTransform& Transform)
{
	for (const FTGOR_Triangle& Mine : Triangles)
	{
		FVector Center = FVector::ZeroVector;
		for (int32 Vert : Mine.Verts)
		{
			Center += Points[Vert] / 3;
		}

		for (int32 Edge = 0; Edge < 3; Edge++)
		{
			int32 Adj = Mine.Adjs[Edge];
			if (Triangles.IsValidIndex(Adj))
			{
				const FTGOR_Triangle& Your = Triangles[Adj];

				FVector Target = FVector::ZeroVector;
				for (int32 Vert : Your.Verts)
				{
					Target += Points[Vert] / 3;
				}

				FColor Color = (Edge == 0 ? (FColor::Green) : (Edge == 1 ? (FColor::Blue) : (FColor::Red)));

				const int32 MineOpp = Mine.OppositeOf(Your);
				if (MineOpp != INDEX_NONE)
				{
					const FVector From = Points[Mine.Verts[MineOpp]];
					DrawDebugDirectionalArrow(World, Transform.TransformPosition(From), Transform.TransformPosition(Target), 5.0f, Color, true, -1, 0, 0.0f);
				}
				else
				{
					DrawDebugDirectionalArrow(World, Transform.TransformPosition(Center), Transform.TransformPosition(Target), 5.0f, Color, true, -1, 0, 2.0f);
				}
			}
		}
	}
}

void FTGOR_Triangulation3D::Circumcenter(int32 Index, FVector& Center, float& Radius) const
{
	const FTGOR_Triangle& Triangle = Triangles[Index];

	const FVector A = Points[Triangle.Verts[0]];
	const FVector B = Points[Triangle.Verts[1]];
	const FVector C = Points[Triangle.Verts[2]];
	if (UTGOR_Math::ComputeCircumcenter(A, B, C, Center))
	{
		Radius = (Center - A).SizeSquared();
		return;
	}
	Radius = 0.0f;
}

/*
void FTGOR_Triangulation3D::FixTriangles(const int32 MaxIterations)
{
	const int32 Total = Triangles.Num();

	TArray<FVector> Centers;
	Centers.SetNum(Total);

	TArray<float> Radius;
	Radius.SetNum(Total);

	// Cache triange circumcircle
	for (int32 Index = 0; Index < Total; Index++)
	{
		Circumcenter(Index, Centers[Index], Radius[Index]);
	}

	// Flip edges
	const int32 Iterations = (MaxIterations < 0) ? Total : MaxIterations;
	for (int32 Iteration = 0; Iteration < Iterations; Iteration++)
	{
		bool Changed = false;
		for (int32 Index = 0; Index < Total; Index++)
		{
			FTGOR_Triangle& Mine = Triangles[Index];

			const FVector& Circ = Centers[Index];
			const float RR = Radius[Index];

			for (int32 MineEdge = 0; MineEdge < 3; MineEdge++)
			{
				const int32 Adj = Mine.Adjs[MineEdge];
				if (Triangles.IsValidIndex(Adj))
				{
					FTGOR_Triangle& Your = Triangles[Adj];

					const int32 YourEdge = Your.OppositeOf(Mine);
					const FVector D = Points[Your.Verts[YourEdge]];
					if ((Circ - D).SizeSquared() < RR - SMALL_NUMBER)
					{
						const int32 MineNext = (MineEdge + 1) % 3;
						Mine.Verts[MineNext] = Your.Verts[YourEdge];

						const int32 YourNext = (YourEdge + 1) % 3;
						Your.Verts[YourNext] = Mine.Verts[MineEdge];


						const int32 MinePrev = (MineEdge + 2) % 3;
						Your.Adjs[YourEdge] = Mine.Adjs[MinePrev];
						Mine.Adjs[MinePrev] = Adj;

						const int32 YourPrev = (YourEdge + 2) % 3;
						Mine.Adjs[MineEdge] = Your.Adjs[YourPrev];
						Your.Adjs[YourPrev] = Index;

						if (Triangles.IsValidIndex(Mine.Adjs[MineEdge]))
						{
							const int32 Opp = Triangles[Mine.Adjs[MineEdge]].OppositeOf(Mine);
							if (Opp == INDEX_NONE)
							{
								RPORT("Error");
								return;
							}

							Triangles[Mine.Adjs[MineEdge]].Adjs[Opp] = Index;
						}

						if (Triangles.IsValidIndex(Your.Adjs[YourEdge]))
						{
							const int32 Opp = Triangles[Your.Adjs[YourEdge]].OppositeOf(Your);
							if (Opp == INDEX_NONE)
							{
								RPORT("Error");
								return;
							}

							Triangles[Your.Adjs[YourEdge]].Adjs[Opp] = Adj;
						}

						Circumcenter(Index, Centers[Index], Radius[Index]);
						Circumcenter(Adj, Centers[Adj], Radius[Adj]);
						Changed = true;
						break;
					}
				}
			}
		}

		// No need to keep going if nothing changed
		if (!Changed)
		{
			break;
		}
	}
}
*/

void FTGOR_Triangulation3D::FixTriangles(int32 MaxIterations)
{
	const int32 Total = Triangles.Num();
	const int32 Iterations = (MaxIterations < 0) ? Total : MaxIterations;
	for (int32 Iteration = 0; Iteration < Iterations; Iteration++)
	{
		bool Changed = false;
		for (int32 Index = 0; Index < Total; Index++)
		{
			FTGOR_Triangle& Mine = Triangles[Index];

			const FVector A = Points[Mine.Verts[0]];
			const FVector B = Points[Mine.Verts[1]];
			const FVector C = Points[Mine.Verts[2]];
			FVector Circ;
			UTGOR_Math::ComputeCircumcenter(A, B, C, Circ);
			const float RR = (Circ - A).SizeSquared();

			for (int32 Adj : Mine.Adjs)
			{
				if (Triangles.IsValidIndex(Adj))
				{
					FTGOR_Triangle& Your = Triangles[Adj];

					const int32 MineOpps = Mine.OppositeOf(Your);
					const int32 YourOpps = Your.OppositeOf(Mine);
					if (MineOpps != INDEX_NONE && YourOpps != INDEX_NONE)
					{
						const FVector D = Points[Your.Verts[YourOpps]];
						if ((Circ - D).SizeSquared() < RR)
						{
							const int32 MineNext = (MineOpps + 1) % 3;
							Mine.Verts[MineNext] = Your.Verts[YourOpps];

							const int32 YourNext = (YourOpps + 1) % 3;
							Your.Verts[YourNext] = Mine.Verts[MineOpps];

							Reparent({ Index, Adj });
							Changed = true;
							break;
						}
					}
				}
			}
		}

		// No need to keep going if nothing changed
		if (!Changed) break;
	}
}


FVector FTGOR_Triangulation2D::ComputeArea(const FTGOR_Triangle& Triangle) const
{
	const FVector2D& A = Points[Triangle.Verts[0]];
	const FVector2D& B = Points[Triangle.Verts[1]];
	const FVector2D& C = Points[Triangle.Verts[2]];

	FVector Out;
	Out[2] = ((B - A) ^ (C - A));
	Out[0] = ((C - B) ^ (A - B));
	Out[1] = ((A - C) ^ (B - C));
	return	Out;
}

FVector FTGOR_Triangulation2D::InsideCheck(const FTGOR_Triangle& Triangle, const FVector2D& Point) const
{
	const FVector2D& A = Points[Triangle.Verts[0]];
	const FVector2D& B = Points[Triangle.Verts[1]];
	const FVector2D& C = Points[Triangle.Verts[2]];

	FVector Out;
	Out[2] = ((B - A) ^ (Point - A));
	Out[0] = ((C - B) ^ (Point - B));
	Out[1] = ((A - C) ^ (Point - C));
	return	Out;
}

void FTGOR_Triangulation2D::Circumcenter(int32 Index, FVector2D& Center, float& Radius) const
{
	const FTGOR_Triangle& Triangle = Triangles[Index];

	const FVector2D A = Points[Triangle.Verts[0]];
	const FVector2D B = Points[Triangle.Verts[1]];
	const FVector2D C = Points[Triangle.Verts[2]];
	if (UTGOR_Math::ComputeCircumcenter2D(A, B, C, Center))
	{
		Radius = (Center - A).SizeSquared();
		return;
	}
	Radius = 0.0f;
}


void FTGOR_Triangulation2D::FixTriangles(int32 MaxIterations)
{
	const int32 Total = Triangles.Num();

	TArray<FVector2D> Centers;
	Centers.SetNum(Total);

	TArray<float> Radius;
	Radius.SetNum(Total);

	// Cache triange circumcircle
	for (int32 Index = 0; Index < Total; Index++)
	{
		Circumcenter(Index, Centers[Index], Radius[Index]);
	}

	// Flip edges
	for (int32 Iteration = 0; Iteration < Total; Iteration++)
	{
		bool Changed = false;
		for (int32 Index = 0; Index < Total; Index++)
		{
			if (MaxIterations-- == 0) return;

			FTGOR_Triangle& Mine = Triangles[Index];

			const FVector2D& Circ = Centers[Index];
			const float RR = Radius[Index];

			for (int32 MineEdge = 0; MineEdge < 3; MineEdge++)
			{
				const int32 Adj = Mine.Adjs[MineEdge];
				if (Triangles.IsValidIndex(Adj))
				{
					FTGOR_Triangle& Your = Triangles[Adj];

					// Check whether inside circumcircle
					const int32 YourEdge = Your.OppositeOf(Mine);
					const FVector2D D = Points[Your.Verts[YourEdge]];
					const float CC = (Circ - D).SizeSquared();
					if (CC < RR - SMALL_NUMBER)
					{
						const int32 MineNext = (MineEdge + 1) % 3;
						Mine.Verts[MineNext] = Your.Verts[YourEdge];

						const int32 YourNext = (YourEdge + 1) % 3;
						Your.Verts[YourNext] = Mine.Verts[MineEdge];

						const int32 MinePrev = (MineEdge + 2) % 3;
						Your.Adjs[YourEdge] = Mine.Adjs[MinePrev];
						Mine.Adjs[MinePrev] = Adj;

						const int32 YourPrev = (YourEdge + 2) % 3;
						Mine.Adjs[MineEdge] = Your.Adjs[YourPrev];
						Your.Adjs[YourPrev] = Index;

						if (Triangles.IsValidIndex(Mine.Adjs[MineEdge]))
						{
							const int32 Opp = Triangles[Mine.Adjs[MineEdge]].OppositeOf(Mine);
							if (Opp == INDEX_NONE)
							{
								RPORT("Error");
								return;
							}

							Triangles[Mine.Adjs[MineEdge]].Adjs[Opp] = Index;
						}

						if (Triangles.IsValidIndex(Your.Adjs[YourEdge]))
						{
							const int32 Opp = Triangles[Your.Adjs[YourEdge]].OppositeOf(Your);
							if (Opp == INDEX_NONE)
							{
								RPORT("Error");
								return;
							}

							Triangles[Your.Adjs[YourEdge]].Adjs[Opp] = Adj;
						}

						Circumcenter(Index, Centers[Index], Radius[Index]);
						Circumcenter(Adj, Centers[Adj], Radius[Adj]);
						Changed = true;
						break;
					}
				}
			}
		}

		// No need to keep going if nothing changed
		if (!Changed)
		{
			break;
		}
	}
}

int32 FTGOR_Triangulation2D::CutEdge(int32 TriangleIndex, int32 NeighbourIndex, int32 Edge, int32 PointIndex)
{
	const int32 NextIndex = (Edge + 1) % 3;
	const int32 PrevIndex = (Edge + 2) % 3;

	FTGOR_Triangle& Triangle = Triangles[TriangleIndex];

	FTGOR_Triangle New(Triangle);
	New.Verts[NextIndex] = PointIndex;
	Triangle.Verts[PrevIndex] = PointIndex;

	return Triangles.Emplace(New);
}

int32 FTGOR_Triangulation2D::FindTriangle(const FVector2D& Point) const
{
	const int32 Num = Triangles.Num();
	for (int32 Index = 0; Index < Num; Index++)
	{
		const FTGOR_Triangle& Center = Triangles[Index];
		const FVector Area = ComputeArea(Center);
		if (Area.SizeSquared() < SMALL_NUMBER)
		{
			// Error handling?
		}
		else
		{
			const FVector Inside = InsideCheck(Center, Point);
			const FVector Check = Inside * Area;
			if (Check.GetMin() > -SMALL_NUMBER)
			{
				return Index;
			}
		}
	}
	return INDEX_NONE;
}


void FTGOR_Triangulation2D::SetBorders(const FVector2D& Min, const FVector2D& Max)
{
	Points.Append({ FVector2D(Min.X, Min.Y) , FVector2D(Max.X, Min.Y) , FVector2D(Min.X, Max.Y) , FVector2D(Max.X, Max.Y) });
	FTGOR_Triangle Left(1, 0, 2);
	Left.Adjs[1] = 1;

	FTGOR_Triangle Right(3, 1, 2);
	Right.Adjs[0] = 0;

	Triangles.Append({ Left, Right });
}

void FTGOR_Triangulation2D::AddPoints(const FVector2D& Point)
{
	const int32 CenterIndex = FindTriangle(Point);
	if (Triangles.IsValidIndex(CenterIndex))
	{
		FTGOR_Triangle& Center = Triangles[CenterIndex];
		const FVector Area = ComputeArea(Center);
		const FVector Inside = InsideCheck(Center, Point);
		const FVector Check = Inside * Area;

		// Don't add exact match
		if ((Check.X < SMALL_NUMBER ? 1 : 0) + (Check.Y < SMALL_NUMBER ? 1 : 0) + (Check.Z < SMALL_NUMBER ? 1 : 0) >= 2)
		{
			return;
		}

		// Point is definitely added
		const int32 PointIndex = Points.Emplace(Point);

		// Special behaviour for edge hit
		for (int32 Edge = 0; Edge < 3; Edge++)
		{
			if (Check[Edge] < SMALL_NUMBER)
			{
				TArray<int32> TriangleIndices;

				// Cut neighbour if available
				const int32 OppIndex = Center.Adjs[Edge];
				if (Triangles.IsValidIndex(OppIndex))
				{
					const FTGOR_Triangle& Opp = Triangles[OppIndex];
					const int32 OppEdge = Opp.OppositeOf(Center);
					const int32 OppNew = CutEdge(OppIndex, CenterIndex, OppEdge, PointIndex);
					TriangleIndices.Append({ OppIndex, OppNew });
				}

				const int32 New = CutEdge(CenterIndex, OppIndex, Edge, PointIndex);
				TriangleIndices.Append({ CenterIndex, New });

				Reparent(TriangleIndices);
				return;
			}
		}

		// Hook up vertices and ajdacency lists
		FTGOR_Triangle Left(Center);
		FTGOR_Triangle Right(Center);

		Center.Verts[0] = PointIndex;
		Right.Verts[1] = PointIndex;
		Left.Verts[2] = PointIndex;

		const int32 RightIndex = Triangles.Emplace(Right);
		const int32 LeftIndex = Triangles.Emplace(Left);

		Reparent({ CenterIndex, LeftIndex, RightIndex });
		return;
	}
}


void FTGOR_Triangulation2D::QHull(TArray<FVector2D> Cloud, int32 Iterations)
{
	Points.Empty();
	Triangles.Empty();
	if (Cloud.Num() > 2)
	{
		FVector2D Mean = 0;
		for (const FVector2D& Point : Cloud)
		{
			Mean += Point;
		}
		Mean /= Cloud.Num();

		// Get closest point to mean
		Cloud.Sort([Mean](const FVector2D& A, const FVector2D& B) -> bool { return (A - Mean).SizeSquared() > (B - Mean).SizeSquared(); });
		const FVector2D P0 = Cloud.Pop();

		// Get next closest point
		Cloud.Sort([P0](const FVector2D& A, const FVector2D& B) -> bool { return (A - P0).SizeSquared() > (B - P0).SizeSquared(); });
		const FVector2D P1 = Cloud.Pop();

		// Find smallest circumcircle
		float ClosestDQ = FLT_MAX;
		int32 Closest = Cloud.Num() - 1;
		for (int32 Index = Closest; Index >= 0; Index--)
		{
			const FVector2D& P2 = Cloud[Index];
			if (ClosestDQ < (P0 - P2).SizeSquared())
			{
				break;
			}
			
			FVector2D Circ;
			if (UTGOR_Math::ComputeCircumcenter2D(P0, P1, P2, Circ))
			{
				const float CQ = (Circ - P2).SizeSquared();
				if (CQ < ClosestDQ)
				{
					ClosestDQ = CQ;
					Closest = Index;
				}
			}
		}

		if (!Cloud.IsValidIndex(Closest))
		{
			return;
		}

		// Create Triangle
		const FVector2D P2 = Cloud[Closest];
		Cloud.RemoveAt(Closest);

		if (((P2 - P0) ^ (P1 - P0)) < 0.0f)
		{
			Points.Append({ P1, P0, P2 });
		}
		else
		{
			Points.Append({ P2, P0, P1 });
		}
		const int32 CT = Triangles.Emplace(FTGOR_Triangle(0, 1, 2));

		// Create convex hull
		TArray<FTGOR_Edge> Convex;
		Convex.Emplace(FTGOR_Edge(CT, 0));
		Convex.Emplace(FTGOR_Edge(CT, 1));
		Convex.Emplace(FTGOR_Edge(CT, 2));

		// Sort points away from circumcircle
		FVector2D Circ;
		UTGOR_Math::ComputeCircumcenter2D(P0, P1, P2, Circ);
		Cloud.Sort([Circ](const FVector2D& A, const FVector2D& B) -> bool { return (A - Circ).SizeSquared() > (B - Circ).SizeSquared(); });

		while (Cloud.Num() > 0 && Iterations-- != 0) //for (int32 N = 0; N < 2; N++) //
		{
			const FVector2D Point = Cloud.Pop();
			const int32 PointIndex = Points.Emplace(Point);

			// Find range of visible edges
			bool IsVisible = false;
			int32 Start = 0, Count = 0;
			const int32 Num = Convex.Num();
			for (int32 Index = 0; Index < Num; Index++)
			{
				// Get edge vector
				const FTGOR_Edge& Edge = Convex[Index];
				const int32 Prev = Triangles[Edge.T].Verts[(Edge.E + 1) % 3];
				const int32 Next = Triangles[Edge.T].Verts[(Edge.E + 2) % 3];

				// see whether convex hull is visible to Point
				const float Cross = (Points[Next] - Points[Prev]) ^ (Point - Points[Prev]);
				if (Cross < 0.000001f)
				{
					IsVisible = false;
				}
				else
				{
					if (!IsVisible)
					{
						Start = Index;
						IsVisible = true;
					}
					Count++;
				}
			}

			// Construct triangles
			TArray<FTGOR_Edge> NewConvex;
			for (int32 Index = 0; Index < Num; Index++)
			{
				const FTGOR_Edge& Edge = Convex[(Start + Index) % Num];
				if (Index < Count)
				{
					// Chain new triangles
					const int32 Prev = Triangles[Edge.T].Verts[(Edge.E + 1) % 3];
					const int32 Next = Triangles[Edge.T].Verts[(Edge.E + 2) % 3];

					FTGOR_Triangle Triangle(Prev, PointIndex, Next);
					Triangle.Adjs[0] = INDEX_NONE;
					Triangle.Adjs[1] = Edge.T;
					Triangle.Adjs[2] = INDEX_NONE;

					const int32 TriangleIndex = Triangles.Emplace(Triangle);
					Triangles[Edge.T].Adjs[Edge.E] = TriangleIndex;

					if (Index == 0)
					{
						NewConvex.Emplace(FTGOR_Edge(TriangleIndex, 2));
					}
					else
					{
						Triangles[TriangleIndex].Adjs[2] = TriangleIndex - 1;
					}

					if (Index == Count-1)
					{
						NewConvex.Emplace(FTGOR_Edge(TriangleIndex, 0));
					}
					else
					{
						Triangles[TriangleIndex].Adjs[0] = TriangleIndex + 1;
					}
				}
				else
				{
					// Reapply old convex hull
					NewConvex.Emplace(Edge);
				}
			}

			Convex = NewConvex;
		}
	}
}


FTGOR_TriangleVertex::FTGOR_TriangleVertex()
	: Tangent(FVector::ZeroVector),
	Normal(FVector::UpVector),
	UV(FVector2D::ZeroVector),
	Color(FColor::Black)
{

}

FTGOR_ConvexMesh::FTGOR_ConvexMesh()
{
}

FTGOR_ConvexMesh::FTGOR_ConvexMesh(const TArray<FVector>& Points)
	: Points(Points)
{
}

FTGOR_TriangleMesh::FTGOR_TriangleMesh()
	: Material(nullptr)
{
}
