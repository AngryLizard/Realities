// The Gateway of Realities: Planes of Existence.

#pragma once

#include "CoreMinimal.h"

#include "Realities/Interfaces/TGOR_RegisterInterface.h"
#include "Realities/Components/TGOR_Component.h"
#include "TGOR_EffectComponent.generated.h"

////////////////////////////////////////////// DECL //////////////////////////////////////////////////////

class UTGOR_Effect;

////////////////////////////////////////////// STRUCT //////////////////////////////////////////////////////

USTRUCT(BlueprintType)
struct FTGOR_EffectState
{
	GENERATED_USTRUCT_BODY()
		SERIALISE_INIT_HEADER;

	void Write(FTGOR_GroupWritePackage& Package, UTGOR_Singleton* Context) const;
	void Read(FTGOR_GroupReadPackage& Package, UTGOR_Singleton* Context);
	void Send(FTGOR_NetworkWritePackage& Package, UTGOR_Singleton* Context) const;
	void Recv(FTGOR_NetworkReadPackage& Package, UTGOR_Singleton* Context);

	/** Time this effect will end. */
	UPROPERTY(BlueprintReadWrite, Category = "!TGOR Effects")
		FTGOR_Time Time;

	/** Effect level. */
	UPROPERTY(BlueprintReadWrite, Category = "!TGOR Effects")
		uint8 Level;
};

template<>
struct TStructOpsTypeTraits<FTGOR_EffectState> : public TStructOpsTypeTraitsBase2<FTGOR_EffectState>
{
	enum { WithNetSerializer = true };
};

USTRUCT(BlueprintType)
struct FTGOR_EffectStates
{
	GENERATED_USTRUCT_BODY()
		SERIALISE_INIT_HEADER;

	void Write(FTGOR_GroupWritePackage& Package, UTGOR_Singleton* Context) const;
	void Read(FTGOR_GroupReadPackage& Package, UTGOR_Singleton* Context);
	void Send(FTGOR_NetworkWritePackage& Package, UTGOR_Singleton* Context) const;
	void Recv(FTGOR_NetworkReadPackage& Package, UTGOR_Singleton* Context);

	/** Current stat values. */
	UPROPERTY(BlueprintReadWrite, Category = "!TGOR Effects")
		TMap<UTGOR_Effect*, FTGOR_EffectState> States;
};

template<>
struct TStructOpsTypeTraits<FTGOR_EffectStates> : public TStructOpsTypeTraitsBase2<FTGOR_EffectStates>
{
	enum { WithNetSerializer = true };
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FEffectChangeDelegate);

/**
 * 
 */
UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class REALITIES_API UTGOR_EffectComponent : public UTGOR_Component, public ITGOR_RegisterInterface
{
	GENERATED_BODY()
	
public:
	UTGOR_EffectComponent();

	virtual void BeginPlay() override;

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	void GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const override;

	virtual TSet<UTGOR_Content*> GetActiveContent_Implementation() const override;

	////////////////////////////////////////////////////////////////////////////////////////////////////
protected:

	UPROPERTY(BlueprintAssignable, Category = "!TGOR Effects")
		FEffectChangeDelegate OnEffectChanged;

	////////////////////////////////////////////////////////////////////////////////////////////////////
public:

	/** Called on each client on replication of current setup structure. */
	UFUNCTION()
		void OnReplicateEffects();

	/** Add effect with a given duration */
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "!TGOR Effects", Meta = (Keywords = "C++"))
		void AddEffect(UTGOR_Effect* Effect, int32 Level, float Duration);

	/** Get currently active effects */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Effects", Meta = (Keywords = "C++"))
		TArray<UTGOR_Effect*> GetCurrentEffects() const;

	/** Check whether a given effect is active */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Effects", Meta = (Keywords = "C++"))
		bool IsEffectActive(UTGOR_Effect* Effect) const;

	/** Get time in seconds left for a given effect */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Effects", Meta = (Keywords = "C++"))
		float GetEffectDuration(UTGOR_Effect* Effect) const;

	/** Get level of a given effect */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Effects", Meta = (Keywords = "C++"))
		int32 GetEffectLevel(UTGOR_Effect* Effect) const;

	////////////////////////////////////////////////////////////////////////////////////////////////////
protected:

	/** Currently active effects */
	UPROPERTY(ReplicatedUsing = OnReplicateEffects, BlueprintReadWrite, Category = "!TGOR Effects")
		FTGOR_EffectStates EffectStates;

	////////////////////////////////////////////////////////////////////////////////////////////////////
private:

	/** Send current effect state to attached attribute component */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Effects", Meta = (Keywords = "C++"))
		void UpdateAttributes();

};
