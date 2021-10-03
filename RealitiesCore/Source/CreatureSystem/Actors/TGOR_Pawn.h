// TGOR (C) // CHECKED //
#pragma once

#include "Components/BoxComponent.h"
#include "MovementSystem/TGOR_MovementInstance.h"

#include "DimensionSystem/Interfaces/TGOR_DimensionInterface.h"
#include "CoreSystem/Interfaces/TGOR_SingletonInterface.h"
#include "GameFramework/Character.h"
#include "TGOR_Pawn.generated.h"

///////////////////////////////////////////////// DECL ///////////////////////////////////////////////////

class UTGOR_IdentityComponent;
class UTGOR_AttributeComponent;

/**
* TGOR_Pawn is the base class for player controlled characters. It defines behaviour for login/logout behaviour.
*/
UCLASS()
class CREATURESYSTEM_API ATGOR_Pawn : public APawn, public ITGOR_SingletonInterface, public ITGOR_DimensionInterface
{
	GENERATED_BODY()

public:

	////////////////////////////////////////////////////////////////////////////////////////////////////

	ATGOR_Pawn(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	//////////////////////////////////////////////// IMPLEMENTABLES /////////////////////////////////////////

	/** Returns true if Pawn should get despawned on player disconnect */
	UFUNCTION(BlueprintNativeEvent, BlueprintPure, Category = "!TGOR Pawn", Meta = (Keywords = "C++"))
		bool DespawnsOnLogout();
		virtual bool DespawnsOnLogout_Implementation();

	/** Called when a controlling player has just switched bodies on server and client. */
	UFUNCTION(BlueprintImplementableEvent, Category = "!TGOR Pawn", Meta = (Keywords = "C++"))
		void OnUnposession();

	////////////////////////////////////////////// COMPONENTS //////////////////////////////////////////////////////
private:

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Character, meta = (AllowPrivateAccess = "true"))
		UTGOR_IdentityComponent* IdentityComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Character, meta = (AllowPrivateAccess = "true"))
		UTGOR_AttributeComponent* AttributeComponent;

	////////////////////////////////////////////////////////////////////////////////////////////////////
public:

	FORCEINLINE UTGOR_IdentityComponent* GetIdentity() const { return IdentityComponent; }

	////////////////////////////////////////////////////////////////////////////////////////////////////
public:

	/** Computes closest portal of a given type */
	UFUNCTION(BlueprintPure, Category = "!TGOR Pawn", Meta = (Keywords = "C++"))
		UTGOR_DimensionReceiverComponent* GetClosestPortal(TSubclassOf<UTGOR_DimensionReceiverComponent> Type) const;

	/////////////////////////////////////////////// INTERNAL ///////////////////////////////////////////

protected:
private:

};