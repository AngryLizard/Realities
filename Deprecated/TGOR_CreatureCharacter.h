// TGOR (C) // CHECKED //
#pragma once

#include "TGOR_MovementCharacter.h"
#include "TGOR_CreatureCharacter.generated.h"

///////////////////////////////////////////////// DECL ///////////////////////////////////////////////////

class UTGOR_SaveComponent;
class UTGOR_InteractableComponent;
class UTGOR_CharacterMeshComponent;

/**
* TGOR_CreatureCharacter is the top level class used for player controlled creatures and inherits from parent classes
* that build up functionality layers for the different aspects of the character(movement, combat, etc.)
*/
UCLASS(Blueprintable)
class REALITIES_API ATGOR_CreatureCharacter : public ATGOR_MovementCharacter
{
	GENERATED_BODY()

	public:
		////////////////////////////////////////////////////////////////////////////////////////////////////
		//////////////////////////////////////////////// ENGINE ////////////////////////////////////////////
		////////////////////////////////////////////////////////////////////////////////////////////////////

		ATGOR_CreatureCharacter(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

		virtual void BeginPlay() override;
		virtual void GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const override;
		virtual void Tick(float DeltaTime) override;
		virtual void Starting_Implementation() override;

		//////////////////////////////////////////////// IMPLEMENTABLES /////////////////////////////////////////
		
		/** Called when initially spawned. */
		UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "TGOR|System", Meta = (Keywords = "C++"))
			void InitialSpawn();

		/** Called when an externally opened inventory got closed. */
		UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "TGOR|System", Meta = (Keywords = "C++"))
			void ExternalClosed();

		////////////////////////////////////////////// COMPONENTS //////////////////////////////////////////////////////

		UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "TGOR|Creature", meta = (AllowPrivateAccess = "true"))
			UTGOR_CharacterMeshComponent* CharacterMeshComponent;

		UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "TGOR|Creature", meta = (AllowPrivateAccess = "true"))
			UTGOR_InteractableComponent* SelfInteractable;

		////////////////////////////////////////////////////////////////////////////////////////////////////

		/** Socket names for attaching the WeaponActor to the character model. */
		UPROPERTY(BlueprintReadWrite, Category = "TGOR|Combat")
			FName WeaponSocketLeftShoulder;

		UPROPERTY(BlueprintReadWrite, Category = "TGOR|Combat")
			FName WeaponSocketRightShoulder;

		UPROPERTY(BlueprintReadWrite, Category = "TGOR|Combat")
			FName WeaponSocketLeftHand;

		UPROPERTY(BlueprintReadWrite, Category = "TGOR|Combat")
			FName WeaponSocketRightHand;

		UPROPERTY(BlueprintReadWrite, Category = "TGOR|Combat")
			FName WeaponSocketLeftLowerArm;

		UPROPERTY(BlueprintReadWrite, Category = "TGOR|Combat")
			FName WeaponSocketRightLowerArm;

		/////////////////////////////////////////////// INTERNAL ///////////////////////////////////////////

protected:

private:

};