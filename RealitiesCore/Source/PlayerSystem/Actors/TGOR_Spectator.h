// TGOR (C) // CHECKED //
#pragma once

#include "CoreMinimal.h"
#include "RealitiesUtility/Structures/TGOR_Time.h"
#include "CreatureSystem/TGOR_CreatureInstance.h"

#include "CreatureSystem/Actors/TGOR_Pawn.h"
#include "TGOR_Spectator.generated.h"

class UTGOR_PhysicsComponent;
class UTGOR_EquipmentComponent;

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
class PLAYERSYSTEM_API ATGOR_Spectator : public ATGOR_Pawn
{
	GENERATED_BODY()

public:

	////////////////////////////////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////// ENGINE ////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////////////////////////////

	ATGOR_Spectator(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	virtual void GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const override;
	virtual bool Assemble(UTGOR_DimensionData* Dimension) override;
	virtual void UnPossessed() override;
	
	////////////////////////////////////////////////// COMPONENTS //////////////////////////////////////
private:
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Character, Meta = (AllowPrivateAccess = "true"))
		UTGOR_ActionComponent* ActionComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Character, meta = (AllowPrivateAccess = "true"))
		UTGOR_AimComponent* AimComponent;

public:

	FORCEINLINE UTGOR_ActionComponent* GetAction() const { return ActionComponent; }
	FORCEINLINE UTGOR_AimComponent* GetAim() const { return AimComponent; }

public:

	/** Request creation of a new body */
	UFUNCTION(BlueprintCallable, Reliable, Server, WithValidation, Category = "!TGOR Dimension", Meta = (Keywords = "C++"))
		void RequestNewBody(FTGOR_CreateBodySetup Setup);
	virtual void RequestNewBody_Implementation(FTGOR_CreateBodySetup Setup);
	virtual bool RequestNewBody_Validate(FTGOR_CreateBodySetup Setup);

	/////////////////////////////////////////////// INTERNAL ///////////////////////////////////////////

	/** Returns Transform of where requested character ought to spawn. */
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, BlueprintAuthorityOnly, Category = "!TGOR Pawn", Meta = (Keywords = "C++"))
		void GetSpawnTransform(FTransform& Transform) const;

	/** Body request callback. */
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, BlueprintAuthorityOnly, Category = "!TGOR Pawn", Meta = (Keywords = "C++"))
		void OnBodyRequest(ATGOR_Avatar* Avatar);

protected:

private:

};
