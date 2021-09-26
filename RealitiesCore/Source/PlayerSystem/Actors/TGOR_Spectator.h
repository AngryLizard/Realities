// TGOR (C) // CHECKED //
#pragma once

#include "CoreMinimal.h"
#include "RealitiesUtility/Structures/TGOR_Time.h"
#include "CreatureSystem/TGOR_CreatureInstance.h"

#include "GameFramework/SpectatorPawn.h"
#include "DimensionSystem/Interfaces/TGOR_DimensionInterface.h"
#include "CoreSystem/Interfaces/TGOR_SingletonInterface.h"
#include "TGOR_Spectator.generated.h"

class UTGOR_IdentityComponent;


USTRUCT(BlueprintType)
struct FTGOR_CreateBodySetup
{
	GENERATED_BODY()
		SERIALISE_INIT_HEADER;
	FTGOR_CreateBodySetup();

	void Send(FTGOR_NetworkWritePackage& Package, UTGOR_Singleton* Context) const;
	void Recv(FTGOR_NetworkReadPackage& Package, UTGOR_Singleton* Context);

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		UTGOR_Creature* Creature;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FTGOR_CreatureSetupInstance Setup;
};

template<>
struct TStructOpsTypeTraits<FTGOR_CreateBodySetup> : public TStructOpsTypeTraitsBase2<FTGOR_CreateBodySetup>
{
	enum
	{
		WithNetSerializer = true
	};
};

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

	////////////////////////////////////////////////////////////////////////////////////////////////////
public:

	FORCEINLINE UTGOR_IdentityComponent* GetIdentity() const { return IdentityComponent; }

	////////////////////////////////////////////////////////////////////////////////////////////////////
public:

	/** Request creation of a new body */
	UFUNCTION(BlueprintCallable, Reliable, Server, WithValidation, Category = "!TGOR Pawn", Meta = (Keywords = "C++"))
		void RequestNewBody(FTGOR_CreateBodySetup Setup);
	virtual void RequestNewBody_Implementation(FTGOR_CreateBodySetup Setup);
	virtual bool RequestNewBody_Validate(FTGOR_CreateBodySetup Setup);

	/////////////////////////////////////////////// INTERNAL ///////////////////////////////////////////

	/** Returns Transform of where requested character ought to spawn. */
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, BlueprintAuthorityOnly, Category = "!TGOR Pawn", Meta = (Keywords = "C++"))
		void GetSpawnTransform(FTransform& Transform) const;

	/** Body request callback. */
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, BlueprintAuthorityOnly, Category = "!TGOR Pawn", Meta = (Keywords = "C++"))
		void OnBodyRequest(APawn* Pawn);

protected:

private:

};
