// The Gateway of Realities: Planes of Existence.

#include "TGOR_ProceduralActor.h"
#include "ProceduralMeshComponent/Public/ProceduralMeshComponent.h"
#include "RealitiesProcedural/Generators/TGOR_ProceduralLibrary.h"
#include "RealitiesUtility/Utility/TGOR_Math.h"


ATGOR_ProceduralActor::ATGOR_ProceduralActor(const FObjectInitializer& ObjectInitializer)
:	Super(ObjectInitializer),
	EnableAutoGenerate(true),
	PreviewLOD(0),
	MaxLOD(3),
	CollisionLOD(2),
	EnableCollision(true)
{
	USceneComponent* Root = ObjectInitializer.CreateDefaultSubobject<USceneComponent>(this, FName(TEXT("Root")));
	SetRootComponent(Root);
	Root->SetMobility(EComponentMobility::Static);

	ProceduralMesh = ObjectInitializer.CreateDefaultSubobject<UProceduralMeshComponent>(this, FName(TEXT("ProceduralMesh")));
	ProceduralMesh->SetupAttachment(Root);
}

void ATGOR_ProceduralActor::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

	if (EnableAutoGenerate)
	{
		Generate(PreviewLOD);
	}
}

TArray<FTGOR_TriangleMesh> ATGOR_ProceduralActor::GenerateMesh_Implementation(const FTransform& Transform, int32 LOD) const
{
	return TArray<FTGOR_TriangleMesh>();
}

bool ATGOR_ProceduralActor::Generate(int32 LOD)
{
	FEditorScriptExecutionGuard ScriptGuard;

	const FTransform& Base = ProceduralMesh->GetComponentTransform();
	const TArray<FTGOR_TriangleMesh> Meshes = GenerateMesh(Base, LOD);
	if (Meshes.Num() > 0)
	{
		UTGOR_ProceduralLibrary::ApplyToMeshes(ProceduralMesh, Meshes, EnableCollision);
		return true;
	}
	return false;
}

void ATGOR_ProceduralActor::Preview()
{
	Generate(PreviewLOD);
}
