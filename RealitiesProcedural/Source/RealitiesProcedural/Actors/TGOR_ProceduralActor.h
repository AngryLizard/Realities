// The Gateway of Realities: Planes of Existence.

#pragma once

#include "CoreMinimal.h"
#include "RealitiesProcedural/Utility/TGOR_Triangulation.h"

#include "GameFramework/Actor.h"
#include "TGOR_ProceduralActor.generated.h"

//////////////////////////////////////////// DECL /////////////////////////////////////////////////

class UProceduralMeshComponent;

/**
* TGOR_ProceduralActor creates procedural meshes for baking
*/
UCLASS(ClassGroup = (Custom), Blueprintable, meta = (BlueprintSpawnableComponent))
class REALITIESPROCEDURAL_API ATGOR_ProceduralActor : public AActor
{
	GENERATED_BODY()

public:

	////////////////////////////////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////// ENGINE ////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////////////////////////////

	ATGOR_ProceduralActor(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
	virtual void OnConstruction(const FTransform& Transform) override;

	//////////////////////////////////////////// IMPLEMENTABLES ////////////////////////////////////////

	/** Generate a mesh for a given LOD and procedural mesh transform */
	UFUNCTION(BlueprintNativeEvent, CallInEditor, Category = "Procedural Mesh", Meta = (Keywords = "C++"))
		TArray<FTGOR_TriangleMesh> GenerateMesh(const FTransform& Transform, int32 LOD) const;
	virtual TArray<FTGOR_TriangleMesh> GenerateMesh_Implementation(const FTransform& Transform, int32 LOD) const;

	////////////////////////////////////////////// COMPONENTS //////////////////////////////////////////////////////
private:

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Procedural Mesh", meta = (AllowPrivateAccess = "true"))
		UProceduralMeshComponent* ProceduralMesh;

	////////////////////////////////////////////////////////////////////////////////////////////////////
public:

	FORCEINLINE UProceduralMeshComponent* GetMesh() const { return ProceduralMesh; }

	////////////////////////////////////////////////////////////////////////////////////////////////////
public:

	/** Whether to automatically generate mesh on construction */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Procedural Mesh|Collision")
		bool EnableAutoGenerate;

	/** Which LOD to preview */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Procedural Mesh|Generate", meta = (ClampMin = 0, ClampMax = 4))
		int32 PreviewLOD;

	/** Automatically generate LODs on construction */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Procedural Mesh|Generate", meta = (ClampMin = 0, ClampMax = 4))
		int32 MaxLOD;


	/** LOD to use for complex collision channel when using bUseComplexAsSimpleCollision */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Procedural Mesh|Collision", meta = (ClampMin = 0, ClampMax = 4))
		int32 CollisionLOD;

	/** Whether to enable complex collision when generating the mesh */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Procedural Mesh|Collision")
		bool EnableCollision;

	////////////////////////////////////////////////////////////////////////////////////////////////////
public:

	/** Generate procedural mesh, return whether any mesh was generated */
	UFUNCTION(BlueprintCallable, Category = "Procedural Mesh", Meta = (Keywords = "C++"))
		bool Generate(int32 LOD);

	////////////////////////////////////////////////////////////////////////////////////////////////////
protected:

	/** Generate procedural mesh with preview params */
	UFUNCTION(BlueprintCallable, CallInEditor, Category = "Procedural Mesh", Meta = (Keywords = "C++"))
		virtual void Preview();

};
