// TGOR (C) // CHECKED //
#pragma once

#include "DimensionSystem/Actors/TGOR_DimensionActor.h"
#include "TGOR_NpcSpawnActor.generated.h"

///////////////////////////////////////////////// DECL ///////////////////////////////////////////////////

class UTGOR_ModularSkeletalMeshComponent;
class UTGOR_IdentityComponent;

/**
* TGOR_NpcSpawnActor
*/
UCLASS(Abstract, Blueprintable)
class AISYSTEM_API ATGOR_NpcSpawnActor : public ATGOR_DimensionActor
{
	GENERATED_BODY()
	
public:

	////////////////////////////////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////// ENGINE ////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////////////////////////////

	ATGOR_NpcSpawnActor(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	virtual void BeginPlay() override;
	virtual void Tick( float DeltaSeconds ) override;
	virtual void GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const override;

	//////////////////////////////////////////////// IMPLEMENTABLES /////////////////////////////////////////

	////////////////////////////////////////////// COMPONENTS //////////////////////////////////////////////////////
private:

	/** Preview mesh */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Actor, meta = (AllowPrivateAccess = "true"))
		UTGOR_ModularSkeletalMeshComponent* PreviewComponent;

public:

	FORCEINLINE UTGOR_ModularSkeletalMeshComponent* GetPreviewComponent() const { return PreviewComponent; }

	////////////////////////////////////////////////////////////////////////////////////////////////////
protected:

	/** Spawn npc from this spawner and remove myself */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Creature", Meta = (Keywords = "C++"))
		void Spawn();

	////////////////////////////////////////////////////////////////////////////////////////////////////
public:

	////////////////////////////////////////////////////////////////////////////////////////////////////
public:


	/////////////////////////////////////////////// INTERNAL ///////////////////////////////////////////

private:
	
};
