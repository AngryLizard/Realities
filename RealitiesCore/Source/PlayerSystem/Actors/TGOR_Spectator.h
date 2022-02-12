// TGOR (C) // CHECKED //
#pragma once

#include "CoreMinimal.h"
#include "RealitiesUtility/Structures/TGOR_Time.h"

#include "GameFramework/SpectatorPawn.h"
#include "DimensionSystem/Interfaces/TGOR_DimensionInterface.h"
#include "CoreSystem/Interfaces/TGOR_SingletonInterface.h"
#include "TGOR_Spectator.generated.h"

class UTGOR_IdentityComponent;
class UTGOR_BodySpawnComponent;

UCLASS()
class PLAYERSYSTEM_API ATGOR_Spectator : public APawn, public ITGOR_SingletonInterface, public ITGOR_DimensionInterface
{
	GENERATED_BODY()

public:

	////////////////////////////////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////// ENGINE ////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////////////////////////////

	ATGOR_Spectator(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	////////////////////////////////////////////// COMPONENTS //////////////////////////////////////////////////////
private:

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Character, meta = (AllowPrivateAccess = "true"))
		UTGOR_IdentityComponent* IdentityComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Character, meta = (AllowPrivateAccess = "true"))
		UTGOR_BodySpawnComponent* SpawnComponent;

	////////////////////////////////////////////////////////////////////////////////////////////////////
public:

	FORCEINLINE UTGOR_IdentityComponent* GetIdentity() const { return IdentityComponent; }
	FORCEINLINE UTGOR_BodySpawnComponent* GetSpawn() const { return SpawnComponent; }

	////////////////////////////////////////////////////////////////////////////////////////////////////
public:

protected:

private:

};
