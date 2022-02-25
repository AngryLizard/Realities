// TGOR (C) // CHECKED //
#pragma once

#include "CoreMinimal.h"
#include "Realities/Utility/Datastructures/TGOR_Time.h"
#include "Realities/Utility/TGOR_CustomEnumerations.h"

#include "Realities/Components/Inventory/TGOR_ProcessComponent.h"
#include "TGOR_ScienceComponent.generated.h"


////////////////////////////////////////////// DECL //////////////////////////////////////////////////////

class UTGOR_Science;
class UTGOR_ItemStorage;
class UTGOR_Item;


///////////////////////////////////////////// STRUCTS ///////////////////////////////////////////////////


DECLARE_DYNAMIC_MULTICAST_DELEGATE(FScienceTerminalDelegate);

/**
 * TGOR_ScienceComponent allows storing science ingredients
 */
UCLASS(ClassGroup = (Custom), Blueprintable, meta = (BlueprintSpawnableComponent))
class REALITIES_API UTGOR_ScienceComponent : public UTGOR_ProcessComponent
{
	GENERATED_BODY()

public:

	////////////////////////////////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////// ENGINE ////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////////////////////////////

	UTGOR_ScienceComponent();

	void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	void GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const override;

	//////////////////////////////////////////// IMPLEMENTABLES ////////////////////////////////////////

	////////////////////////////////////////////////////////////////////////////////////////////////////
protected:

	/** Energy/second when sciencing items  */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "TGOR Inventory")
		float ScienceRate;

	/** input sockets to consider for science */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TGOR Inventory")
		TArray<TSubclassOf<UTGOR_NamedSocket>> InputSockets;

	////////////////////////////////////////////////////////////////////////////////////////////////////
protected:

	/** Currently active science recipe. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "TGOR Inventory")
		UTGOR_Science* Recipe;

	////////////////////////////////////////////////////////////////////////////////////////////////////
public:




	/////////////////////////////////////////////// INTERNAL ///////////////////////////////////////////

protected:

private:

};
