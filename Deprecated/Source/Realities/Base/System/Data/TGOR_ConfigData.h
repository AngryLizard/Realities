// The Gateway of Realities: Planes of Existence.

#pragma once
#include "Realities/Utility/Storage/TGOR_Buffer.h"

#include "TGOR_Data.h"
#include "TGOR_ConfigData.generated.h"

/**
* TGOR_UserMapData allows storing a players local accounts
*/
USTRUCT(BlueprintType)
struct REALITIES_API FTGOR_UserMapData
{
	GENERATED_USTRUCT_BODY()
		FTGOR_UserMapData();

	void Write(FTGOR_GroupWritePackage& Package, UTGOR_Singleton* Context) const;
	void Read(FTGOR_GroupReadPackage& Package, UTGOR_Singleton* Context);

	UPROPERTY(BlueprintReadWrite, Category = "TGOR Config")
		int32 UserKey;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "TGOR Game", Meta = (Keywords = "C++"))
		FTGOR_Buffer PrivateKey;
};

/**
* TGOR_ConfigData allows storing configurations and the local player's user accounts
*/
UCLASS(BlueprintType)
class REALITIES_API UTGOR_ConfigData : public UTGOR_Data
{
	GENERATED_BODY()
public:
	UTGOR_ConfigData();

	virtual void Write_Implementation(FTGOR_GroupWritePackage& Package, UTGOR_Singleton* Context) const override;
	virtual bool Read_Implementation(FTGOR_GroupReadPackage& Package, UTGOR_Singleton* Context) override;


	////////////////////////////////////////////////////////////////////////////////////////////////////
	
	/** Gets user map data */
	UFUNCTION(BlueprintCallable, Category = "TGOR Config", Meta = (Keywords = "C++"))
		FTGOR_UserMapData& GetUserData();

private:
	UPROPERTY()
		FTGOR_UserMapData UserData;
};
