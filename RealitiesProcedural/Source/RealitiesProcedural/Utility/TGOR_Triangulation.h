// The Gateway of Realities: Planes of Existence. By Salireths.

#pragma once

#include "RealitiesUtility/Utility/TGOR_Error.h"
#include "TGOR_Triangulation.generated.h"

USTRUCT(BlueprintType)
struct REALITIESPROCEDURAL_API FTGOR_Edge
{
	GENERATED_USTRUCT_BODY()
public:
	FTGOR_Edge();
	FTGOR_Edge(int32 T, int32 E);
	FTGOR_Edge(const FTGOR_Edge& Other);

	int32 T;
	int32 E;
};

USTRUCT(BlueprintType)
struct REALITIESPROCEDURAL_API FTGOR_Convex
{
	GENERATED_USTRUCT_BODY()
public:
	FTGOR_Convex();
	FTGOR_Convex(int32 A, int32 B);
	
	TArray<int32> Vertices;
};

USTRUCT(BlueprintType)
struct REALITIESPROCEDURAL_API FTGOR_Triangle
{
	GENERATED_USTRUCT_BODY()
public:
	FTGOR_Triangle();
	FTGOR_Triangle(int32 A, int32 B, int32 C);
	FTGOR_Triangle(const FTGOR_Triangle& Other);

	void ClearAdjs();
	bool HasVertex(int32 Vertex) const;
	void ReplaceAdj(int32 From, int32 To);
	int32 OppositeOf(const FTGOR_Triangle& Other) const;
	bool IsConnected(const FTGOR_Triangle& Other) const;

	int32 Verts[3];
	int32 Adjs[3];
	bool Enabled;
};

USTRUCT(BlueprintType)
struct REALITIESPROCEDURAL_API FTGOR_Triangulation
{
	GENERATED_USTRUCT_BODY()
public:

	UPROPERTY(EditAnywhere, Category = "Procedural Mesh")
		TArray<FTGOR_Triangle> Triangles;

	void Reparent(const TArray<int32>& TriangleIndices);
	void ReparentAll();
};

USTRUCT(BlueprintType)
struct REALITIESPROCEDURAL_API FTGOR_Triangulation3D : public FTGOR_Triangulation
{
	GENERATED_USTRUCT_BODY()
public:

	UPROPERTY(EditAnywhere, Category = "Procedural Mesh")
		TArray<FVector> Points;


	void DrawTriangles(UWorld* World, const FTransform& Transform);
	void Circumcenter(int32 Index, FVector& Center, float& Radius) const;
	void FixTriangles(int32 MaxIterations);
};


USTRUCT(BlueprintType)
struct REALITIESPROCEDURAL_API FTGOR_Triangulation2D : public FTGOR_Triangulation
{
	GENERATED_USTRUCT_BODY()
public:

	UPROPERTY(EditAnywhere, Category = "Procedural Mesh")
		TArray<FVector2D> Points;

	void Circumcenter(int32 Index, FVector2D& Center, float& Radius) const;
	void FixTriangles(int32 MaxIterations);

	FVector ComputeArea(const FTGOR_Triangle& Triangle) const;
	FVector InsideCheck(const FTGOR_Triangle& Triangle, const FVector2D& Point) const;
	int32 FindTriangle(const FVector2D& Point) const;

	int32 CutEdge(int32 TriangleIndex, int32 NeighbourIndex, int32 Edge, int32 PointIndex);
	void SetBorders(const FVector2D& Min, const FVector2D& Max);
	void AddPoints(const FVector2D& Point);

	void QHull(TArray<FVector2D> Cloud, int32 Iterations);
};

USTRUCT(BlueprintType)
struct REALITIESPROCEDURAL_API FTGOR_TriangleVertex
{
	GENERATED_USTRUCT_BODY()
		FTGOR_TriangleVertex();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Procedural Mesh")
		FVector Tangent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Procedural Mesh")
		FVector Normal;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Procedural Mesh")
		FVector2D UV;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Procedural Mesh")
		FColor Color;
};

USTRUCT(BlueprintType)
struct REALITIESPROCEDURAL_API FTGOR_ConvexMesh
{
	GENERATED_USTRUCT_BODY()
		FTGOR_ConvexMesh();
	FTGOR_ConvexMesh(const TArray<FVector>& Points);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Procedural Mesh")
		TArray<FVector> Points;
};

USTRUCT(BlueprintType)
struct REALITIESPROCEDURAL_API FTGOR_TriangleMesh
{
	GENERATED_USTRUCT_BODY()
		FTGOR_TriangleMesh();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Procedural Mesh")
		FTGOR_Triangulation3D Triangulation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Procedural Mesh")
		TArray<FTGOR_TriangleVertex> Vertices;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Procedural Mesh")
		TArray<FTGOR_ConvexMesh> Convex;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Procedural Mesh")
		UMaterialInterface* Material;
};

