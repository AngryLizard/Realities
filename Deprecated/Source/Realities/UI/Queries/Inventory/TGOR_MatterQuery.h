// The Gateway of Realities: Planes of Existence.

#pragma once

#include "CoreMinimal.h"

#include "Realities/UI/Queries/TGOR_ContentQuery.h"
#include "TGOR_MatterQuery.generated.h"

class UTGOR_Matter;
class UTGOR_MatterComponent;

/**
*
*/
USTRUCT(BlueprintType)
struct REALITIES_API FTGOR_MatterPair
{
	GENERATED_USTRUCT_BODY()

		FTGOR_MatterPair();
	bool operator<(const FTGOR_MatterPair& Other) const;

	////////////////////////////////////////////////////////////////////////////////////////////////////

	/** */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TGOR Inventory")
		UTGOR_Matter* Matter;

	/** */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TGOR Inventory")
		int32 Quantity;

	/** */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TGOR Inventory")
		int32 Capacity;

};

/**
 * 
 */
UCLASS()
class REALITIES_API UTGOR_MatterQuery : public UTGOR_ContentQuery
{
	GENERATED_BODY()
	
public:
	UTGOR_MatterQuery();
	virtual TArray<UTGOR_Content*> QueryContent() const override;

	/** Construct matter list from a map */
	UFUNCTION(BlueprintCallable, Category = "TGOR Inventory", Meta = (Keywords = "C++"))
		void AssignMap(const TMap<UTGOR_Matter*, int32>& Matter);

	/** Add capacity mapping from a matter module */
	UFUNCTION(BlueprintCallable, Category = "TGOR Inventory", Meta = (Keywords = "C++"))
		void AssignCapacities(UTGOR_MatterComponent* Component);

	/** Add matter and capacity mapping from an item storage */
	UFUNCTION(BlueprintCallable, Category = "TGOR Inventory", Meta = (Keywords = "C++"))
		void AssignStorage(UTGOR_ItemStorage* Storage);

	UPROPERTY(BlueprintReadOnly, Category = "TGOR Inventory")
		TArray<FTGOR_MatterPair> Instances;
};
