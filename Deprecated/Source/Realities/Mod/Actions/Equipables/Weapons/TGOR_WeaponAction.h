// TGOR (C) // CHECKED //
#pragma once

#include "Realities/Utility/TGOR_CustomEnumerations.h"
#include "Realities/Actors/Combat/HitVolumes/TGOR_HitVolume.h"

#include "Realities/Mod/Actions/Equipables/TGOR_EquipableAction.h"
#include "TGOR_WeaponAction.generated.h"

////////////////////////////////////////////// DECL //////////////////////////////////////////////////////

class UTGOR_Bodypart;

/**
*
*/
UCLASS()
class REALITIES_API UTGOR_WeaponAction : public UTGOR_EquipableAction
{
	GENERATED_BODY()

public:

	////////////////////////////////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////// ENGINE ////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////////////////////////////

	UTGOR_WeaponAction();

	//////////////////////////////////////////// IMPLEMENTABLES ////////////////////////////////////////

	/** */

	////////////////////////////////////////////////////////////////////////////////////////////////////
public:

	/** Get current aim in world space according to owning pawn AimComponent */
	UFUNCTION(BlueprintPure, Category = "TGOR Action", Meta = (Keywords = "C++"))
		FVector GetCurrentAimLocation(UTGOR_ActionComponent* Component) const;

	////////////////////////////////////////////////////////////////////////////////////////////////////
protected:

	/** Shoot/Spawn HitVolume of given type  */
	UFUNCTION(BlueprintCallable, Category = "TGOR Weapon", Meta = (Keywords = "C++"))
		void ShootHitVolume(UTGOR_ActionComponent* Component, TSubclassOf<ATGOR_HitVolume> Type, FVector Location, FRotator Rotation, const FTGOR_ForceInstance& Forces);


	////////////////////////////////////////////////////////////////////////////////////////////////////
protected:

	/** Display a given bodypart type on the current avatar skeletal component */
	UFUNCTION(BlueprintCallable, Category = "TGOR Weapon", Meta = (Keywords = "C++"))
		void RegisterBodypart(UTGOR_ActionComponent* Component, TSubclassOf<UTGOR_Bodypart> Type);

	/** Remove a given bodypart type from the current avatar skeletal component */
	UFUNCTION(BlueprintCallable, Category = "TGOR Weapon", Meta = (Keywords = "C++"))
		void UnregisterBodypart(UTGOR_ActionComponent* Component);

	////////////////////////////////////////////////////////////////////////////////////////////////////
private:

};
